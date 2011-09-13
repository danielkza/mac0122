#ifndef BITMAP_REGION_H
#define BITMAP_REGION_H

/// Struct for representing an arbitrary connected region of points in a bitmap
typedef struct {
    /// Linked list entry for keep track of the following points
    linked_list list;

    /// Position of this point on the x-axis
    size_t x;
    /// Position of this point on the y-axis
    size_t y;
} bitmap_region;

typedef struct {
    /// Linked list entry for keep track of the following regions
    linked_list list;

    /// One connected region of a bitmap
    bitmap_region *region;
} bitmap_region_list;


void bitmap_find_region(bitmap* map,
                        size_t start_x,
                        size_t start_y,
                        bitmap_region* region);

bitmap* bitmap_from_region(bitmap_region* region,
                           size_t orig_width,
                           size_t orig_height);


#endif //BITMAP_REGION_H