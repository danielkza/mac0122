/** @file bitmap.h
 *
 * Functions and structures for representing and manipulating bit matrixes
 *
 * @author Daniel Miranda (No. USP: 7577406) <danielkza2@gmail.com>
 *         Exercício-Programa 2 - MAC0122 - IME-USP - 2011
 */

#ifndef BITMAP_H
#define BITMAP_H

#include <stddef.h>

/** Type representing a single bit in an image matrix */
typedef unsigned char image_bit;

/** Type for a matrix of bits */
typedef struct {
    /** Width of the bitmap */
    int width;
    /** Height of the bitmap */
    int height;
    /** Pointer to an array containing the width * height image_bit's */
    image_bit *data;
} bitmap;

/** Type representing a connected region of points in a bitmap */
typedef struct bitmap_region {
    /** 0-based position of the point in the x-axis */
    int x;
    /** 0-based position of the point in the y-axis */
    int y;
    /** Pointer to the next node in the list */
    struct bitmap_region *next;
} bitmap_region;

/** Type representing a list of multiple bitmap_region's */
typedef struct bitmap_region_list {
    /** Pointer to a bitmap region */
    bitmap_region *region;
    /** Pointer to the next node in the list */
    struct bitmap_region_list *next;
} bitmap_region_list;

/**
 * Retrieves the value of a single bit from a bitmap
 *
 * @param map The bitmap to use
 * @param x   0-base coordinate of the bit in the x-axis
 * @param y   0-base coordinate of the bit in the y-axis
 *
 * @returns The value of the bit, or 0 if the coordinates are out of range
 */
image_bit
bitmap_getbit(const bitmap *map,
              int x,
              int y);


/**
 * Sets the value of a single bit from a bitmap
 *
 * @param map   The bitmap to use
 * @param x     0-base coordinate of the bit in the x-axis
 * @param y     0-base coordinate of the bit in the y-axis
 * @param value The new value to attribute to the bit
 */

void
bitmap_setbit(bitmap *map,
              int x,
              int y,
              image_bit value);

/**
 * Frees a bitmap and its associated data
 *
 * @param map A bitmap to free
 */


void
bitmap_free(bitmap *map);

/**
 * Frees a bitmap_region and its associated data
 *
 * @param region A bitmap_region to free
 */
void
bitmap_region_free(bitmap_region *region);

/**
 * Frees a bitmap_region_list and its associated data
 *
 * @param list A bitmap_region_list to free
 */
void
bitmap_region_list_free(bitmap_region_list* list);

/**
 * Retrieves a list of all the connected regions from a bitmap.
 * 
 * @warning This function is destructive: the bitmap will be completely zero-ed
 *          out after all regions are found
 *
 * @param map The bitmap to use
 *
 * @returns Pointer to the first item of the list of regions, or NULL on error
 */
bitmap_region_list*
bitmap_find_all_regions(bitmap* map);

/**
 * Generates the string representation of multiple regions on a matrix.
 * The letter representing each regions starts at 'a'.
 *
 * @param map     The bitmap to use
 * @param regions The list of regions (obtained with bitmap_find_all_regions)
 *
 * @returns Pointer to a string. free() it after you're done.
 */
char*
bitmap_regions_print(const bitmap* map,
                     const bitmap_region_list* regions);

/**
 * Reads a bitmap from a file.
 *
 * @param infile Input stream
 *
 * @returns The read bitmap or NULL on error
 */
bitmap*
bitmap_read(FILE* infile);

#endif /* BITMAP_H */

