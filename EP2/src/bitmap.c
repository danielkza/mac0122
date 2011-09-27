/** @file bitmap.c
 *
 * Functions and structures for representing and manipulating bit matrixes
 *
 * @author Daniel Miranda (No. USP: 7577406) <danielkza2@gmail.com>
 *         Exercício-Programa 2 - MAC0122 - IME-USP - 2011
 *
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

    return map->data[(y * map->width) + x];
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

    map->data[(y * map->width) + x] = value;
}

/**
 * @internal
 *
 * Adds a point to the list of points representing a region on a bitmap
 *
 * @param x            0-based index of the starting position on the x-axis
 * @param y            0-based index of the starting position on the y-axis
 * @param region_start Pointer to a pointer to a bitmap_region that will be
 *                     updated to refer to the first item of the newly created
 *                     region
 * @param region_end Pointer to a pointer to a bitmap_region that will be
 *                     updated to refer to the last item of the newly created
 *                     region
 */
static void
bitmap_region_entry_add__(int x,
                          int y,
                          bitmap_region **region_start,
                          bitmap_region **region_end)
{
    bitmap_region *region_entry = malloc(sizeof(*region_entry));
    if(region_entry == NULL)
        return;

    region_entry->x = x;
    region_entry->y = y;
    region_entry->next = NULL;

    if(*region_start == NULL)
        *region_start = region_entry;
    else if(*region_end != NULL)
        (*region_end)->next = region_entry;

    *region_end = region_entry;
}

/**
 * @internal
 *
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
 */

static int
bitmap_find_region__(bitmap *map,
                     int start_x,
                     int start_y,
                     bitmap_region** region_start,
                     bitmap_region** region_end)
{
	/* Array of structures representing the directions we'll check */
    struct {
        int x, y;
    } const deltas[] = {
        { 1,  0}, /* right */ {-1,  0}, /* left */
        { 0,  1}, /* down  */ { 0, -1}  /* up   */
    };
    /* All elements are zero-initialized */
    unsigned int edges[ARRAY_SIZE(deltas)] = {0};
    unsigned int i, j, k;

    if(region_start == NULL || region_end == NULL)
        return 0;

    /* Don't bother doing anything if the very first point on the region is a
       zero. */
    if(bitmap_getbit(map, start_x, start_y) == 0)
        return 0;

    /* Clear the bit so it is not found again later: this makes our algorithm
       much simpler as we can just recursively walk around all adjacent points
       naively. */
    bitmap_setbit(map, start_x, start_y, 0);
    bitmap_region_entry_add__(start_x, start_y, region_start, region_end);

    /* Instead of doing a naive recursion to four directions for each point.
       try walking as much as possible in each direction first, until a zero
       point is reached. */

    for(i = 0; i < ARRAY_SIZE(deltas); i++) {
        int dx = deltas[i].x, dy = deltas[i].y,
            x = start_x + dx,
            y = start_y + dy;

        while(bitmap_getbit(map, x, y) != 0)
        {
            bitmap_setbit(map, x, y, 0);
            bitmap_region_entry_add__(x, y, region_start, region_end);

            x += dx;
            y += dy;

            edges[i]++;
        }
    }

    /* Now that we walked and removed as much points as possible in all
       directions, we must call bitmap_find_region__() manually for each
       direction, because the center points have been unset already. */

    for(i = 0; i < ARRAY_SIZE(deltas); i++) {
        for(j = 1; j <= edges[i]; j++) {
            for(k = 0; k < ARRAY_SIZE(deltas); k++) {
                int x = start_x + (deltas[i].x * j) + deltas[k].x,
					y = start_y + (deltas[i].y * j) + deltas[k].y;

                bitmap_find_region__(map, x, y, region_start, region_end);
            }
        }
    }

    return 1;
}

bitmap_region_list*
bitmap_find_all_regions(bitmap* map)
{
    bitmap_region_list *list_start = NULL,
                       *list_end = NULL;
    bitmap_region *region_start, *region_end;
    int x, y;

    if(map == NULL || map->data == NULL)
        return NULL;

    for(y = 0; y < map->height; y++) {
        for(x = 0; x < map->width; x++) {
            region_start = region_end = NULL;

            if(bitmap_find_region__(map, x, y, &region_start, &region_end)) {
                /* A region was found, create an entry for it and append it to
                   the list of regions. */
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
    /* An error may happen when we have acquired a region, but are unable to
       continue adding it to the region list. Make sure not to leak it in that
       case. */
    if(region_start != NULL) {
        linked_list_free(region_start, bitmap_region, next);
    }

    if(list_start != NULL) {
        bitmap_region_list *iter, *iter_next;

        /* Free both the stored regions themselves, and the list entries */
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
    char ch;

    char* regions_str;
    size_t regions_str_row_size, regions_str_size;

    if(regions == NULL)
        return NULL;

    /* Allocate space for a string representing the matrix. In a row, one
       character per column is meant for a region letter and another one for a
       paddinng whitespace (a space for all but the last column, which is
       followed by a newline). */
    regions_str_row_size = (2 * map->width);
    regions_str_size = (map->height * regions_str_row_size) + 1;

    regions_str = malloc(regions_str_size);
    if(regions_str == NULL)
        return NULL;

    /* Initialize the whole string with spaces: we'll then sparingly insert the
       newlines, and finally the region letters as we walk the lists */
    memset(regions_str, ' ', regions_str_size - 1);
    regions_str[regions_str_size - 1] = '\0';

    for(row = 1; row <= map->height; row++)
        regions_str[(row * regions_str_row_size) - 1] = '\n';

    ch = 'a';
    linked_list_foreach(regions, region_cur, region_next, next) {
        const bitmap_region *point_cur, *point_next;

        linked_list_foreach(region_cur->region, point_cur, point_next, next) {
            regions_str[(point_cur->y * regions_str_row_size) +
                        (2 * point_cur->x)] = ch;
        }

        ch++;
    }

    return regions_str;
}

bitmap*
bitmap_read(FILE* infile)
{
    unsigned int width, height, x, y;
    image_bit *bitmap_data = NULL;
    bitmap* map = NULL;

    if(fscanf(infile, "%u %u", &height, &width) != 2
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
            /* The data array starts zeroed, skip a redundant assignmen */

            /* bitmap_data[y * width + x] = 0; */
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