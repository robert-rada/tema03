#include "../include/image.h"

void read(Image **image, int *threshold, char *image_name, char *archive_name,
        struct bmp_fileheader *fileheader, struct bmp_infoheader *infoheader)
{
    FILE *in = fopen("input.txt", "r");

    fscanf(in, "%s", image_name);

    FILE *bmp_file = fopen(image_name, "rb");
    
    fread(fileheader, sizeof(struct bmp_fileheader), 1, bmp_file);
    fread(infoheader, sizeof(struct bmp_infoheader), 1, bmp_file);
    fseek(bmp_file, fileheader->imageDataOffset, SEEK_SET);
    *image = newImage(infoheader->width, infoheader->height);
    readImage(*image, bmp_file);

    fscanf(in, "%d", threshold);

    fscanf(in, "%s", archive_name);

    fclose(in);
    fclose(bmp_file);
}

int main()
{
    Image                 *image;
    int                   threshold;
    char                  image_name[256];
    char                  archive_name[256];
    struct bmp_fileheader fileheader;
    struct bmp_infoheader infoheader;

    read(&image, &threshold, image_name, archive_name, 
            &fileheader, &infoheader);

    makeGrayscaleImage(image, image_name, &fileheader, &infoheader);
    applyFilters(image, image_name, &fileheader, &infoheader);

    image = deleteImage(image);

    return 0;
}
