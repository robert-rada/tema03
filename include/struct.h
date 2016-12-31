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

typedef struct
{
    short x;
    short y;
} Coord;

typedef struct
{
    Coord coord;
    Pixel color;
} EdgePixel;

#pragma pack()

#endif
