#include "helpers.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

double matrixmult(int a[3][3], int b[3][3]);

// Convert image to grayscale
void grayscale(int height, int width, RGBTRIPLE image[height][width])
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            float x = 1.0 * (image[i][j].rgbtRed + image[i][j].rgbtGreen + image[i][j].rgbtBlue);
            int a = (int)round(x / 3);

            image[i][j].rgbtRed = a;
            image[i][j].rgbtGreen = a;
            image[i][j].rgbtBlue = a;
        }
    }
    return;
}

// Reflect image horizontally
void reflect(int height, int width, RGBTRIPLE image[height][width])
{
    RGBTRIPLE img[width];
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            img[(width - 1) - j].rgbtRed = image[i][j].rgbtRed;
            img[(width - 1) - j].rgbtGreen = image[i][j].rgbtGreen;
            img[(width - 1) - j].rgbtBlue = image[i][j].rgbtBlue;
        }
        for (int j = 0; j < width; j++)
        {
            image[i][j].rgbtRed = img[j].rgbtRed;
            image[i][j].rgbtGreen = img[j].rgbtGreen;
            image[i][j].rgbtBlue = img[j].rgbtBlue;
        }
    }
    return;
}

// Blur image
void blur(int height, int width, RGBTRIPLE image[height][width])
{
    RGBTRIPLE(*t)
    [width] = calloc(height, width * sizeof(RGBTRIPLE));
    for (int x = 0; x < height; x++)
    {
        for (int y = 0; y < width; y++)
        {
            // Taking averages of 3x3 pixels
            int tr = 0, tg = 0, tb = 0;
            int n = 0;
            for (int i = -1; i <= 1; ++i)
            {
                if (x + i >= 0 && x + i < height)
                {
                    for (int j = -1; j <= 1; ++j)
                    {
                        if (y + j >= 0 && y + j < width)
                        {
                            tr = tr + image[x + i][y + j].rgbtRed;
                            tg = tg + image[x + i][y + j].rgbtGreen;
                            tb = tb + image[x + i][y + j].rgbtBlue;
                            n++;
                        }
                    }
                }
            }
            float r = tr * 1.0 / n;
            float g = tg * 1.0 / n;
            float b = tb * 1.0 / n;
            t[x][y].rgbtRed = (int)round(r);
            t[x][y].rgbtGreen = (int)round(g);
            t[x][y].rgbtBlue = (int)round(b);
        }
    }
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            image[i][j].rgbtRed = t[i][j].rgbtRed;
            image[i][j].rgbtGreen = t[i][j].rgbtGreen;
            image[i][j].rgbtBlue = t[i][j].rgbtBlue;
        }
    }

    return;
}

// Detect edges
void edges(int height, int width, RGBTRIPLE image[height][width])
{
    int Gx[3][3] = {
        {-1, 0, 1},
        {-2, 0, 2},
        {-1, 0, 1}};
    int Gy[3][3] = {
        {-1, -2, -1},
        {0, 0, 0},
        {1, 2, 1}};

    RGBTRIPLE(*t)
    [width] = calloc(height, width * sizeof(RGBTRIPLE));
    for (int x = 0; x < height; x++)
    {
        for (int y = 0; y < width; y++)
        {
            // Building 2d arrays
            double Gxr, Gxg, Gxb;
            double Gyr, Gyg, Gyb;
            int rmat[3][3], gmat[3][3], bmat[3][3];

            // Array of red pixels
            for (int i = -1; i <= 1; ++i)
            {
                for (int j = -1; j <= 1; ++j)
                {
                    if (x + i >= 0 && x + i < height && y + j >= 0 && y + j < width)
                    {
                        rmat[i + 1][j + 1] = image[x + i][y + j].rgbtRed;
                    }
                    else
                    {
                        rmat[i + 1][j + 1] = 0;
                    }
                }
            }

            // Array of Green pixels
            for (int i = -1; i <= 1; ++i)
            {
                for (int j = -1; j <= 1; ++j)
                {
                    if (x + i >= 0 && x + i < height && y + j >= 0 && y + j < width)
                    {
                        gmat[i + 1][j + 1] = image[x + i][y + j].rgbtGreen;
                    }
                    else
                    {
                        gmat[i + 1][j + 1] = 0;
                    }
                }
            }

            // Array if blue pixels
            for (int i = -1; i <= 1; ++i)
            {
                for (int j = -1; j <= 1; ++j)
                {
                    if (x + i >= 0 && x + i < height && y + j >= 0 && y + j < width)
                    {
                        bmat[i + 1][j + 1] = image[x + i][y + j].rgbtBlue;
                    }
                    else
                    {
                        bmat[i + 1][j + 1] = 0;
                    }
                }
            }

            // Calculating Gx and Gy values
            Gxr = pow(matrixmult(Gx, rmat), 2);
            Gyr = pow(matrixmult(Gy, rmat), 2);
            Gxg = pow(matrixmult(Gx, gmat), 2);
            Gyg = pow(matrixmult(Gy, gmat), 2);
            Gxb = pow(matrixmult(Gx, bmat), 2);
            Gyb = pow(matrixmult(Gy, bmat), 2);

            // Calculating G
            float r = sqrt(Gxr + Gyr * 1.00);
            float g = sqrt(Gxg + Gyg * 1.00);
            float b = sqrt(Gxb + Gyb * 1.00);

            // Capping at 255
            if (r > 255)
            {
                r = 255;
            }
            if (g > 255)
            {
                g = 255;
            }
            if (b > 255)
            {
                b = 255;
            }

            t[x][y].rgbtRed = (int)round(r);
            t[x][y].rgbtGreen = (int)round(g);
            t[x][y].rgbtBlue = (int)round(b);
        }
    }
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            image[i][j].rgbtRed = t[i][j].rgbtRed;
            image[i][j].rgbtGreen = t[i][j].rgbtGreen;
            image[i][j].rgbtBlue = t[i][j].rgbtBlue;
        }
    }

    return;
}

double matrixmult(int a[3][3], int b[3][3])
{
    double result = 0.0;

    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            result += a[i][j] * b[j][i];
        }
    }
    return result;
}