#ifndef IMAGE_H
#define IMAGE_H

#include "struct.h"

Image*      newImage(int widht, int height);
Image*      deleteImage(Image *image);
Pixel*      getPixel(Image *image, int line, int col);
Pixel       setPixel(unsigned char red, unsigned char green,
                unsigned char blue);
EdgePixel*  fill(Image *image, int lin, int col, int threshold, char **vis,
                int *size);
void        printPixel(Pixel pixel);
void        printImage(Image *image);
void        readImage(Image *image, FILE *input);
void        swapPointers(Pixel **a, Pixel **b);
void        makeGrayscaleImage(Image *image, char *image_name,
                struct bmp_fileheader *fileheader, 
                struct bmp_infoheader *infoheader);
void        makeFilteredImage(Image *image, int filter[3][3], FILE *out);
void        applyFilters(Image *image, char *image_name,
                struct bmp_fileheader *fileheader, 
                struct bmp_infoheader *infoheader);
void        compressImage(Image *image, int threshold,
                struct bmp_fileheader *fileheader,
                struct bmp_infoheader *infoheader);
void        decompressImage(char *file_name);
int         getPaddingSize(int width);

#endif
