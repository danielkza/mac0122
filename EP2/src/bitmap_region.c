#include <stdlib.h>
#include <stdio.h>

#include "linked_list.h"

#include "bitmap.h"
#include "bitmap_region.h"

static int
bitmap_find_region_scan_line__(bitmap* map,
                               size_t start_x,
                               size_t start_y,
                               int delta_x,
                               int delta_y,
                               bitmap_region* region)
{
    size_t cur_x, cur_y;
    size_t count;

    if(map == NULL || region == NULL)
        return 0;

    if(start_x >= map->width || start_y >= map->height)
        return 0;

    // Ugly but necessary check: we need to start at an incremented position
    // but can't allow unsigned wrapping
    if(delta_x < 0 && start_x < (unsigned)abs(delta_x)
       || delta_y < 0 && start_y < (unsigned)abs(delta_y))
    {
        return 0;
    }

    cur_x = start_x + delta_x;
    cur_y = start_y + delta_y;

    count = 0;

    // bitmap_getbit will return 0 if we exceed the upper bounds, but we must
    // check the lower bound ourselves due to possible unsigned wrapping
    while(bitmap_getbit(map, cur_x, cur_y) == 1) {
        count++;

        if(delta_x < 0 && cur_x < (unsigned)abs(delta_x)
           || delta_y < 0 && cur_y < (unsigned)abs(delta_y))
        {
            break;
        }

        cur_x += delta_x;
        cur_y += delta_y;
    }

    return count;
}

void
bitmap_find_region(bitmap* map,
                   size_t start_x,
                   size_t start_y,
                   bitmap_region* region)
{
    struct {
        int x, y;
    } const deltas[] = {
        {+1,  0}, // right>
        {-1,  0}, // left
        { 0, +1}, // up
        { 0, -1}, // down
        /*
        {+1, +1}, // you can figure out the rest
        {+1, -1},
        {-1, +1},
        {-1, -1},
        */
    };

    size_t i;
    bitmap_region* region_entry;

    if(map == NULL || region == NULL)
        return;

    if(start_x >= map->width || start_y >= map->height)
        return;
    
    /* We use an optimized recursive approach to flood filling (finding a
     * region):
     * - We check if the starting bit itself is set. If it is not we have
     *   nothing to do.
     * - If it is actually set, we unset the bit so it is not found again by 
     *   any other calls.
     * - We then walk in a 'cross' pattern around the starting bit, extending 
     *   as further as possible on each direction. This works better than doing
     *   another call for each pixel around us by splitting some of the
     *   recursion into a linear loop, saving a significant amount of stack
     *   space for large regions. At least it's not always
     *   O(number of set bits) anymore, it's lower (trust me on this).
     */

    if(bitmap_getbit(map, start_x, start_y) == 0)
        return;

    bitmap_setbit(map, start_x, start_y, 0);

    region_entry = (bitmap_region*)malloc(sizeof(*region_entry));
    if(region_entry == NULL)
        return;

    region_entry->x = start_x;
    region_entry->y = start_y;

    linked_list_push(&(region_entry->list), &(region->list));

    for(i = 0; i < sizeof(deltas) / sizeof(deltas[0]); i++) {
        size_t scan_line_count, scan_cur;
              
        scan_line_count =
        bitmap_find_region_scan_line__(map, start_x, start_y,
                                       deltas[i].x, deltas[i].y, region);

        for(scan_cur = 1; scan_cur <= scan_line_count; scan_cur++) {
            bitmap_find_region(map,
                               start_x + (deltas[i].x * scan_cur),
                               start_y + (deltas[i].y * scan_cur),
                               region);
        }
    }

    return;
}

