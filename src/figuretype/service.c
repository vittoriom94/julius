#include "service.h"

#include "assets/assets.h"
#include "building/building.h"
#include "building/market.h"
#include "core/config.h"
#include "core/image.h"
#include "figure/combat.h"
#include "figure/image.h"
#include "figure/movement.h"
#include "figure/route.h"
#include "figuretype/supplier.h"
#include "map/building.h"
#include "map/road_access.h"

static void roamer_action(figure *f, int num_ticks)
{
    switch (f->action_state) {
        case FIGURE_ACTION_150_ATTACK:
            figure_combat_handle_attack(f);
            break;
        case FIGURE_ACTION_149_CORPSE:
            figure_combat_handle_corpse(f);
            break;
        case FIGURE_ACTION_125_ROAMING:
            f->is_ghost = 0;
            f->roam_length++;
            if (f->roam_length >= f->max_roam_length) {
                int x, y;
                building *b = building_get(f->building_id);
                if (map_closest_road_within_radius(b->x, b->y, b->size, 2, &x, &y)) {
                    f->action_state = FIGURE_ACTION_126_ROAMER_RETURNING;
                    f->destination_x = x;
                    f->destination_y = y;
                    figure_route_remove(f);
                    f->roam_length = 0;
                } else {
                    f->state = FIGURE_STATE_DEAD;
                }
            }
            figure_movement_roam_ticks(f, num_ticks);
            break;
        case FIGURE_ACTION_126_ROAMER_RETURNING:
            figure_movement_move_ticks(f, num_ticks);
            if (f->direction == DIR_FIGURE_AT_DESTINATION ||
                f->direction == DIR_FIGURE_REROUTE || f->direction == DIR_FIGURE_LOST) {
                f->state = FIGURE_STATE_DEAD;
            }
            break;
    }
}

static void culture_action(figure *f, int group)
{
    f->terrain_usage = TERRAIN_USAGE_ROADS;
    f->use_cross_country = 0;
    f->max_roam_length = 384;
    building *b = building_get(f->building_id);
    if (b->state != BUILDING_STATE_IN_USE || b->figure_id != f->id) {
        f->state = FIGURE_STATE_DEAD;
    }
    figure_image_increase_offset(f, 12);
    roamer_action(f, 1);
    figure_image_update(f, image_group(group));
}

