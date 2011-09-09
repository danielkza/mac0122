/*! @file bitmap.h
 *
 * Definition and functions for a monocromatic image bitmap.
 *
 * @author Daniel Miranda
 *
 * @copyright Copyright (c) 2011 Daniel Miranda. All Rights Reserved.
 *            Licensed under the GNU General Public License, version 2.
 */

#ifndef BITMAP_H
#define BITMAP_H

#include "linked_list.h"

/// Representation of a single bit of a bitmap
typedef unsigned char image_bit;

/// Struct for representing a monochrome, rectangular image bitmap
typedef struct  {
    /// Width of the bitmap in pixels
    size_t width;
    /// Height of the bitmap in pixels
    size_t height;

    /// Pointer to an array image_bit's containing all the data
    image_bit* bits;
} bitmap;

/// Struct for representing an arbitrary connected region of points in a bitmap
typedef struct {
    /// Linked list entry for keep track of the following points
    linked_list list;

    /// Position of this point on the x-axis
    size_t x;
    /// Position of this point on the y-axis
    size_t y;
} bitmap_region;


bitmap* bitmap_create(size_t width,
                      size_t height);

void bitmap_setbit(bitmap* map,
                   size_t x,
                   size_t y,
                   image_bit value);

image_bit bitmap_getbit(bitmap* map,
                        size_t x,
                        size_t y);

bitmap* bitmap_copy_region(bitmap* map,
                           size_t start_x,
                           size_t start_y,
                           size_t width,
                           size_t height);

bitmap* bitmap_copy(bitmap* map);


void bitmap_free(bitmap* map);

void bitmap_print(bitmap* map,
                  FILE* outfile,
                  char* unset_str,
                  char* set_str);

int bitmap_read_row(FILE* infile,
                    bitmap* bitmap,
                    size_t row_num,
                    char* buf,
                    int buf_size,
                    char** buf_pos);

bitmap* bitmap_read(FILE* infile);

void bitmap_find_region(bitmap* map,
                        size_t start_x,
                        size_t start_y,
                        bitmap_region* region);

bitmap* bitmap_from_region(bitmap_region* region,
                           size_t orig_width,
                           size_t orig_height);

#endif // BITMAP_H

