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
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

/* Macros
 *
 * MAX_BUFF_SIZE                        -- Limits the amount of bytes for the
input file
 * BINARY_WRITE                         -- String representing binary write flag
 * BINARY_READ                          -- String representing binary read flag
 * BYTES_PER_PIXEL_GRAYSCALE            -- bytes per pixel in a grayscale image
 * BYTES_PER_PIXEL_RGB565               -- bytes per pixel in a 16bit rgb image
 * BYTES_PER_PIXEL_RGB888               -- bytes per pixel in a 24bit rgb image
 * */
#define MAX_BUFF_SIZE                   1024 * 1024 
#define BINARY_WRITE                    "wb"
#define BINARY_READ                     "rb"
#define BYTES_PER_PIXEL_GRAYSCALE       1
#define BYTES_PER_PIXEL_RGB565          2
#define BYTES_PER_PIXEL_RGB888          3


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
    u8 bytes_per_pixel;
} image;


/* Variable definitions
 *
 * img              -- Pointer to image struct
 * */
static image* img = NULL;


/* @brief: Windowing function that cuts an image to a specified size
 * 
 * @param: width            -- width of output image
 * @param: height           -- height of output image
 * @param: width_offset     -- width offset of output image
 * @param: height_offset    -- height offset of output image
 * */
void image_windowing(u32 width, u32 height, u32 width_offset, u32 height_offset){
    if(width > img->width) {
        fprintf(stderr, "[ERROR]: Given width %u exceeds image width of %u", width, img->width);
        exit(1);
    }
    if(height > img->height) {
        fprintf(stderr, "[ERROR]: Given height %u exceeds image heigth of %u", height, img->height);
        exit(1);
    }
    /* Local Variables
     *
     * bpp                  - bytes per pixel
     * width_orig           - width of input image
     * height_orig          - height of input image
     * width_offset_corr    - width offset corrected by bpp
     * width_max_idx        - max width index of selected part of image
     * height_max_idx       - max heigth index of selected part of image
     * */
    u8 bpp=img->bytes_per_pixel; 
    u32 width_orig = img->width * bpp;
    u32 height_orig = img->height;
    u32 width_offset_corr = width_offset * bpp;
    u32 width_max_idx = width_offset_corr + (width * bpp);  
    u32 height_max_idx = height_offset + height;

    u32 output_length = width * height * bpp;

    // Calculate new imagge pixels
    usize output_idx = 0;
    for (int j = 0; j < height_orig; j++) {
        for (int i = 0; i < width_orig; i++) {
            if ((i > width_offset_corr) && (i < width_max_idx) &&
            (j > height_offset) && (j < height_max_idx)) {
                img->buffer[output_idx++] = img->buffer[(j + width_orig) + i];
            }
        }
    }

    // Set remaining index to 0
    for (; output_idx < output_length; output_idx++) {
        img->buffer[output_idx] = 0;
    }

    // Store new dimensions to struct
    img->width = width;
    img->height = height;
    img->length = output_length;
    return;
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
usize file_open_and_read (char *filepath, char* buffer){
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
    if (argc <= 4 || argc > 4){
        fprintf(stderr, "Usage: %s <input file> <output file> <format: none|grayscale|rgb565|rgb888>\n", argv[0]);
        exit(0);
    }

    /* Input variables
     *
     * read_filepath        -- path to input file
     * write_filepath       -- path to output file
     * output_file_size     -- determines size of output file
     * b_is_grayscale       -- set grayscale output to true/false
     * */
    char *read_filepath     = argv[1];
    char *write_filepath    = argv[2];
    char* format            = argv[3];
    usize output_file_size  = 0;
    
    /* Variables for scanf
    *
    * width                -- stores image width (needed for netpbm formats)
    * height               -- stores image height (needed for netpbm formats)
    * */
    u32 width                   = 0;
    u32 height                  = 0;

  
    void* input_buffer = calloc(MAX_BUFF_SIZE, sizeof(char));
    void* output_buffer = calloc(MAX_BUFF_SIZE, sizeof(u8));

    // Read the input file
    usize file_size = file_open_and_read(read_filepath, input_buffer);

    // Get dimensions from user, since they cannot be extracted from raw data
    printf("Enter width: ");
    scanf("%20u", &width);
    printf("Enter height: ");
    scanf("%20u", &height);
    img->buffer = (u8*) input_buffer;

    if ((strcmp(format, "grayscale") == 0)) {
      img->bytes_per_pixel = BYTES_PER_PIXEL_GRAYSCALE;
    } else if ((strcmp(format, "rgb565") == 0)) {
      img->bytes_per_pixel = BYTES_PER_PIXEL_RGB565;
    } else if ((strcmp(format, "rgb888") == 0)) {
      img->bytes_per_pixel = BYTES_PER_PIXEL_RGB888;
    } else {
      img->bytes_per_pixel = 1;
      fprintf(stdout, "No image format given. Assuming %d byte(s) per pixel", img->bytes_per_pixel);
    }

    // Write the input file to binary, half file size because of concatenation
    file_size = file_open_and_write(write_filepath, output_buffer,
                                    output_file_size);

    printf("Sucessfully wrote file '%s' of size '%zu'\n", write_filepath,
           file_size);

    // Realease pointer
    free(input_buffer);
    free(output_buffer);
    input_buffer = NULL;
    output_buffer = NULL;
    return EXIT_SUCCESS;
}
