#include "surveyors_post.h"

#include "building/distribution.h"
#include "building/type.h"
#include "scenario/property.h"

#define MAX_DISTANCE 40
#define MAX_GOOD 600

int building_surveyors_post_get_needed_inventory(building* surveyors_post)
{
    int needed = INVENTORY_FLAG_NONE;
    if (building_distribution_is_good_accepted(INVENTORY_CLAY, surveyors_post)) {
        inventory_set(&needed, INVENTORY_CLAY);
    }
    if (building_distribution_is_good_accepted(INVENTORY_TIMBER, surveyors_post)) {
        inventory_set(&needed, INVENTORY_TIMBER);
    }
    if (building_distribution_is_good_accepted(INVENTORY_IRON, surveyors_post)) {
        inventory_set(&needed, INVENTORY_IRON);
    }
    if (building_distribution_is_good_accepted(INVENTORY_MARBLE, surveyors_post)) {
        inventory_set(&needed, INVENTORY_MARBLE);
    }

    return needed;
}

int building_surveyors_post_fetch_inventory(building* surveyors_post, inventory_storage_info* info, int needed_inventory)
{
    // Prefer whichever good we don't have
    int fetch_inventory = building_distribution_fetch(surveyors_post, info, 0, 1, needed_inventory);
    if (fetch_inventory != INVENTORY_NONE) {
        return fetch_inventory;
    }
    // Then prefer smallest stock below baseline stock
    fetch_inventory = building_distribution_fetch(surveyors_post, info, BASELINE_STOCK, 0, needed_inventory);
    if (fetch_inventory != INVENTORY_NONE) {
        return fetch_inventory;
    }
    // All items well stocked: pick raw below threshold
    fetch_inventory = building_distribution_fetch(surveyors_post, info, MAX_GOOD, 0,
        needed_inventory & INVENTORY_FLAG_ALL_RAW);
    if (fetch_inventory != INVENTORY_NONE) {
        return fetch_inventory;
    }
    return INVENTORY_NONE;
}

int building_surveyors_post_get_storage_destination(building* surveyors_post)
{
    int needed_inventory = building_surveyors_post_get_needed_inventory(surveyors_post);
    if (needed_inventory == INVENTORY_FLAG_NONE) {
        return 0;
    }
    inventory_storage_info info[INVENTORY_MAX];
    if (!building_distribution_get_inventory_storages(info, BUILDING_SURVEYORS_POST,
        surveyors_post->road_network_id, surveyors_post->road_access_x, surveyors_post->road_access_y, MAX_DISTANCE)) {
        return 0;
    }
    int fetch_inventory = building_surveyors_post_fetch_inventory(surveyors_post, info, needed_inventory);
    if (fetch_inventory == INVENTORY_NONE) {
        return 0;
    }
    surveyors_post->data.market.fetch_inventory_id = fetch_inventory;
    return info[fetch_inventory].building_id;
}


