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

/// Table of strings that map to bit values for pretty-printing bitmaps
typedef char* bitmap_print_char_table[];


bitmap*
bitmap_create(size_t width,
              size_t height);

void
bitmap_setbit(bitmap* map,
              size_t x,
              size_t y,
              image_bit value);

image_bit
bitmap_getbit(bitmap* map,
              size_t x,
              size_t y);

bitmap*
bitmap_copy_region(bitmap* map,
                   size_t start_x,
                   size_t start_y,
                   size_t width,
                   size_t height);

bitmap*
bitmap_copy(bitmap* map);


void
bitmap_free(bitmap* map);

void
bitmap_print(bitmap* map,
             FILE* outfile,
             bitmap_print_char_table* str_table,
             size_t str_table_len,
             char* default_str,
             int padding_len);

bitmap_print_char_table*
bitmap_print_char_table_create(unsigned char start,
                               size_t size);

void
bitmap_print_char_table_free(bitmap_print_char_table *table,
                             size_t size);

bitmap* bitmap_read(FILE* infile);


#endif // BITMAP_H

