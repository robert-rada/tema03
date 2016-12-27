#include "../include/image.h"

Image *newImage(int width, int height)
{
    Image *new_image = malloc(sizeof(Image));
    if (new_image == NULL)
        fprintf(stderr, "MEMORY ALLOCATION FAILED\n");

    new_image->pixels = malloc(height * sizeof(Pixel*));
    if (new_image->pixels == NULL)
        fprintf(stderr, "MEMORY ALLOCATION FAILED\n");

    for (int i = 0; i < height; i++)
    {
        new_image->pixels[i] = malloc(width * sizeof(Pixel));
        if (new_image->pixels[i] == NULL)
            fprintf(stderr, "MEMORY ALLOCATION FAILED\n");
    }

    new_image->width = width;
    new_image->height = height;

    return new_image;
}

Image *deleteImage(Image *image)
{
    if (image == NULL)
        return NULL;

    for (int i = 0; i < image->height; i++)
        free(image->pixels[i]);
    free(image->pixels);
    free(image);

    return NULL;
}

Pixel setPixel(unsigned char red, unsigned char green, unsigned char blue)
{
    Pixel tmp;

    tmp.red = red;
    tmp.green = green;
    tmp.blue = blue;

    return tmp;
}

void swapPointers(Pixel **a, Pixel **b)
{
    void *tmp = *a;
    *a = *b;
    *b = tmp;
}

int getPaddingSize(int width)
{
    return (((3 * width) - 1) / 4 + 1) * 4;
}

void readImage(Image *image, FILE *input)
{
    int line_size = getPaddingSize(image->width);

    for (int line = image->height - 1; line >= 0; line--)
    {
        image->pixels[line] = malloc(line_size);
        if (image->pixels[line] == NULL)
            fprintf(stderr, "MEMORY ALLOCATION FAILED\n");
        fread(image->pixels[line], line_size, 1, input);
    }
}

void printHeader(struct bmp_fileheader *fileheader, 
        struct bmp_infoheader *infoheader, FILE *out)
{
    fwrite(fileheader, sizeof(struct bmp_fileheader), 1, out);
    fwrite(infoheader, sizeof(struct bmp_infoheader), 1, out);

    int empty_bytes = fileheader->imageDataOffset
        - sizeof(struct bmp_fileheader) - sizeof(struct bmp_infoheader);
    while (empty_bytes--)
        fputc(0, out);
}


void makeGrayscaleImage(Image *image, char *image_name,
        struct bmp_fileheader *fileheader, struct bmp_infoheader *infoheader)
{
    char file_name[256];
    strncpy(file_name, image_name, strlen(image_name) - 4);
    file_name[strlen(image_name) - 4] = 0;
    strcat(file_name, "_black_white.bmp");

    FILE *out = fopen(file_name, "wb");

    printHeader(fileheader, infoheader, out);

    for (int lin = image->height - 1; lin >= 0; lin--)
        {
            for (int col = 0; col < image->width; col++)
            {
                unsigned short value = image->pixels[lin][col].red;
                value += image->pixels[lin][col].green;
                value += image->pixels[lin][col].blue;
                value /= 3;
                fwrite(&value, 1, 1, out);
                fwrite(&value, 1, 1, out);
                fwrite(&value, 1, 1, out);
                image->pixels[lin][col] = setPixel(value, value, value);
            }

        int empty_bytes = getPaddingSize(image->width) - image->width * 3;
        while (empty_bytes--)
            fputc(0, out);
    }

    fclose(out);
}

unsigned char getFilteredValue(Image *image, int lin, int col, int offset,
        int filter[3][3])
{
    int value = 0;
    for (int i = 0; i <= 2; i++)
        for (int j = 0; j <= 2; j++)
        {
            int x = lin + i - 1;
            int y = col + j - 1;
            if (x < 0 || x >= image->height)
                continue;
            if (y < 0 || y >= image->width)
                continue;
            unsigned char *p = &(image->pixels[x][y]);
            p += offset;
            value += *p * filter[i][j];
        }
    
    if (value < 0)
        return (unsigned char)0;
    if (value > 255)
        return (unsigned char)255;
    return (unsigned char)value;
}

void makeFilteredImage(Image *image, int filter[3][3], FILE *out)
{
    for (int lin = image->height - 1; lin >= 0; lin--)
    {
        for (int col = 0; col < image->width; col++)
        {
            unsigned char r = getFilteredValue(image, lin, col, 0, filter);
            unsigned char g = getFilteredValue(image, lin, col, 1, filter);
            unsigned char b = getFilteredValue(image, lin, col, 2, filter);
            fwrite(&b, 1, 1, out);
            fwrite(&g, 1, 1, out);
            fwrite(&r, 1, 1, out);
        }
    }
}

void applyFilters(Image *image, char *image_name,
        struct bmp_fileheader *fileheader, struct bmp_infoheader *infoheader)
{
    int filter[3][3][3] = { { {-1, -1, -1}  ,
                              {-1,  8, -1}  ,
                              {-1, -1, -1} },

                            { { 0,  1,  0}  ,
                              { 1, -4,  1}  ,
                              { 0,  1,  0} },

                            { { 1,  0, -1}  ,
                              { 0,  0,  0}  ,
                              {-1,  0,  1}}};
    
    for (int i = 0; i < 3; i++)
    {
        char file_name[256];
        strncpy(file_name, image_name, strlen(image_name) - 4);
        file_name[strlen(image_name) - 4] = 0;

        char index[2];
        index[0] = '1' + (char)i;
        index[1] = 0;
        strcat(file_name, "_f");
        strcat(file_name, index);
        strcat(file_name, ".bmp");

        FILE *out = fopen(file_name, "wb");

        printHeader(fileheader, infoheader, out);
        makeFilteredImage(image, filter[i], out);

        fclose(out);
    }
}

void printPixel(Pixel pixel)
{
    printf("%d %d %d ", pixel.red, pixel.green, pixel.blue);
}

void printImage(Image *image)
{
    if (image == NULL)
        fprintf(stderr, "IMAGE IS EMPTY\n");

    printf("%d %d\n", image->width, image->height);
}

