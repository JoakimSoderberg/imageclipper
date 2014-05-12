/** @file
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
#ifndef CV_PRINTMAT_INCLUDED
#define CV_PRINTMAT_INCLUDED


#include "cv.h"
#include "cvaux.h"
#include <stdio.h>
#include <iostream>

CV_INLINE void cvPrintMatProperty( const CvMat* mat );
CV_INLINE void cvPrintImageProperty( const IplImage* img );
CV_INLINE void cvPrintMat( const CvArr* arr );

/**
* Print CvMat Property
*
* @param mat
* @return void
*/
CV_INLINE void cvPrintMatProperty( const CvMat* mat )
{
    printf("CvMat Property\n");
    printf(" rows: %d\n", mat->rows);
    printf(" cols: %d\n", mat->cols);
    printf(" type: %d\n", mat->type);
    printf(" step: %d\n", mat->step);
    fflush( stdout );
}

/**
* Print IplImage Property
*
* @param img
* @return void
*/
CV_INLINE void cvPrintImageProperty( const IplImage* img )
{
    printf("IplImage Property\n");
    printf(" width: %d\n", img->width);
    printf(" height: %d\n", img->height);
    printf(" depth: %d\n", img->depth);
    printf(" nChannels: %d\n", img->nChannels);
    fflush( stdout );
}

/**
* Print array
*
* @param arr array
* @return void
*/
CV_INLINE void cvPrintMat( const CvArr* arr )
{
    CV_FUNCNAME( "cvPrintMat" );
    __BEGIN__;
    int row, col, ch;
    int coi = 0;
    CvMat matstub, *mat = (CvMat*)arr;
    int depth, nChannels;
    CvScalar value;
    if( !CV_IS_MAT( mat ) )
    {
        CV_CALL( mat = cvGetMat( mat, &matstub, &coi ) ); // i.e. IplImage to CvMat
        if (coi != 0) CV_ERROR_FROM_CODE(CV_BadCOI);
    }
    depth = CV_MAT_DEPTH( mat->type );
    nChannels = CV_MAT_CN( mat->type );
    for( row = 0; row < mat->rows; row++ )
    {
        for( col = 0; col < mat->cols; col++ )
        {
            value = cvGet2D( mat, row, col );
            if( nChannels > 1 )
            {
                printf( "(%lf", value.val[0] );
                for( ch = 1; ch < nChannels; ch++ )
                {
                    printf( " %lf", value.val[ch] );
                }
                printf( ") " );
            }
            else
            {
                printf( "%lf ", value.val[0] );
            }
        }
        printf("\n");
    }
    fflush( stdout );
    __END__;
}


#endif
