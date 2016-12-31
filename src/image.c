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

Coord setCoord(short x, short y)
{
    Coord ret;
    ret.x = x;
    ret.y = y;
    return ret;
}

EdgePixel setEdgePixel(short x, short y, Pixel pixel)
{
    EdgePixel ret;
    ret.coord = setCoord(x, y);
    ret.color = pixel;
    return ret;
}    

int abs(int x)
{
    if (x < 0)
        return -1 * x;
    return x;
}

int pixelDiff(Pixel x, Pixel y)
{
    return abs((int)x.red - (int)y.red) + abs((int)x.green - (int)y.green)
        + abs((int)x.blue - (int)y.blue);
}

EdgePixel *fill(Image *image, int lin, int col, int threshold, char **vis, int *size)
{
    int queue_size = 1;
    int queue_capacity = 100;
    Coord *queue = malloc(sizeof(Coord) * queue_capacity);
    int edge_size = 0;
    int edge_capacity = 30;
    EdgePixel *edge_pixels = malloc(sizeof(EdgePixel) * edge_capacity);
    
    queue[0] = setCoord(lin, col);
    vis[lin][col] = 1;
    short dx[] = {-1, 0, 1, 0};
    short dy[] = {0, 1, 0, -1};
    for (int left = 0; left < queue_size; left++)
    {
        char edge = 0;
        for (int i = 0; i < 4; i++)
        {
            short x = queue[left].x + dx[i];
            short y = queue[left].y + dy[i];
            if (x < 0 || x >= image->height ||
                y < 0 || y >= image->width ||
                vis[x][y] == 2)
            {
                if (edge == 0)
                {
                    edge = 1;
                    if (edge_size == edge_capacity)
                    {
                        edge_capacity *= 2;
                        edge_pixels = realloc(edge_pixels, sizeof(EdgePixel) * edge_capacity);
                        if (edge_pixels == NULL) 
                            fprintf(stderr, "REALLOC FAILED2\n");
                    }
                    edge_pixels[edge_size++] = setEdgePixel(queue[left].x,
                            queue[left].y, image->pixels[lin][col]);
                }
                continue;
            }
            if (pixelDiff(image->pixels[x][y], image->pixels[lin][col]) <=
                    threshold)
            {
                if (vis[x][y] == 0)
                {
                    vis[x][y] = 1;
                    if (queue_size == queue_capacity)
                    {
                        queue_capacity *= 2;
                        queue = realloc(queue, sizeof(Coord) * queue_capacity);
                        if (queue == NULL)
                            fprintf(stderr, "REALLOC FAILED1\n");
                    }
                    queue[queue_size++] = setCoord(x, y);
                }
            }
            else if (edge == 0)
            {
                edge = 1;
                if (edge_size == edge_capacity)
                {
                    edge_capacity *= 2;
                    edge_pixels = realloc(edge_pixels, sizeof(EdgePixel) * edge_capacity);
                    if (edge_pixels == NULL) 
                        fprintf(stderr, "REALLOC FAILED2\n");
                }
                edge_pixels[edge_size++] = setEdgePixel(queue[left].x,
                        queue[left].y, image->pixels[lin][col]);
            }
        }
    }

    for (int i = 0; i < edge_size; i++)
        vis[edge_pixels[i].coord.x][edge_pixels[i].coord.y] = 2;

    free(queue);
    *size = edge_size;
    return edge_pixels;
}

int compare(const void *a, const void *b)
{
    EdgePixel A = *(EdgePixel*)a;
    EdgePixel B = *(EdgePixel*)b;
    if (A.coord.x == B.coord.x)
        return A.coord.y - B.coord.y;
    return A.coord.x - B.coord.x;
}

void compressImage(Image *image, int threshold, 
        struct bmp_fileheader *fileheader, struct bmp_infoheader *infoheader)
{
    char **vis = malloc(sizeof(char*) * image->height);
    for (int i = 0; i < image->height; i++)
    {
        vis[i] = malloc(sizeof(char) * image->width);
        memset(vis[i], 0, image->width);
    }

    EdgePixel *edge_pixels = malloc(1);
    int size = 0;

    for (int lin = 0; lin < image->height; lin++)
        for (int col = 0; col < image->width; col++)
            if (vis[lin][col] == 0)
            {
                int new_size = 0;
                EdgePixel *new_pixels = fill(image, lin, col, threshold, vis,
                        &new_size);
                edge_pixels = realloc(edge_pixels, sizeof(EdgePixel) * (size + new_size));
                if (edge_pixels == NULL) 
                    fprintf(stderr, "REALLOC FAILED4\n");
                for (int i = 0; i < new_size; i++)
                    edge_pixels[size+i] = new_pixels[i];
                size += new_size;
                free(new_pixels);
            }

    qsort(edge_pixels, size, sizeof(EdgePixel), compare);

    FILE *out = fopen("compressed.bin", "wb");

    printHeader(fileheader, infoheader, out);
    for (int i = 0; i < size; i++)
    {
        edge_pixels[i].coord.x++;
        edge_pixels[i].coord.y++;
        fwrite(&(edge_pixels[i].coord.x), 2, 1, out);
        fwrite(&(edge_pixels[i].coord.y), 2, 1, out);
        fwrite(&(edge_pixels[i].color.blue), 1, 1, out);
        fwrite(&(edge_pixels[i].color.green), 1, 1, out);
        fwrite(&(edge_pixels[i].color.red), 1, 1, out);
    }

    free(edge_pixels);

    fclose(out);
}