void figure_destination_priest_action(figure *f)
{
    building *b = building_get(f->building_id);
    building *destination = building_get(f->destination_building_id);
    f->terrain_usage = TERRAIN_USAGE_ROADS;
    if (b->state != BUILDING_STATE_IN_USE || (b->figure_id4 != f->id && b->figure_id2 != f->id) || destination->state != BUILDING_STATE_IN_USE) {
        f->state = FIGURE_STATE_DEAD;
    }

    switch (f->action_state) {
        case FIGURE_ACTION_150_ATTACK:
            figure_combat_handle_attack(f);
            break;
        case FIGURE_ACTION_149_CORPSE:
            figure_combat_handle_corpse(f);
            break;
        case FIGURE_ACTION_212_DESTINATION_PRIEST_CREATED:
            f->destination_x = destination->road_access_x;
            f->destination_y = destination->road_access_y;
            f->action_state = FIGURE_ACTION_213_PRIEST_GOING_TO_PANTHEON;

            break;
        case FIGURE_ACTION_214_DESTINATION_MARS_PRIEST_CREATED:
            f->destination_x = destination->road_access_x;
            f->destination_y = destination->road_access_y;
            int market_units = b->data.market.inventory[f->collecting_item_id];
            int num_loads;
            int max_units = MAX_FOOD_STOCKED_MESS_HALL - market_units;

            if (market_units >= 800) {
                num_loads = 8;
            }         else if (market_units >= 700) {
                num_loads = 7;
            }         else if (market_units >= 600) {
                num_loads = 6;
            }         else if (market_units >= 500) {
                num_loads = 5;
            }         else if (market_units >= 400) {
                num_loads = 4;
            }         else if (market_units >= 300) {
                num_loads = 3;
            }         else if (market_units >= 200) {
                num_loads = 2;
            }         else if (market_units >= 100) {
                num_loads = 1;
            }         else {
                num_loads = 0;
            }
            if (num_loads > max_units / 100) {
                num_loads = max_units / 100;
            }
            if (num_loads <= 0) {
                return;
            }

            b->data.market.inventory[f->collecting_item_id] -= (100 * num_loads);

            // create delivery boys
            int priest_id = f->id;
            int previous_boy = f->id;
            for (int i = 0; i < num_loads; i++) {
                previous_boy = figure_supplier_create_delivery_boy(previous_boy, priest_id, FIGURE_DELIVERY_BOY);
            }
            f = figure_get(priest_id);

            f->action_state = FIGURE_ACTION_215_PRIEST_GOING_TO_MESS_HALL;
            break;

        case FIGURE_ACTION_213_PRIEST_GOING_TO_PANTHEON:
            figure_movement_move_ticks(f, 1);
            if (f->direction == DIR_FIGURE_AT_DESTINATION) {
                f->state = FIGURE_STATE_DEAD;
            }         else if (f->direction == DIR_FIGURE_REROUTE) {
                figure_route_remove(f);
            }         else if (f->direction == DIR_FIGURE_LOST) {
                f->state = FIGURE_STATE_DEAD;
            }
            break;
        case FIGURE_ACTION_215_PRIEST_GOING_TO_MESS_HALL:
            f->terrain_usage = TERRAIN_USAGE_PREFER_ROADS;
            figure_movement_move_ticks(f, 1);
            if (f->direction == DIR_FIGURE_AT_DESTINATION) {
                f->state = FIGURE_STATE_DEAD;
            }         else if (f->direction == DIR_FIGURE_REROUTE) {
                figure_route_remove(f);
            }         else if (f->direction == DIR_FIGURE_LOST) {
                f->state = FIGURE_STATE_DEAD;
            }
            break;
    }

    figure_image_increase_offset(f, 12);
    figure_image_update(f, image_group(GROUP_FIGURE_PRIEST));
}


void figure_priest_action(figure *f)
{
    if (f->destination_building_id) {
        figure_destination_priest_action(f);
    } else {
        culture_action(f, GROUP_FIGURE_PRIEST);
    }
}

void figure_school_child_action(figure *f)
{
    f->terrain_usage = TERRAIN_USAGE_ROADS;
    f->use_cross_country = 0;
    f->max_roam_length = 192;

    building *b = building_get(f->building_id);
    if (b->state != BUILDING_STATE_IN_USE || b->type != BUILDING_SCHOOL) {
        f->state = FIGURE_STATE_DEAD;
    }
    figure_image_increase_offset(f, 12);
    switch (f->action_state) {
        case FIGURE_ACTION_150_ATTACK:
            figure_combat_handle_attack(f);
            break;
        case FIGURE_ACTION_149_CORPSE:
            figure_combat_handle_corpse(f);
            break;
        case FIGURE_ACTION_125_ROAMING:
            f->is_ghost = 0;
            f->roam_length++;
            if (f->roam_length >= f->max_roam_length) {
                f->state = FIGURE_STATE_DEAD;
            }
            figure_movement_roam_ticks(f, 2);
            break;
    }
    figure_image_update(f, image_group(GROUP_FIGURE_SCHOOL_CHILD));
}

void figure_teacher_action(figure *f)
{
    culture_action(f, GROUP_FIGURE_TEACHER_LIBRARIAN);
}

void figure_librarian_action(figure *f)
{
    culture_action(f, GROUP_FIGURE_TEACHER_LIBRARIAN);
}

void figure_barber_action(figure *f)
{
    culture_action(f, GROUP_FIGURE_BARBER);
}

void figure_bathhouse_worker_action(figure *f)
{
    culture_action(f, GROUP_FIGURE_BATHHOUSE_WORKER);
}

