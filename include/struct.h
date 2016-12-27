#ifndef STRUCT_H
#define STRUCT_H

#include "headers.h"

#pragma pack(1)

typedef struct
{
    unsigned char red;
    unsigned char green;
    unsigned char blue;
} Pixel;

typedef struct 
{
    int width;
    int height;
    Pixel **pixels;
} Image;

#pragma pack()

#endif
