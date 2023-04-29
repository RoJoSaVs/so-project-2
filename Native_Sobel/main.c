
//============================================================================
// Name        : main.c
// Author      : Daniele Gadler
// Version     :
// Description : Sobel operator in native C
//============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "file_operations.h"
#include "image_operations.h"
#include "math.h"
#include "string.h"
#include <sys/time.h>

typedef unsigned char byte;

#define STRING_BUFFER_SIZE 1024

//used to track start and end time
#define get_time(time) (gettimeofday(&time, NULL))

//false --> No vertical gradient and horizontal gradient are output
//true --> Vertical gradient and horizontal gradient are output
#define INTERMEDIATE_OUTPUT false

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        printf("You did not provide any input image name and thread. Usage: output [input_image_name] . \n");
        return -2;
    }

    //###########1. STEP - LOAD THE IMAGE, ITS HEIGHT, WIDTH AND CONVERT IT TO RGB FORMAT#########
    //Specify the input image. Formats supported: png, jpg, GIF.
    // char * file_output_RGB = strcat(argv[3], "image.rgb");
    char * arrayAddress[2] = {argv[3], "image.rgb"};
    char * file_output_RGB = array_strings_to_string(arrayAddress, 2, STRING_BUFFER_SIZE);

    char * png_strings[4] = { "convert ", argv[1], " ", file_output_RGB };
    char * str_PNG_to_RGB = array_strings_to_string(png_strings, 4,	STRING_BUFFER_SIZE);

    //actually execute the conversion from PNG to RGB, as that format is required for the program
    int status_conversion = system(str_PNG_to_RGB);

    if (status_conversion != 0)
    {
        printf("ERROR! Conversion of input PNG image to RGB was not successful. Program aborting.\n");
        return -1;
    }

    //get the height and width of the input image
    int width = 0;
    int height = 0;

    get_image_size(argv[1], &width, &height);

    //Three dimensions because the input image is in colored format(R,G,B)
    int rgb_size = width * height * 3;

    //Used as a buffer for all pixels of the image
    byte * rgb_image;

    //Load up the input image in RGB format into one single flattened array (rgb_image)
    read_file(file_output_RGB, &rgb_image, rgb_size);


    //#########2. STEP - CONVERT IMAGE TO GRAY-SCALE #################
    //convert the width and height to char *
    char str_width[100];
    sprintf(str_width, "%d", width);

    char str_height[100];
    sprintf(str_height, "%d", height);

    byte * grayImage;

    //convert the RGB vector to gray-scale
    int gray_size = rgb_to_gray(rgb_image, &grayImage, rgb_size);

    //######################3. Step - Compute vertical and horizontal gradient ##########
    byte * sobel_h_res;
    byte * sobel_v_res;

    //kernel for the horizontal axis
    int sobel_h[] = { -1, 0, 1, -2, 0, 2, -1, 0, 1 };

    itConv(grayImage, gray_size, width, sobel_h, &sobel_h_res);

    //kernel for the vertical axis
    int sobel_v[] = { 1, 2, 1, 0, 0, 0, -1, -2, -1 };

    itConv(grayImage, gray_size, width, sobel_v, &sobel_v_res);


    //#############4. Step - Compute the countour by putting together the vertical and horizontal gradients####
    byte * countour_img;
    contour(sobel_h_res, sobel_v_res, gray_size, &countour_img);

    // if(argc > 2)
    // {
    output_gradient(true, countour_img, gray_size, str_width, str_height, STRING_BUFFER_SIZE, argv[2], argv[3]);
    // }

    // else
    // {
    // 	output_gradient(true, countour_img, gray_size, str_width, str_height, STRING_BUFFER_SIZE, "sobel_countour.png");
    // }

    //let's deallocate the memory to avoid any memory leaks
    free(grayImage);
    free(sobel_h_res);
    free(sobel_v_res);
    free(countour_img);

    // Delete aux image
    // char deleteImgCmd[100] = "rm ";
    // strcat(deleteCmd, file_output_RGB);
    // printf("%s\n", deleteCmd);
    // system(deleteCmd);
    return 0;
}