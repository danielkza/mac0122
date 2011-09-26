/** @file bitmap.c
 *
 * Functions and structures for representing and manipulating bit matrixes
 *
 * @author Daniel Miranda
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "utils.h"
#include "bitmap.h"

image_bit
bitmap_getbit(const bitmap *map,
              int x,
              int y)
{
    if(map == NULL || map->data == NULL
       || x < 0 || x >= map->width
       || y < 0 || y >= map->height)
    {
        return 0;
    }

    return map->data[y * map->width + x];
}

void
bitmap_setbit(bitmap *map,
              int x,
              int y,
              image_bit value)
{
    if(map == NULL || map->data == NULL
       || x < 0 || x >= map->width
       || y < 0 || y >= map->height)
    {
        return;
    }

    map->data[y * map->width + x] = value;
}

/**
 * Retrieves a single connected regions from a bitmap, starting at a given
 * point.
 *
 * @param map          The bitmap to use
 * @param start_x      0-based index of the starting position on the x-axis
 * @param start_y      0-based index of the starting position on the y-axis
 * @param region_start Pointer to a pointer to a bitmap_region that will be 
 *                     updated to refer to the first item of the newly created
 *                     region
 * @param region_end Pointer to a pointer to a bitmap_region that will be 
 *                     updated to refer to the last item of the newly created
 *                     region
 *
 * @internal
 */

static int
bitmap_find_region__(bitmap *map,
                     int start_x,
                     int start_y,
                     bitmap_region** region_start,
                     bitmap_region** region_end)
{
    bitmap_region *region_entry;

    if(region_start == NULL || region_end == NULL)
        return 0;

    if(bitmap_getbit(map, start_x, start_y) == 0)
        return 0;
                    
    bitmap_setbit(map, start_x, start_y, 0);
        
    region_entry = malloc(sizeof(*region_entry));
    if(region_entry == NULL)
        return 0;

    region_entry->x = start_x;
    region_entry->y = start_y;
    region_entry->next = NULL;

    if(*region_start == NULL)
        *region_start = region_entry;
    else if(*region_end != NULL)
        (*region_end)->next = region_entry;
        
    *region_end = region_entry;

    bitmap_find_region__(map, start_x + 1, start_y, region_start, region_end);
    bitmap_find_region__(map, start_x - 1, start_y, region_start, region_end);
    bitmap_find_region__(map, start_x, start_y + 1, region_start, region_end);
    bitmap_find_region__(map, start_x, start_y - 1, region_start, region_end);

    return 1;
}

bitmap_region_list*
bitmap_find_all_regions(bitmap* map)
{
    bitmap_region_list *list_start = NULL,
                       *list_end = NULL;
    bitmap_region *region_start = NULL,
                  *region_end = NULL;
    int x, y;

    if(map == NULL || map->data == NULL)
        return NULL;

    for(y = 0; y < map->height; y++) {
        for(x = 0; x < map->width; x++) {
            region_start = region_end = NULL;

            if(bitmap_find_region__(map, x, y, &region_start, &region_end)) {
                bitmap_region_list *list_entry = malloc(sizeof(*list_entry));
                if(list_entry == NULL)
                    goto error;

                list_entry->region = region_start;
                list_entry->next = NULL;

                if(list_start == NULL)
                    list_start = list_entry;
                else if(list_end != NULL)
                    list_end->next = list_entry;

                list_end = list_entry;
            }
        }
    }
    
    return list_start;

error:
    if(region_start != NULL) {
        linked_list_free(region_start, bitmap_region, next);
    }
    
    if(list_start != NULL) {
        bitmap_region_list *iter, *iter_next;

        linked_list_foreach(list_start, iter, iter_next, next) {
            linked_list_free(iter->region, bitmap_region, next);
            free(iter);
        }
    }

    return NULL;
}

char*
bitmap_regions_print(const bitmap* map,
                     const bitmap_region_list* regions)
{
    const bitmap_region_list *region_cur, *region_next;
    int row;
    unsigned char region_num;

    char* regions_str;
    size_t regions_str_row_size, regions_str_size;

    if(regions == NULL)
        return NULL;

    /* Allocate space for each item on the matrix (with the padding space
     * between each item and a newline at each row), plus the terminator
     */
    regions_str_row_size = 2 * map->width;
    regions_str_size = map->height * regions_str_row_size + 1;
                                           
    regions_str = malloc(regions_str_size);
    if(regions_str == NULL)
        return NULL;

    memset(regions_str, ' ', regions_str_size - 1);
    regions_str[regions_str_size - 1] = '\0';

    for(row = 1; row <= map->height; row++) {
        regions_str[row * regions_str_row_size - 1] = '\n';
    }

    region_num = 0;
    linked_list_foreach(regions, region_cur, region_next, next) {
        const bitmap_region *point_cur, *point_next;
        char c = 'a' + region_num;

        linked_list_foreach(region_cur->region, point_cur, point_next, next) {
            regions_str[(point_cur->y * regions_str_row_size) +
                        (2 * point_cur->x)] = c;
        }

        region_num++;
    }

    return regions_str;
}

bitmap*
bitmap_read(FILE* infile)
{
    unsigned int width, height, x, y;
    image_bit *bitmap_data = NULL;
    bitmap* map = NULL;

    if(fscanf(infile, "%u %u", &width, &height) != 2
       || width == 0 || height == 0)
    {
        return NULL;
    }

    map = calloc(1, sizeof(bitmap));
    if(map == NULL)
        return NULL;

    bitmap_data = (image_bit*)calloc(width * height, sizeof(*bitmap_data));
    if(bitmap_data == NULL)
        goto error;

    x = 0; y = 0;
    for(;;) {
        int c = getchar();
        if(c == EOF) {
            fprintf(stderr, "ERROR: Missing bits in row %d, col %d.\n", y, x);
            goto error;
        }

        if(isspace(c))
            continue;
            
        switch(c) {
        case '0':
            break;
        case '1':
            bitmap_data[y * width + x] = 1;
            break;
        default:
            fprintf(stderr, "ERROR: Invalid bitmap char '%c'.\n", c);
            goto error;
        }

        if(++x >= width) {
            x = 0;
            if(++y >= height)
                break;
        }
    }

    map->width = width;
    map->height = height;
    map->data = bitmap_data;

    return map;

error:
    if(map != NULL)
        free(map);

    if(bitmap_data != NULL)
        free(bitmap_data);

    return NULL;
}