void figure_tavern_action(figure *f)
{
    f->terrain_usage = TERRAIN_USAGE_ROADS;
    f->use_cross_country = 0;
    f->max_roam_length = 384;
    building *b = building_get(f->building_id);
    if (b->state != BUILDING_STATE_IN_USE || b->figure_id != f->id) {
        f->state = FIGURE_STATE_DEAD;
    }
    figure_image_increase_offset(f, 12);
    roamer_action(f, 1);
    int dir = figure_image_normalize_direction(f->direction < 8 ? f->direction : f->previous_tile_direction);
    if (f->action_state == FIGURE_ACTION_149_CORPSE) {
        f->image_id = assets_get_image_id(assets_get_group_id("Areldir", "Entertainment"), "Barkeep Death 01") +
            figure_image_corpse_offset(f);
    } else {
        f->image_id = assets_get_image_id(assets_get_group_id("Areldir", "Entertainment"), "Barkeep NE 01") + dir * 12 +
            f->image_offset;
    }
}

void figure_doctor_action(figure *f)
{
    culture_action(f, GROUP_FIGURE_DOCTOR_SURGEON);
}

void figure_missionary_action(figure *f)
{
    f->terrain_usage = TERRAIN_USAGE_ROADS;
    f->use_cross_country = 0;
    f->max_roam_length = 192;
    building *b = building_get(f->building_id);
    if (b->state != BUILDING_STATE_IN_USE || b->figure_id != f->id) {
        f->state = FIGURE_STATE_DEAD;
    }
    figure_image_increase_offset(f, 12);
    roamer_action(f, 1);
    figure_image_update(f, image_group(GROUP_FIGURE_MISSIONARY));
}

void figure_patrician_action(figure *f)
{
    f->terrain_usage = TERRAIN_USAGE_ROADS;
    f->use_cross_country = 0;
    f->max_roam_length = 128;
    if (building_get(f->building_id)->state != BUILDING_STATE_IN_USE) {
        f->state = FIGURE_STATE_DEAD;
    }
    figure_image_increase_offset(f, 12);
    roamer_action(f, 1);
    figure_image_update(f, image_group(GROUP_FIGURE_PATRICIAN));
}

void figure_labor_seeker_action(figure *f)
{
    f->terrain_usage = TERRAIN_USAGE_ROADS;
    f->use_cross_country = 0;
    f->max_roam_length = 384;
    building *b = building_get(f->building_id);
    if (b->state != BUILDING_STATE_IN_USE || b->figure_id2 != f->id) {
        f->state = FIGURE_STATE_DEAD;
    }
    figure_image_increase_offset(f, 12);
    roamer_action(f, 1);
    figure_image_update(f, image_group(GROUP_FIGURE_LABOR_SEEKER));
}

void figure_market_trader_action(figure *f)
{
    f->terrain_usage = TERRAIN_USAGE_ROADS;
    f->use_cross_country = 0;
    f->max_roam_length = 384;
    building *market = building_get(f->building_id);
    if (market->state != BUILDING_STATE_IN_USE || market->figure_id != f->id) {
        f->state = FIGURE_STATE_DEAD;
    }
    figure_image_increase_offset(f, 12);
    if (f->action_state == FIGURE_ACTION_125_ROAMING) {
        // force return on out of stock
        int stock = building_market_get_max_food_stock(market) +
            building_market_get_max_goods_stock(market);
        if (f->roam_length >= 96 && stock <= 0) {
            f->roam_length = f->max_roam_length;
        }
    }
    roamer_action(f, 1);
    figure_image_update(f, image_group(GROUP_FIGURE_MARKET_LADY));
}

