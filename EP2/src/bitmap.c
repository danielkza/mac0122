#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "bitmap.h"

/**
 * Creates a monochrome bitmap with the given width and height. 
 *
 * @param width  Width of the bitmap
 * @param height Height of the bitmap
 *
 * @return Pointer to a bitmap struct, or a null pointer on failure
 */

bitmap*
bitmap_create(size_t width,
              size_t height)
{
    bitmap* map = (bitmap*)malloc(sizeof *map);
    if(map != NULL) {    
        image_bit *bits = (image_bit*)calloc(sizeof(image_bit),
                                             width * height);
        if(bits == NULL) {
            free(map);
            return NULL;
        }

        map->width = width;
        map->height = height;
        map->bits = bits;
    }

    return map;
}

/**
 * Sets a bit on an existing bitmap
 *
 * @param map   Pointer to a bitmap struct
 * @param x     0-based index of the bit on the x-axis
 * @param y     0-based index of the bit on the y-axis
 * @param value Value to set the bit to. 0 is the 'blank' value: any other
 *              value is taken as filled, but wrote as-is
 */

void
bitmap_setbit(bitmap* map,
              size_t x,
              size_t y,
              image_bit value)
{
    if(map == NULL)
        return;
    
    if(map->width <= x || map->height <= y || map->bits == NULL)
        return;

    map->bits[(y * map->width) + x] = ((value == 0) ? 0 : 1);
}

/**
 * Retrieves the current value of a bit on an existing bitmap
 *
 * @param map   Pointer to a bitmap struct
 * @param x     0-based index of the bit on the x-axis
 * @param y     0-based index of the bit on the y-axis
 * 
 * @return Value of the bit: 0 for an unset bit or an invalid position, or 1
 *         for a set bit
 */

image_bit
bitmap_getbit(bitmap* map,
              size_t x,
              size_t y)
{
    if(map == NULL)
        return 0;
    
    if(map->width <= x || map->height <= y || map->bits == NULL)
        return 0;

    return map->bits[(y * map->width) + x];
}

/**
 * Creates a copy of a rectangular region ion of an existing bitmap.

 * @param map     Pointer to a bitmap struct
 * @param start_x 0-based index of the x-axis of the first pixel to be copied
 * @param start_y 0-based index of the y-axis of the first pixel to be copied
 * @param width   Horizontal size of the region to be copied in pixels
 * @param height  Vertical size of the region to be copied in pixels
 * 
 * @return Pointer to a new bitmap. Destroy it with bitmap_free when done using
 */
bitmap*
bitmap_copy_rect(bitmap* map,
                 size_t start_x,
                 size_t start_y,
                 size_t width,
                 size_t height)
{
    bitmap* new_map;
    image_bit *new_bits,
              *new_bits_pos, *old_bits_pos;
    size_t row;

    // Sanity checking
    if(map == NULL || map->width == 0 || map->height == 0 || map->bits == NULL)
        return NULL;

    if(width == 0 || height == 0
        || start_x + width > map->width || start_y + height > map->height)
    {
        return NULL;
    }

    new_bits = (image_bit*)malloc(sizeof(image_bit) * width * height);
    if(new_bits == NULL)
        return NULL;

    new_map = (bitmap*)malloc(sizeof(*new_map));
    if(new_map == NULL) {
        free(new_bits);
        return NULL;
    }
    
    // Start with pointers to the old and new bits, and increment them by
    // the proper amount: the width of the region for the new bits, and the
    // original width of the bitmap for the old bits

    new_bits_pos = new_bits + start_x;
    old_bits_pos = map->bits + (start_y * map->width) + start_x;

    for(row = 0; row < height; row++) {
        memcpy(new_bits_pos, old_bits_pos, width);

        new_bits_pos += width;
        old_bits_pos += map->width;
    }

    new_map->width = width;
    new_map->height = height;
    new_map->bits = new_bits;

    return new_map;
}

/**
 * Creates a copy of an existing bitmap and all of its data.
 *
 * @param map Pointer to a bitmap struct
 *
 * @return Pointer to a new bitmap. Destroy it with bitmap_free when done using
 */

