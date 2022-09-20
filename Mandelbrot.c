/*
 * Created by Charalampos Piotopoulos and Pascal Gaertner
 */

#include <stdio.h>
#include <complex.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <ctype.h>

void mandelbrot_V0(float complex start, size_t width, size_t height, float res, unsigned n, unsigned char* img)
{
    double CX, CY; //the real (x) and imaginary (y) part of C from the formula Zn = Z(n-1)^2 + c

    const double CxMin=-2.5; //needed these adjustments for the picture to be in the middle
    const double CyMin=-2.0; //needed these adjustments for the picture to be in the middle

    double PixelWidth = (res) / width; //size of pixel width
    double PixelHeight = (res) / height; //size of pixel height

    static unsigned char color[3];

    double Zx, Zy; //x -> real and y -> imaginary
    double Zx2, Zy2; //Zx^2 and Zy^2
    const double EscapeRadius = 2;
    double ER2 = EscapeRadius * EscapeRadius; //an additional check whether the number is part of the set

    int Iteration = 0; //so it can be used outside the for loop


    for(int y = 0; y < height; y++) //iterate through y
    {
        CY =  CyMin + y * PixelHeight; //actual y coordinate of pixel being worked on
        if (fabs(CY) < PixelHeight / 2) CY = 0.0; //check if i actually moved a pixel

        for(int x = 0; x < width; x++) //iterate through x
        {
            CX = CxMin + x * PixelWidth; //actual x coordinate of pixel being worked on

            Zx = cabs(start);
            Zy = carg(start);
            Zx2 = Zx;
            Zy2 = Zy;

            for (Iteration = 0; Iteration < n && ((Zx2 + Zy2) < ER2); Iteration++)
            { //(Zx + Zy)^2 + c
                //Zx^2 + 2*Zx*Zy - Zy^2 + c
                //then split into real: Zx^2 - Zy^2 + Cx
                //and
                //imaginary: 2*Zx*Zy + Cy
                Zy = 2 * Zx * Zy + CY;
                Zx = Zx2 - Zy2 + CX;
                Zx2 = Zx * Zx;
                Zy2 = Zy * Zy;
            };

            //if we leave the for loop because the number of our iterations reaches n, then we color the pixel black else white
            img[y*width+x] = (Iteration == n) ? 0 : 255;
        }
    }
}

void mandelbrot_V1_helper (double z, double z_imag, size_t x, size_t y, size_t width, size_t height, float res, unsigned n, unsigned char* img)
{
    // calculating the complex number for the current pixel
    double real_c = (x - width / (res / 2)) * res / width;
    double imag_c = (y - height / (res / 2)) * res / width;
    int i = 0;

    // iterating until the number of iteration has exceeded the maximum given
    while (z*z+z_imag*z_imag < res && i < n) {
        double temp = z*z - z_imag*z_imag + real_c;
        z_imag = (res/2)*z*z_imag + imag_c;
        z = temp;
        i++;
    }

    // setting the pixel to the correct color according to the number of iterations
    img[(y)*width+(x)] = (i == n) ? 0 : 255;
}

void mandelbrot_V1(float complex start, size_t width, size_t height, float res, unsigned n, unsigned char* img)
{
    // initializing the complex number
    double z = cabs(start);
    double z_imag = carg(start);

    // iterating over every pixel using the SSE instructions
    for (size_t y = 0; y <= height; y++)
    {
        size_t x;
        size_t remainder = width % 8;
        for (x = 0; x <= width - remainder; x+=8)
        {
            mandelbrot_V1_helper(z, z_imag, x, y, width, height, res, n, img);
            mandelbrot_V1_helper(z, z_imag, x+1, y, width, height, res, n, img);
            mandelbrot_V1_helper(z, z_imag, x+2, y, width, height, res, n, img);
            mandelbrot_V1_helper(z, z_imag, x+3, y, width, height, res, n, img);
            mandelbrot_V1_helper(z, z_imag, x+4, y, width, height, res, n, img);
            mandelbrot_V1_helper(z, z_imag, x+5, y, width, height, res, n, img);
            mandelbrot_V1_helper(z, z_imag, x+6, y, width, height, res, n, img);
            mandelbrot_V1_helper(z, z_imag, x+7, y, width, height, res, n, img);
        }
        for (int i = 0; i < remainder; i++)
        {
            mandelbrot_V1_helper(z, z_imag, x+i, y, width, height, res, n, img);
            x++;
        }
    }
}

