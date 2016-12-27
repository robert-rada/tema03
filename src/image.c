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

void makeGrayscaleImage(Image *image, char *image_name,
        struct bmp_fileheader *fileheader, struct bmp_infoheader *infoheader)
{
    char file_name[256];
    strncpy(file_name, image_name, strlen(image_name) - 4);
    file_name[strlen(image_name) - 4] = 0;
    strcat(file_name, "_black_white.bmp");

    FILE *out = fopen(file_name, "wb");

    fwrite(fileheader, sizeof(struct bmp_fileheader), 1, out);
    fwrite(infoheader, sizeof(struct bmp_infoheader), 1, out);

    int empty_bytes = fileheader->imageDataOffset
        - sizeof(struct bmp_fileheader) - sizeof(struct bmp_infoheader);
    while (empty_bytes--)
        fputc(0, out);

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
        }

        empty_bytes = getPaddingSize(image->width) - image->width * 3;
        while (empty_bytes--)
            fputc(0, out);
    }

    fclose(out);
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