bitmap*
bitmap_copy(bitmap* map)
{
    bitmap* new_map;
    image_bit *bits;
    size_t bits_size;

    if(map == NULL)
        return NULL;

    if(map->width == 0 || map->height == 0 || map->bits == NULL)
        return NULL;

    bits_size = sizeof(image_bit) * map->width * map->height;
    if(bits_size == 0)
        return NULL;

    bits = (image_bit*)malloc(bits_size);
    if(bits == NULL)
        return NULL;

    new_map = (bitmap*)malloc(sizeof *new_map);
    if(new_map == NULL) {
        free(bits);
        return NULL;
    }

    memcpy(bits, map->bits, bits_size);
    
    new_map->width = map->width;
    new_map->height = map->height;
    new_map->bits = bits;

    return new_map;
}



/**
 * Frees a bitmap and its internal data.
 *
 * @param map Pointer to a bitmap struct. If its null the function has no
 *            effect. Otherwise, it will no longer be valid.
 */

void
bitmap_free(bitmap* map)
{
    if(map != NULL) {
        if(map->bits != NULL)
            free(map->bits);

        free(map);
    }
}

bitmap_print_table*
bitmap_print_table_create_alpha(void)
{
    bitmap_print_table* table;
    size_t i;
    unsigned char c;
    char* cur_str;

    table = (bitmap_print_table*)calloc(sizeof(bitmap_print_table), (26+2));
    if(table == NULL)
        return NULL;

    cur_str = (char*)malloc(1);
    if(cur_str == NULL) {
        free(table);
        return NULL;
    }

    cur_str[0] = '\0';

    table[0].bit_value = 0;
    table[0].repr = cur_str;

    // Actually fill the table starting from bit value 1
    for(i = 1, c = 'a';
        i <= 26;
        i++, c++)
    {
        cur_str = (char*)malloc(2);
        if(cur_str == NULL) {
            bitmap_print_table_free(table);
            return NULL;
        }

        cur_str[0] = c;
        cur_str[1] = '\0';
        table[i].repr = cur_str;
    }

    table[i].repr = NULL;
    return table;
}

void
bitmap_print_table_free(bitmap_print_table *table)
{
    if(table != NULL) { 
        size_t i;
        for(i = 0; table[i].repr != NULL; i++) {
           free(table[i].repr);
        }
    
        free(table);
    }
}

char*
bitmap_print_table_find(bitmap_print_table* table, 
                        image_bit value)
{
    size_t i;
    if(table == NULL)
        return NULL;

    for(i = 0; table[i].repr != NULL; i++) {
        if(table[i].bit_value == value)
            return table[i].repr;
    }

    return NULL;
}

/**
 * Prints a textual representation of a bitmap to an output stream.
 *
 * @param map           Pointer to a bitmap struct
 * @param outfile       Pointer to an output stream
 * @param print_table   Pointer to an array of bitmap_print_table entries
 *                      containing textual represenatations for bit values
 * @param default_str   String to be used if a value has no correspondence in
 *                      str_array
 * @param padding_len   How many spaces to insert as padding between each
 *                      printed bit
 */

void
bitmap_print(bitmap* map,
             FILE* outfile,
             bitmap_print_table print_table,
             char* default_str,
             int padding_len)
{
    size_t x, y;
    
    if(map == NULL || map->height == 0 || map->width == 0 || map->bits == NULL)
        return;

    if(outfile == NULL || print_table == NULL || default_str == NULL)
        return;

    if(padding_len <= 0)
        return;

    for(y = 0; y < map->height; y++) {
        for(x = 0; x < map->width; x++) {
            image_bit bit_value = bitmap_getbit(map, x, y);
            char* bit_repr = bitmap_print_table_find(print_table, bit_value);
            if(bit_repr == NULL)
               bit_repr = default_str;

            if(bit_repr != NULL)
                fprintf(outfile, "%*s", padding_len, bit_repr);
            else
                fprintf(outfile, "%*u", padding_len, (unsigned int)bit_value);
        }

        fprintf(outfile, "\n");
    }
}