bitmap_region_list*
bitmap_find_all_regions(bitmap* map)
{
    bitmap *map_copy;
    
    bitmap_region      *region_head = NULL;
    bitmap_region_list *region_list_head, *region_list_entry;

    size_t cur_x, cur_y;

    if(map == NULL)
        return NULL;

    // Since the process of finding regions is destructive (bits found to be
    // part of certain regions are erased), we make a temporary copy to work
    // with.
    map_copy = bitmap_copy(map);
    if(map_copy == NULL)
        return NULL;

    region_list_head = bitmap_region_list_create();
    if(region_list_head == NULL)
        goto error_cleanup;
        
    for(cur_y = 0; cur_y < map->height; cur_y++)
    {
        for(cur_x = 0; cur_x < map->width; cur_x++)
        {
            // We will have many instances of points that will contain no
            // regions, due to the way we erase bits that were part of other
            // regions already. Thus we'll not reallocate a new region head
            // if one was not used on the previous iteration

            if(region_head == NULL) {
                region_head = bitmap_region_create();
            
                if(region_head == NULL)
                   goto error_cleanup;
            }
            
            bitmap_find_region(map_copy, cur_x, cur_y, region_head);
            if(linked_list_empty(&(region_head->list)))
                continue;

            region_list_entry =
            (bitmap_region_list*)malloc(sizeof(*region_list_entry));

            if(region_list_entry == NULL)
                goto error_cleanup;

            region_list_entry->region = region_head;
            linked_list_push(&(region_list_entry->list),
                             &(region_list_head->list));

            region_head = NULL;
        }
    }

    goto end;

error_cleanup:
    if(region_head != NULL) {
        bitmap_region_free(region_head);
    }

    if(region_list_entry != NULL)
        free(region_list_entry);

    if(region_list_head != NULL) {
        bitmap_region_list_free(region_list_head);
    }

end:
    if(map_copy != NULL)
        bitmap_free(map_copy);

    return NULL;
}

bitmap_region*
bitmap_region_create()
{
    bitmap_region* head = (bitmap_region*)malloc(sizeof(*head));
    if(head != NULL)
        linked_list_init(&(head->list));

    return head;
}

void
bitmap_region_free(bitmap_region* region)
{
    if(region != NULL) {
        linked_list_free_items(&(region->list), bitmap_region, free);
        free(region);
    }
}

bitmap_region_list*
bitmap_region_list_create()
{
    bitmap_region_list* head = (bitmap_region_list*)malloc(sizeof(*head));
    if(head != NULL)
        linked_list_init(&(head->list));

    return head;
}

void
bitmap_region_list_free(bitmap_region_list* region_list)
{
    bitmap_region_list *iter, *next;
    linked_list_foreach_safe(iter, next, &(region_list->list),
                             bitmap_region_list)
    {
        bitmap_region_free(iter->region);
    }
    
    linked_list_free_items(&(region_list->list), bitmap_region_list, free);
}

bitmap*
bitmap_from_region(bitmap_region* region,
                   size_t orig_width,
                   size_t orig_height)
{
    bitmap* new_map;
    bitmap_region* cur_region_point;

    if(region == NULL || orig_width == 0 || orig_height == 0)
        return NULL;
    
    new_map = bitmap_create(orig_width, orig_height);
    if(new_map == NULL)
        return NULL;

    linked_list_foreach(cur_region_point, &(region->list), bitmap_region) {
        bitmap_setbit(new_map, cur_region_point->x, cur_region_point->y, 1);
    }

    return new_map;
}

bitmap*
bitmap_from_region_list(bitmap_region_list* region_list,
                        size_t orig_width,
                        size_t orig_height)
{
    bitmap* new_map;

    bitmap_region_list* cur_region;
    image_bit bit_value;

    if(region_list == NULL || orig_width == 0 || orig_height == 0)
        return NULL;
    
    new_map = bitmap_create(orig_width, orig_height);
    if(new_map == NULL)
        return NULL;

    bit_value = 1;

    linked_list_foreach(cur_region, &(region_list->list), bitmap_region_list) {
        bitmap_region* cur_region_point;

        linked_list_foreach(cur_region_point, &(cur_region->list), bitmap_region) {
            bitmap_setbit(new_map,
                          cur_region_point->x, cur_region_point->y,
                          bit_value);
        }

        bit_value++;
    }

    return new_map;
}