int main(int argc, char *argv[])
{
    char *nameOfProgram = "mandelbrot.ppm"; //default name of the picture
    size_t width = 1000; //default size of picture width
    size_t height = 1000; //default size of picture height

    float complex start = 0.0 + 0.0 * I;
    size_t implementation = 0;
    size_t repeats = 1; //default number of repeats

    float res = 4.0; //default resolution
    unsigned n = 255; //default maximum number of iterations

    size_t time = 0; //show run time

    for (size_t i = 1; i < argc; i++) {
        if (strncmp(argv[i], "-V", 2) == 0) {
            if (strlen(argv[i]) < 3) {
                implementation = 0;
            } else {
                char *str = argv[i], *p = str;
                while (*p) {
                    if (isdigit(*p)) {
                        implementation = strtol(p, &p, 10);
                    } else {
                        p++;
                    }
                }
                if (implementation >
                    1) { //needs to be adjusted in the future depending on the amount of implementations we have
                    printf("An implementation with the number %zu does not exist\n", implementation);
                    return 0;
                }
            }
            printf("V%zu is used\n", implementation);
        } else if (strncmp(argv[i], "-B", 2) == 0) {
            time = 1;
            char *str = argv[i], *p = str;
            while (*p) {
                if (isdigit(*p)) {
                    repeats = strtol(p, &p, 10);
                } else {
                    p++;
                }
            }

            if (repeats < 1) {
                printf("Not enough repeats, number must be greater than 0\n");
                return 0;
            }
            printf("Program will be run %zu times\n", repeats);
        } else if (strncmp(argv[i], "-s", 2) == 0) {
            if (strlen(argv[i]) < 5) {
                printf("Input Missing");
                return 0;
            }
            if (strchr(argv[i], ',') == NULL) {
                printf("Input Missing");
                return 0;
            }
            char *tempReal = strtok(argv[i], ","), *p = tempReal;
            char *tempImaginart = strtok(NULL, ""), *l = tempImaginart;
            if (strlen(tempReal) == 2 || tempImaginart == NULL) {
                printf("Input Missing");
                return 0;
            }
            float tReal;
            int counter1 = 0;
            while (*p) {
                if (isdigit(*p)) {
                    tReal = strtof(p, &p);
                    counter1++;
                } else {
                    p++;
                }
            }
            float tImaginary;
            int counter2 = 0;
            while (*l) {
                if (isdigit(*l)) {
                    tImaginary = strtof(l, &l);
                    counter2++;
                } else {
                    l++;
                }
            }
            if (counter1 == 0 || counter2 == 0) {
                printf("Input Missing");
                return 0;
            }
            start = tReal + tImaginary * I;
            printf("Your start has been changed to real: %f and imaginary: %f * I\n", tReal, tImaginary);
        } else if (strncmp(argv[i], "-d", 2) == 0) {
            if (strlen(argv[i]) < 5) {
                printf("Input Missing");
                return 0;
            }
            if (strchr(argv[i], ',') == NULL) {
                printf("Input Missing");
                return 0;
            }
            char *tempWidth = strtok(argv[i], ","), *p = tempWidth;
            char *tempHeight = strtok(NULL, ""), *l = tempHeight;
            if (strlen(tempWidth) == 2 || tempHeight == NULL) {
                printf("Input Missing");
                return 0;
            }
            size_t tWidth;
            int counter1 = 0;
            while (*p) {
                if (isdigit(*p)) {
                    tWidth = strtol(p, &p, 10);
                    counter1++;
                } else {
                    p++;
                }
            }
            size_t tHeight;
            int counter2 = 0;
            while (*l) {
                if (isdigit(*l)) {
                    tHeight = strtol(l, &l, 10);
                    counter2++;
                } else {
                    l++;
                }
            }
            if (counter1 == 0 || counter2 == 0) {
                printf("Input Missing");
                return 0;
            }
            width = tWidth;
            height = tHeight;
            printf("Your width has been set to %zu and your height to %zu\n", tWidth, tHeight);
        } else if (strncmp(argv[i], "-n", 2) == 0) {
            if (strlen(argv[i]) < 3) {
                printf("Input Missing");
                return 0;
            }
            char *str = argv[i], *p = str;
            unsigned temp;
            int counter = 0;
            while (*p) {
                if (isdigit(*p)) {
                    temp = strtol(p, &p, 10);
                    counter++;
                } else {
                    p++;
                }
            }
            if (counter == 0) {
                printf("Input Missing");
                return 0;
            }
            n = temp;
            printf("%d", temp);
            printf("The amount of iterations has been set to: %d\n", temp);
        } else if (strncmp(argv[i], "-r", 2) == 0) {
            if (strlen(argv[i]) < 3) {
                printf("Input Missing");
                return 0;
            }
            char *str = argv[i], *p = str;
            float temp;
            int counter = 0;
            while (*p) {
                if (isdigit(*p)) {
                    temp = strtof(p, &p);
                    counter++;
                } else {
                    p++;
                }
            }
            if (counter == 0) {
                printf("Input Missing");
                return 0;
            }
            res = temp;
            printf("Your resolution has now been set to: %f\n", temp);
        } else if (strncmp(argv[i], "-o", 2) == 0) {
            if (strlen(argv[i]) < 3) {
                printf("Input Missing");
                return 0;
            }
            char *temp = strtok(argv[i], "o");
            temp = strtok(NULL, "");
            nameOfProgram = temp;
            printf("The name of the picture has been set to: %s\n(Tip: Don't forget the .ppm at the end)\n", temp);
        } else if (strcmp(argv[i], "-h") == 0) {
            printf("-V<int>: Selects the implementation to use\n-B<int>: Selects the amount of repeats\n-s<real>,<imag>: Selects the start\n-d<int>,<int>: Selects the width and height\n-n<int>: Selects the amount of iterations\n-r<float>: Selects the resolution\n-o<file>: Selects the name of the picture\n-h: Shows this help\n");
        } else if (strcmp(argv[i], "--help") == 0) {
            printf("-V<int>: Selects the implementation to use\n-B<int>: Selects the amount of repeats\n-s<real>,<imag>: Selects the start\n-d<int>,<int>: Selects the width and height\n-n<int>: Selects the amount of iterations\n-r<float>: Selects the resolution\n-o<file>: Selects the name of the picture\n-h: Shows this help\n");
        }
    }

    unsigned char* img = malloc(width * height); //reserving memory for image

    FILE* f = fopen(nameOfProgram, "w");

    clock_t begin, end;
    float z;

    begin = clock();

    switch (implementation) {
        case 0:
            for (size_t i = 0; i < repeats; i++) {
                mandelbrot_V0(start, width, height, res, n, img);
            }
            break;
        case 1:
            for (size_t i = 0; i < repeats; i++) {
                mandelbrot_V1(start, width, height, res, n, img);
            }
    }

    end = clock();
    z = end - begin;
    z /= CLOCKS_PER_SEC;
    if (time != 0) {
        printf("Runtime: %f seconds\n", z);
    }

    fprintf(f, "P3\n%zu %zu\n255\n", width, height);

    for (size_t i = 0; i < width * height; i++)
    {
        fprintf(f, "%u %u %u ", img[i], img[i], img[i]);
    }

    fclose(f);
    free(img);
    return 0;
}