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
    bitmap_print_char_table* print_table;

    bitmap *cur_bitmap, *in_proc_bitmap;
    
    bitmap_region cur_bitmap_region_point;
    bitmap_region_list cur_bitmap_region;

    print_table = bitmap_print_char_table_create('a', 26);

    linked_list_init(&(cur_bitmap_region_point.list));
    linked_list_init(&(cur_bitmap_region.list));

    for(;;) {
        size_t cur_x, cur_y;

        cur_bitmap = bitmap_read(stdin);
        if(cur_bitmap == NULL)
            break;

        in_proc_bitmap = bitmap_copy(cur_bitmap);
        if(in_proc_bitmap == NULL)
            break;

        printf("parsed bitmap:\n");
        bitmap_print(cur_bitmap, stdout, print_table, 26+1, "U", 1);

        for(cur_x = 0, cur_y = 0;
            cur_x < cur_bitmap->width && cur_y < cur_bitmap->height;
            cur_x++, cur_y++)
        {
            bitmap_find_region(cur_bitmap, 0, 0, &bitmap_region_head);
        if(!linked_list_empty(&(bitmap_region_head.list))) {
            bitmap* region_bitmap = bitmap_from_region(&bitmap_region_head,
                                                       cur_bitmap->width,
                                                       cur_bitmap->height);

            bitmap_region* region_entry;
            linked_list_foreach(region_entry, &(bitmap_region_head.list), bitmap_region) {
                printf("entry: x=%lu, y=%lu\n", region_entry->x, region_entry->y);
            }
            
            printf("region bitmap:\n");
            bitmap_print(region_bitmap, stdout, "_", "a");
            
            bitmap_free(region_bitmap);
            linked_list_free_items(&(bitmap_region_head.list), bitmap_region,
                                   free);

            printf("processed bitmap:\n");
            bitmap_print(cur_bitmap, stdout, "0", "1");
        }
    }

    getchar();
    return 0;
}