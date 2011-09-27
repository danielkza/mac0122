/** @file matrix_regions.c
 *
 * Main program file
 *
 * @author Daniel Miranda (No. USP: 7577406) <danielkza2@gmail.com>
 *         Exercício-Programa 2 - MAC0122 - IME-USP - 2011
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "utils.h"
#include "bitmap.h"

#define DEBUG_PRINT_READ_BITMAP 0

int main(void) {
    for(;;) {
        bitmap* map;
        bitmap_region_list *regions;
        
        map = bitmap_read(stdin);
        if(map == NULL)
            break;

        #if DEBUG_PRINT_READ_BITMAP
        {
            int x, y;

            for(y = 0; y < map->height; y++) {
                for(x = 0; x < map->width; x++) {
                    printf("%u", map->data[(y * map->width) + x]);
                }
                printf("\n");
            }
        }
        #endif

        regions = bitmap_find_all_regions(map);
        if(regions != NULL) {
            unsigned int region_count, region_num;
            bitmap_region_list *region_cur, *region_next;
                
            region_count = 0;
            linked_list_foreach(regions, region_cur, region_next, next) {
                region_count++;
            }

            if(region_count < 26) {
                char *str = bitmap_regions_print(map, regions);
                if(str != NULL) {
                    puts(str);
                    free(str);
                }
            }

            printf("%u regiões encontradas:\n", region_count);

            region_num = 1;
            linked_list_foreach(regions, region_cur, region_next, next) {
                unsigned int point_count;
                bitmap_region *point_cur, *point_next;
                    
                point_count = 0;
                linked_list_foreach(region_cur->region, point_cur, point_next, next) {
                    point_count++;
                }

                printf("  %u: %u pontos\n", region_num, point_count);
                region_num++;
            }
        } else {
            printf("Nenhuma região encontrada.\n");
        }
    }

    return 0;
}
