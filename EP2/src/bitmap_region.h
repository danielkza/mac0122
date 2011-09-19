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

/**
 * A type representing which regions to use for searching for regions on a
 * region. The vector must be terminated by an element {0, 0}.
 */
typedef struct {
    /// Delta in the x-axis
    int x;
    /// Delta in the y-axis
    int y;
} bitmap_region_search;

const bitmap_region_search BITMAP_SEARCH_CROSS[] = {
    { 1,  0},
    {-1,  0},
    { 0,  1},
    { 0, -1},
    { 0,  0}
};

const bitmap_region_search BITMAP_SEARCH_RIGHT_DOWN[] = {
    { 1,  0},
    { 0, -1},
    { 0,  0}
};

typedef struct {
    /// Linked list entry for keep track of the following regions
    linked_list list;

    /// One connected region of a bitmap
    bitmap_region *region;
} bitmap_region_list;

bitmap_region*
bitmap_region_create(void);

void
bitmap_region_free(bitmap_region* region);

bitmap_region_list*
bitmap_region_list_create(void);

void
bitmap_region_list_free(bitmap_region_list* region_list);

void
bitmap_find_region(bitmap* map,
                   size_t start_x,
                   size_t start_y,
                   bitmap_region* region,
                   const bitmap_region_search* search);

bitmap_region_list*
bitmap_find_all_regions(bitmap* map);

bitmap*
bitmap_from_region(bitmap_region* region,
                   size_t orig_width,
                   size_t orig_height);

bitmap*
bitmap_from_region_list(bitmap_region_list* region_list,
                        size_t orig_width,
                        size_t orig_height);


#endif //BITMAP_REGION_H