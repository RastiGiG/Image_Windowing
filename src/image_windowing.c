/**************************************************************************
 * IMAGE WINDOWING V1.0
 * ------------------------------------------------------------------------
 * Copyright (c) 2023-2024 RastiGiG <randomly.ventilates@simplelogin.co>
 * 
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would
 *    be appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not
 *    be misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source
 *    distribution.
 *
 ***********************************************************************/

/* Libraries
 *
 * */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


/* Macros
 *
 * */


/* Type declarations
 *
 * usize            -- Rust inspired name for size_t type
 * u8               -- Rust inspired shorthand for uint8_t
 * u16              -- Rust inspired shorthand for uint16_t
 * u32              -- Rust inspired shorthand for uint16_t
 * image            -- Sturct representing an image
 * */
typedef size_t usize;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

typedef struct {
    u8* buffer;
    u32 width;
    u32 height;
    usize length;
} image;


/* Windowing Function
 *
 * */
//void image_windowing(image image, u32 x_offset, u32 y_offset, u32 max_height, u32 max_width){
void image_windowing(void *buffer, u32 width, u32 height, u32 x_offset, u32 y_offset, u32 max_height, u32 max_width, u8 bytes_per_pixel){
    if(width > max_width) {
        fprintf(stderr, "[ERROR]: Given width exceeds image width of %d", max_width);
        exit(1);
    }
    if(height > max_height) {
        fprintf(stderr, "[ERROR]: Given height exceeds image heigth of %d", max_height);
        exit(1);
    }

    u8* input_buffer = buffer;
    u8* output_buffer;

    usize k = 0;
    for (int i = y_offset * max_width; i < height; i++) {
        for (int j = x_offset; j < width; j++) {
            output_buffer[k] = buffer[j + i*max_width];
            k++;
        }
    }
 

}

/* Handle file opening and writing
 * 
 * */
usize file_open_and_write (char *filepath, void * buffer, usize size){
    // Check file exists
    if (access(filepath, F_OK) == 0){
        fprintf(stderr, "[ERROR]: file '%s' already exists! Delete/move it or choose a different filename.", filepath);
        exit(1);
    }

    // Open file and check writability, 'wb' -> 'write binary'
    FILE *file = fopen(filepath, BINARY_WRITE);
    if(file == NULL){
        fprintf(stderr, "[ERROR]: File '%s' cannot be opened! Do you have write permissions?", filepath);
        exit(1);
    }

    // cast input to 8 bit integer to process data 1 byte at a time
    u8 *data = buffer;
    usize i;

    // Write file to memory
    for (i = 0; i < size; i++) {
        fwrite(&data[i], 1, 1, file);   //sizeof(u8),
    }

    // Check size of written file
    usize file_size;
    fseek(file, 0L, SEEK_END);
    file_size = ftell(file);

    fclose(file);
    return file_size;
}


/* Handle file opening and reading
 * 
 * */
usize file_open_and_read (char *filepath, char* buffer, usize size){
    // Check file exists
    if (access(filepath, F_OK) != 0){
        fprintf(stderr, "[ERROR]: File '%s' cannot be accessed! Does it exist?", filepath);
        exit(1);
    }

    // Open file and check readability
    FILE *file = fopen(filepath, "r");
    if(file == NULL){
        fprintf(stderr, "[ERROR]: File '%s' cannot be opened! Do you have read permissions?", filepath);
        exit(1);
    }

    // Check file size
    usize file_size;
    fseek(file, 0L, SEEK_END);
    file_size = ftell(file);
    fseek(file, 0L, SEEK_SET);

    if(file_size <= 0){
        fprintf(stderr, "[ERROR]: File has '%zu' <= 0 bytes!\n", file_size);
        exit(1);
    }
    if(file_size > size){
        fprintf(stderr, "[ERROR]: File size (%zu) is too big. Max size allowed is %zu!\n", file_size, size);
        exit(1);
    }

    // Read file to memory
    fread(buffer, sizeof(char), file_size, file);
    fclose(file);

    return file_size;
}

/* Main Function
 *
 * */
int main(int argc, char *argv[])
{
    return EXIT_SUCCESS;
}

