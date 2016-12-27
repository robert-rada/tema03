#ifndef IMAGE_H
#define IMAGE_H

#include "struct.h"

Image *newImage(int widht, int height);
Image *deleteImage(Image *image);
Pixel *getPixel(Image *image, int line, int col);
Pixel setPixel(unsigned char red, unsigned char green,
        unsigned char blue);
void  printPixel(Pixel pixel);
void  printImage(Image *image);
void  readImage(Image *image, FILE *input);
void  swapPointers(Pixel **a, Pixel **b);
void  makeGrayscaleImage(Image *image, char *image_name,
        struct bmp_fileheader *fileheader, struct bmp_infoheader *infoheader);
int  getPaddingSize(int width);

#endif
