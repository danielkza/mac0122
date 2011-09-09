#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <ctype.h>

#include "linked_list.h"
#include "bitmap.h"

void bitmap_region_free_func(void* region_entry) {
    free(region_entry);
}

int main(int argc, char** argv)
{
    bitmap* cur_bitmap;
    bitmap_region bitmap_region_head;

    linked_list_init(&(bitmap_region_head.list));

    for(;;) {
        cur_bitmap = bitmap_read(stdin);
        if(cur_bitmap == NULL)
            break;

        printf("parsed bitmap:\n");
        bitmap_print(cur_bitmap, stdout, "0", "1");

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