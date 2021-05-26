#include "action.h"

#include "city/entertainment.h"
#include "city/figures.h"
#include "figure/figure.h"
#include "figuretype/animal.h"
#include "figuretype/cartpusher.h"
#include "figuretype/crime.h"
#include "figuretype/docker.h"
#include "figuretype/editor.h"
#include "figuretype/enemy.h"
#include "figuretype/entertainer.h"
#include "figuretype/maintenance.h"
#include "figuretype/migrant.h"
#include "figuretype/missile.h"
#include "figuretype/native.h"
#include "figuretype/service.h"
#include "figuretype/soldier.h"
#include "figuretype/supplier.h"
#include "figuretype/trader.h"
#include "figuretype/wall.h"
#include "figuretype/water.h"
#include "figuretype/workcamp.h"


static void figure_nobody_action(figure *f)
{}

static void (*figure_action_callbacks[])(figure *f) = {
    figure_nobody_action, //0
    figure_immigrant_action,
    figure_emigrant_action,
    figure_homeless_action,
    figure_cartpusher_action,
    figure_labor_seeker_action,
    figure_explosion_cloud_action,
    figure_tax_collector_action,
    figure_engineer_action,
    figure_warehouseman_action,
    figure_prefect_action, //10
    figure_soldier_action,
    figure_soldier_action,
    figure_soldier_action,
    figure_military_standard_action,
    figure_entertainer_action,
    figure_entertainer_action,
    figure_entertainer_action,
    figure_entertainer_action,
    figure_trade_caravan_action,
    figure_trade_ship_action, //20
    figure_trade_caravan_donkey_action,
    figure_protestor_action,
    figure_rioter_action,
    figure_rioter_action,
    figure_fishing_boat_action,
    figure_market_trader_action,
    figure_priest_action,
    figure_school_child_action,
    figure_teacher_action,
    figure_librarian_action, //30
    figure_barber_action,
    figure_bathhouse_worker_action,
    figure_doctor_action,
    figure_doctor_action,
    figure_worker_action,
    figure_editor_flag_action,
    figure_flotsam_action,
    figure_docker_action,
    figure_supplier_action,
    figure_patrician_action, //40
    figure_indigenous_native_action,
    figure_tower_sentry_action,
    figure_enemy43_spear_action,
    figure_enemy44_sword_action,
    figure_enemy45_sword_action,
    figure_enemy_camel_action,
    figure_enemy_elephant_action,
    figure_enemy_chariot_action,
    figure_enemy49_fast_sword_action,
    figure_enemy50_sword_action, //50
    figure_enemy51_spear_action,
    figure_enemy52_mounted_archer_action,
    figure_enemy53_axe_action,
    figure_enemy_gladiator_action,
    figure_nobody_action,
    figure_nobody_action,
    figure_enemy_caesar_legionary_action,
    figure_native_trader_action,
    figure_arrow_action,
    figure_javelin_action, //60
    figure_bolt_action,
    figure_ballista_action,
    figure_nobody_action,
    figure_missionary_action,
    figure_seagulls_action,
    figure_delivery_boy_action,
    figure_shipwreck_action,
    figure_sheep_action,
    figure_wolf_action,
    figure_zebra_action, //70
    figure_spear_action,
    figure_hippodrome_horse_action,
    figure_workcamp_worker_action,
    figure_workcamp_slave_action,
    figure_workcamp_engineer_action,
    figure_supplier_action,
    figure_delivery_boy_action,
    figure_supplier_action,
    figure_tavern_action,
    figure_supplier_action, // 80
    figure_tourist_action,
    figure_watchman_action,
    figure_watchtower_archer_action,
    figure_friendly_arrow_action,
    figure_supplier_action,
    figure_robber_action,
    figure_looter_action,
    figure_delivery_boy_action,
    figure_supplier_action,
    figure_market_trader_action, // 90
    figure_supplier_action
};

void figure_action_handle(void)
{
    city_figures_reset();
    city_entertainment_set_hippodrome_has_race(0);
    for (int i = 1; i < figure_count(); i++) {
        figure *f = figure_get(i);
        if (f->state) {
            if (f->targeted_by_figure_id) {
                figure *attacker = figure_get(f->targeted_by_figure_id);
                if (attacker->state != FIGURE_STATE_ALIVE) {
                    f->targeted_by_figure_id = 0;
                }
                if (attacker->target_figure_id != i) {
                    f->targeted_by_figure_id = 0;
                }
            }
            figure_action_callbacks[f->type](f);
            if (f->state == FIGURE_STATE_DEAD) {
                figure_delete(f);
            }
        }
    }
}