void figure_surveyors_post_action(figure *f)
{
    f->terrain_usage = TERRAIN_USAGE_ROADS;
    f->use_cross_country = 0;
    f->max_roam_length = 384;
    building *market = building_get(f->building_id);
    if (market->state != BUILDING_STATE_IN_USE || market->figure_id != f->id) {
        f->state = FIGURE_STATE_DEAD;
    }
    figure_image_increase_offset(f, 12);
    roamer_action(f, 1);
    int dir = figure_image_normalize_direction(f->direction < 8 ? f->direction : f->previous_tile_direction);
    if (f->action_state == FIGURE_ACTION_149_CORPSE) {
        f->image_id = assets_get_image_id(assets_get_group_id("Areldir", "Slave_Walker"),
                                            "Slave death 01") + figure_image_corpse_offset(f);
    } else {
        f->image_id = assets_get_image_id(assets_get_group_id("Areldir", "Slave_Walker"),
                                            "Slave NE 01") + dir * 12 + f->image_offset;
    }
}

void figure_tax_collector_action(figure *f)
{
    building *b = building_get(f->building_id);

    f->terrain_usage = TERRAIN_USAGE_ROADS;
    f->use_cross_country = 0;
    f->max_roam_length = 512;
    if (b->state != BUILDING_STATE_IN_USE || b->figure_id != f->id) {
        f->state = FIGURE_STATE_DEAD;
    }
    figure_image_increase_offset(f, 12);

    switch (f->action_state) {
        case FIGURE_ACTION_150_ATTACK:
            figure_combat_handle_attack(f);
            break;
        case FIGURE_ACTION_149_CORPSE:
            figure_combat_handle_corpse(f);
            break;
        case FIGURE_ACTION_40_TAX_COLLECTOR_CREATED:
            f->is_ghost = 1;
            f->image_offset = 0;
            f->wait_ticks--;
            if (f->wait_ticks <= 0) {
                int x_road, y_road;
                if (map_closest_road_within_radius(b->x, b->y, b->size, 2, &x_road, &y_road)) {
                    f->action_state = FIGURE_ACTION_41_TAX_COLLECTOR_ENTERING_EXITING;
                    figure_movement_set_cross_country_destination(f, x_road, y_road);
                    f->roam_length = 0;
                } else {
                    f->state = FIGURE_STATE_DEAD;
                }
            }
            break;
        case FIGURE_ACTION_41_TAX_COLLECTOR_ENTERING_EXITING:
            f->use_cross_country = 1;
            f->is_ghost = 1;
            if (figure_movement_move_ticks_cross_country(f, 1) == 1) {
                if (map_building_at(f->grid_offset) == f->building_id) {
                    // returned to own building
                    f->state = FIGURE_STATE_DEAD;
                } else {
                    f->action_state = FIGURE_ACTION_42_TAX_COLLECTOR_ROAMING;
                    figure_movement_init_roaming(f);
                    f->roam_length = 0;
                }
            }
            break;
        case FIGURE_ACTION_42_TAX_COLLECTOR_ROAMING:
            f->is_ghost = 0;
            f->roam_length++;
            if (f->roam_length >= f->max_roam_length) {
                int x_road, y_road;
                if (map_closest_road_within_radius(b->x, b->y, b->size, 2, &x_road, &y_road)) {
                    f->action_state = FIGURE_ACTION_43_TAX_COLLECTOR_RETURNING;
                    f->destination_x = x_road;
                    f->destination_y = y_road;
                } else {
                    f->state = FIGURE_STATE_DEAD;
                }
            }
            figure_movement_roam_ticks(f, 1);
            break;
        case FIGURE_ACTION_43_TAX_COLLECTOR_RETURNING:
            figure_movement_move_ticks(f, 1);
            if (f->direction == DIR_FIGURE_AT_DESTINATION) {
                f->action_state = FIGURE_ACTION_41_TAX_COLLECTOR_ENTERING_EXITING;
                figure_movement_set_cross_country_destination(f, b->x, b->y);
                f->roam_length = 0;
            } else if (f->direction == DIR_FIGURE_REROUTE || f->direction == DIR_FIGURE_LOST) {
                f->state = FIGURE_STATE_DEAD;
            }
            break;
    }
    figure_image_update(f, image_group(GROUP_FIGURE_TAX_COLLECTOR));
}
