#ifndef BUILDING_SURVEYORS_POST_H
#define BUILDING_SURVEYORS_POST_H

#include "building/distribution.h"

int building_surveyors_post_get_max_goods_stock(building* market);

int building_surveyors_post_get_needed_inventory(building* market);
int building_surveyors_post_fetch_inventory(building* market, inventory_storage_info* data, int needed_inventory);
int building_surveyors_post_get_storage_destination(building* market);


#endif // BUILDING_SURVEYORS_POST_H