/**
 * @internal
 *
 * Parses a row of a bitmap from an input file.
 *
 * @param infile   Pointer to an input stream to read from
 * @param bitmap   Pointer to a bitmap to store the data into
 * @param row_num  0-based index of the row we're reading
 * @param buf      Pointer to a character buffer to read the stream contents
 *                 into
 * @param buf_size Size of the character buffer, including the terminating null
 * @param buf_pos  Pointer to pointer the position in the character buffer we
 *                 should start reading from, if it was already filled with
 *                 data from a previous call, or null otherwise.
 *                 It will be updated if all the data is not consumed when we
 *                 finish reading the row, or set to null otherwise
 *
 * @return 1 on success, 0 on failure
 */                

static int
bitmap_read_row__(FILE* infile,
                  bitmap* bitmap,
                  size_t row_num,
                  char* buf,
                  int buf_size,
                  char** buf_pos)
{
    size_t col_num;
    int buffer_exhausted;

    if(infile == NULL || bitmap == NULL
       || buf == NULL || buf_size <= 0 || buf_pos == NULL)
    {
        return 0;
    }

    col_num = 0;
    buffer_exhausted = (*buf_pos == NULL) ? 1 : 0;

    do {
        char c;

        if(buffer_exhausted) {
            if(feof(stdin) || ferror(stdin)
               || fgets(buf, buf_size, infile) == NULL)
            {
                fprintf(stderr, "ERROR: Failed to read row #%lu.\n",
                        (unsigned long)row_num);

                *buf_pos = NULL;
                return 0;
            }

            *buf_pos = buf;
            buffer_exhausted = 0;
        }

        // Ignore all whitespace
        while(isspace(**buf_pos))
            (*buf_pos)++;

        if(**buf_pos == '\0') {
            buffer_exhausted = 1;
            continue;
        }

        c = **buf_pos;
        (*buf_pos)++;

        switch(c) {
        case '0':
            bitmap_setbit(bitmap, col_num, row_num, 0);
            break;
        case '1':
            bitmap_setbit(bitmap, col_num, row_num, 1);
            break;
        default:
            fprintf(stderr, "ERROR: Invalid bit value at row #%lu, "
                            "col #%lu.\n",
                    (unsigned long)row_num,
                    (unsigned long)col_num);

            return 0;
        }

        col_num++;
    } while(col_num < bitmap->width);

    return 1;
}

/**
 * Parses a bitmap from an input stream. It's length and width will be read as
 * the first two tokens, and followed by a number of 0s and 1s corresponding
 * respectively to unset and set bits.
 *
 * @remark A length or width of 0 stops the parsing.
 *
 * @param infile Pointer to an input stream to read from
 *
 * @returns The fully constructed and read bitmap, or a null pointer on failure
 */

bitmap*
bitmap_read(FILE* infile)
{
    size_t bitmap_width = 0, bitmap_height = 0,
           row_num = 0;

    bitmap* bitmap;

    char buf[512], *buf_pos;
    const size_t buf_size = sizeof(buf);
        
    if(fscanf(infile, "%lu %lu\n", &bitmap_width, &bitmap_height) != 2)
        return NULL;

    if(bitmap_width == 0 || bitmap_height == 0)
        return NULL;

    bitmap = bitmap_create(bitmap_width, bitmap_height);
    if(bitmap == NULL) {
        fprintf(stderr, "ERROR: Failed to create %lu x %lu bitmap.\n",
                (unsigned long)bitmap_width,
                (unsigned long)bitmap_height);

        return NULL;
    }

    // The buffer is not valid now, signal bitmap_read_row__ to fill it first
    buf_pos = NULL;

    for(row_num = 0; row_num < bitmap_height; row_num++) {
        if(!bitmap_read_row__(infile, bitmap, row_num, buf, buf_size,
                              &buf_pos))
        {
            bitmap_free(bitmap);

            return NULL;
        }
    }

    return bitmap;
}