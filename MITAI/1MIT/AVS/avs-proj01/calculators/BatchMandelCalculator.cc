/**
 * @file BatchMandelCalculator.cc
 * @author FULL NAME <xholko02@stud.fit.vutbr.cz>
 * @brief Implementation of Mandelbrot calculator that uses SIMD paralelization over small batches
 * @date DATE
 */

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#include <stdlib.h>
#include <stdexcept>

#include <immintrin.h>

#include "BatchMandelCalculator.h"

BatchMandelCalculator::BatchMandelCalculator (unsigned matrixBaseSize, unsigned limit) :
	BaseMandelCalculator(matrixBaseSize, limit, "BatchMandelCalculator")
{
    data = (int *)_mm_malloc(height * width * sizeof(int), 64);
    zReal_arr = (float *)_mm_malloc(blockSize * sizeof(float), 64);
    zImg_arr = (float *)_mm_malloc(blockSize * sizeof(float), 64);
}

BatchMandelCalculator::~BatchMandelCalculator() {
    _mm_free(data);
    _mm_free(zReal_arr);
    _mm_free(zImg_arr);
}

int * BatchMandelCalculator::calculateMandelbrot () {
    int *pdata = data;
    float *p_zImg = zImg_arr;
    float *p_zReal = zReal_arr;

    // We have set all values to limit in the result matrix
    #pragma omp simd
    for (int i = 0; i < width * height; i++) {
        pdata[i] = limit;
    }

    for (int i = 0; i < height; i++) { // go over all rows in result matrix
        float y = y_start + i * dy;

        for (int blockCol = 0; blockCol < width / blockSize; blockCol++) {

            #pragma omp simd
            for (int a = 0; a < blockSize; a++) {
                p_zImg[a] = y;
                p_zReal[a] = x_start + (a + blockCol * blockSize) * dx;
            }

            for (int k = 0; k < limit; k++) { // go over that row limit times
                int sum = 0;

                #pragma omp simd aligned(p_zImg, p_zReal: 64) reduction(+:sum)
                for (int j = 0; j < blockSize; j++) { // go over all cols in the row
                    const int jGlobal = blockCol * blockSize + j;
                    float x = x_start + jGlobal * dx;
                    float real = *(p_zReal + j);
                    float imag = *(p_zImg + j);

                    float r2 = real * real;
                    float i2 = imag * imag;

                    sum += (r2 + i2 > 4.0f) ? 1 : 0;
                    *(pdata + i * width + jGlobal) = (r2 + i2 > 4.0f) ? std::min(*(pdata + i * width + jGlobal), k) : *(pdata + i * width + jGlobal);

                    *(p_zImg + j) = (r2 + i2 < 4.0f) ? 2.0f * real * imag + y : imag;
                    *(p_zReal + j) = (r2 + i2 < 4.0f) ? r2 - i2 + x : real;
                }

                if (sum == blockSize) { // all blown up - no need for more iterations over this row
                    break;
                }
            }
        }


    }

    return data;
}