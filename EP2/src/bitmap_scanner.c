#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <ctype.h>

#include "linked_list.h"

#include "bitmap.h"
#include "bitmap_region.h"

int main(int argc, char** argv)
{
    bitmap_print_table* print_table;

    bitmap *cur_bitmap, *in_proc_bitmap;
    bitmap_region_list* region_list;

    print_table = bitmap_print_table_create_alpha();
    if(print_table == NULL)
        return 1;
    
    for(;;) {
        cur_bitmap = bitmap_read(stdin);
        if(cur_bitmap == NULL)
            break;

        in_proc_bitmap = bitmap_copy(cur_bitmap);
        if(in_proc_bitmap == NULL)
            break;

        region_list = bitmap_find_all_regions(in_proc_bitmap);
        if(region_list != NULL ) {
            bitmap* regions_bitmap = bitmap_from_region_list(region_list, 
                                                             cur_bitmap->width,
                                                             cur_bitmap->height);
            if(regions_bitmap != NULL) {
                bitmap_print(regions_bitmap, stdout, print_table,  "");
            }
        }
    }

    getchar();
    return 0;
}