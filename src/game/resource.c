#include "resource.h"

#include "building/type.h"
#include "scenario/building.h"

int resource_image_offset(resource_type resource, resource_image_type type)
{
    if (resource == RESOURCE_MEAT && scenario_building_allowed(BUILDING_WHARF)) {
        switch (type) {
            case RESOURCE_IMAGE_STORAGE: return 40;
            case RESOURCE_IMAGE_CART: return 648;
            case RESOURCE_IMAGE_FOOD_CART: return 8;
            case RESOURCE_IMAGE_ICON: return 11;
            default: return 0;
        }
    } else {
        return 0;
    }
}

int resource_is_food(resource_type resource)
{
    return resource == RESOURCE_WHEAT || resource == RESOURCE_VEGETABLES ||
        resource == RESOURCE_FRUIT || resource == RESOURCE_MEAT;
}

workshop_type resource_to_workshop_type(resource_type resource)
{
    switch (resource) {
        case RESOURCE_OLIVES:
            return WORKSHOP_OLIVES_TO_OIL;
        case RESOURCE_VINES:
            return WORKSHOP_VINES_TO_WINE;
        case RESOURCE_IRON:
            return WORKSHOP_IRON_TO_WEAPONS;
        case RESOURCE_TIMBER:
            return WORKSHOP_TIMBER_TO_FURNITURE;
        case RESOURCE_CLAY:
            return WORKSHOP_CLAY_TO_POTTERY;
        default:
            return WORKSHOP_NONE;
    }
}

int inventory_is_set(int inventory, int flag)
{
    return (inventory >> flag) & 1;
}

void inventory_set(int *inventory, int flag)
{
    *inventory |= 1 << flag; 
}

int resource_from_inventory(int inventory_id)
{
    switch (inventory_id) {
        case INVENTORY_WHEAT: return RESOURCE_WHEAT;
        case INVENTORY_VEGETABLES: return RESOURCE_VEGETABLES;
        case INVENTORY_FRUIT: return RESOURCE_FRUIT;
        case INVENTORY_MEAT: return RESOURCE_MEAT;
        case INVENTORY_POTTERY: return RESOURCE_POTTERY;
        case INVENTORY_FURNITURE: return RESOURCE_FURNITURE;
        case INVENTORY_OIL: return RESOURCE_OIL;
        case INVENTORY_WINE: return RESOURCE_WINE;
        case INVENTORY_CLAY: return RESOURCE_CLAY;
        case INVENTORY_TIMBER: return RESOURCE_TIMBER;
        case INVENTORY_IRON: return RESOURCE_IRON;
        case INVENTORY_MARBLE: return RESOURCE_MARBLE;
        default: return RESOURCE_NONE;
    } 
}

int resource_to_inventory(resource_type resource)
{
    switch (resource) {
        case RESOURCE_WHEAT: return INVENTORY_WHEAT;
        case RESOURCE_VEGETABLES: return INVENTORY_VEGETABLES;
        case RESOURCE_FRUIT: return INVENTORY_FRUIT;
        case RESOURCE_MEAT: return INVENTORY_MEAT;
        case RESOURCE_POTTERY: return INVENTORY_POTTERY;
        case RESOURCE_FURNITURE: return INVENTORY_FURNITURE;
        case RESOURCE_OIL: return INVENTORY_OIL;
        case RESOURCE_WINE: return INVENTORY_WINE;
        case RESOURCE_CLAY: return INVENTORY_CLAY;
        case RESOURCE_TIMBER: return INVENTORY_TIMBER;
        case RESOURCE_IRON: return INVENTORY_IRON;
        case RESOURCE_MARBLE: return INVENTORY_MARBLE;
        default: return INVENTORY_NONE;
    } 
}
