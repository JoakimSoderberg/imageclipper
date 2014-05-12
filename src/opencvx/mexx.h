/**
* Matlab C Library (MEX) eXtension
*     verified Matlab 2007b
*
* The MIT License
* 
* Copyright (c) 2008, Naotoshi Seo <sonots(at)sonots.com>
* 
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* 
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/
#ifndef MXX_INCLUDED
#define MXX_INCLUDED


#include "mat.h"
#include "matrix.h"

/**
* definitions 
*/
void mxPrintMatrix(const mxArray* mxarr);
void mxSetCol(mxArray* X, mxArray* X1, int col);
void mxSetRow(mxArray* X, mxArray* X1, int row);
mxArray* mxGetCol(mxArray* X, int col);
mxArray* mxGetRow(mxArray* X, int row);


/**
* Get a pointer to (row,col)
*
* @param mxArray *X 2-D Array
* @param type       object type such as double, uint
* @param int row
* @param int col
*/
#define MX_ARRAY_ELEM(X, type, row, col) (((type*)mxGetPr(X))[mxGetM(X)*col+row])

/**
* Print mxArray 
*
* Currently support only uint8, float, double
* Currently support only upto 3-D. 
*
* @param mxArray* mxarr
* @return void
*/
void mxPrintMatrix(const mxArray* mxarr)
{
    int nDim;
    const mwSize *dims;
    int row, col, ch, nChannel = 1;
    mxClassID classid;
    classid = mxGetClassID(mxarr);
    nDim = mxGetNumberOfDimensions(mxarr);
    dims = mxGetDimensions(mxarr);
    if (nDim >= 3) nChannel = dims[2];

    if (classid == mxUINT8_CLASS) {
        unsigned char *mxData = (unsigned char*)mxGetData(mxarr);
        for (ch = 0; ch < nChannel; ch++) {
            for (row = 0; row < dims[0]; row++) {
                for (col = 0; col < dims[1]; col++) {
                    printf("%d ", mxData[
                        dims[0] * dims[1] * ch + dims[0] * col + row]);
                }
                printf("\n");
            }
            printf("\n");
        }
    } else if (classid == mxDOUBLE_CLASS) {
        double *mxData = (double*)mxGetData(mxarr);
        for (ch = 0; ch < nChannel; ch++) {
            for (row = 0; row < dims[0]; row++) {
                for (col = 0; col < dims[1]; col++) {
                    printf("%lf ", mxData[
                        dims[0] * dims[1] * ch + dims[0] * col + row]);
                }
                printf("\n");
            }
            printf("\n");
        }
    } else if (classid == mxSINGLE_CLASS) {
        float *mxData = (float*)mxGetData(mxarr);
        for (ch = 0; ch < nChannel; ch++) {
            for (row = 0; row < dims[0]; row++) {
                for (col = 0; col < dims[1]; col++) {
                    printf("%lf ", mxData[
                        dims[0] * dims[1] * ch + dims[0] * col + row]);
                }
                printf("\n");
            }
            printf("\n");
        }
    }
}

/**
* Set a column
*
* @param mxArray *X 2-D Array
* @param int col
* @return mxArray*
*/
void mxSetCol(mxArray* X, mxArray* X1, int col)
{
    int nRow = mxGetM(X);
    int nCol = mxGetN(X);
    int row = 0;
    double *Xdata, *X1data;
    Xdata  = (double*)mxGetPr(X);
    X1data = (double*)mxGetPr(X1);
    for (row = 0; row < nRow; row++) {
        Xdata[nRow * col + row] =  X1data[row];
    }
}

/**
* Set a row
*
* @param mxArray *X 2-D Array
* @param int col
* @return mxArray*
*/
void mxSetRow(mxArray* X, mxArray* X1, int row)
{
    int nRow = mxGetM(X);
    int nCol = mxGetN(X);
    int col = 0;
    double *Xdata, *X1data;
    Xdata  = (double*)mxGetPr(X);
    X1data = (double*)mxGetPr(X1);
    for (col = 0; col < nCol; col++) {
        Xdata[nRow * col + row] = X1data[col];
    }
}

/**
* Get a column
*
* @param mxArray *X 2-D Array
* @param int col
* @return mxArray*
*/
mxArray* mxGetCol(mxArray* X, int col)
{
    int nRow = mxGetM(X);
    int nCol = mxGetN(X);
    int row = 0;
    mxArray *X1;
    double *Xdata, *X1data;
    if(col > nCol) return NULL;
    X1     = mxCreateDoubleMatrix(nRow, 1, mxREAL);
    Xdata  = (double*)mxGetPr(X);
    X1data = (double*)mxGetPr(X1);
    for (row = 0; row < nRow; row++) {
        X1data[row] = Xdata[nRow * col + row];
    }
    return X1;
}

/**
* Get a row
*
* @param mxArray *X 2-D Array
* @param int row
* @return mxArray*
*/
mxArray* mxGetRow(mxArray* X, int row)
{
    int nRow = mxGetM(X);
    int nCol = mxGetN(X);
    int col = 0;
    mxArray *X1;
    double *Xdata, *X1data;
    if(row > nRow) return NULL;
    X1     = mxCreateDoubleMatrix(1, nCol, mxREAL);
    Xdata  = (double*)mxGetPr(X);
    X1data = (double*)mxGetPr(X1);
    for (col = 0; col < nCol; col++) {
        X1data[col] = Xdata[nRow * col + row];
    }
    return X1;
}


#endif