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
#ifndef CV_CAT_INCLUDED
#define CV_CAT_INCLUDED


#include "cv.h"
#include "cvaux.h"
#include "cvsetrow.h"
#include "cvsetcol.h"

CVAPI( void ) cvCat( const CvArr* src1arr, const CvArr* src2arr, CvArr* dstarr, int dim = -1 );
#define cvHcat( src1, src2, dst ) cvCat( (src1), (src2), (dst), 0 )
#define cvVcat( src1, src2, dst ) cvCat( (src1), (src2), (dst), 1 )

/**
* Concatinate arrays
*
* Example)
*    IplImage* img    = cvCreateImage( cvSize(4,4), IPL_DEPTH_8U, 1 ); // width(cols), height(rows)
*    IplImage* subimg = cvCreateImage( cvSize(1,4), IPL_DEPTH_8U, 1 );
*    IplImage* catimg = cvCreateImage( cvSize(5,4), IPL_DEPTH_8U, 1 );
*    cvSet( img, cvScalar(1) );
*    cvSet( subimg, cvScalar(0) );
*    cvHcat( img, subimg, catimg ); // 4x4 + 4x1 = 4x5
*    cvMatPrint( catimg );
*
* @param src1 Input array 1
* @param src2 Input array 2
* @param dst Target array
* @param dim 0 horizontally, 1 vertically, -1 flexibly
* @see cvHcat( src1, src2, dst ) // cvCat( src1, src2, dst, 0 )
* @see cvVcat( src1, src2, dst ) // cvCat( src1, src2, dst, 1 )
*/
//CV_IMPL void
CVAPI( void ) cvCat( const CvArr* src1arr, const CvArr* src2arr, CvArr* dstarr, int dim )
{
    int coi = 0;
    CvMat *src1 = (CvMat*)src1arr, src1stub;
    CvMat *src2 = (CvMat*)src2arr, src2stub;
    CvMat *dst  = (CvMat*)dstarr, dststub;
    CV_FUNCNAME( "cvCat" );
    __BEGIN__;
    if( !CV_IS_MAT(src1) )
    {
        src1 = cvGetMat( src1, &src1stub, &coi );
        if (coi != 0) CV_ERROR_FROM_CODE(CV_BadCOI);
    }
    if( !CV_IS_MAT(src2) )
    {
        src2 = cvGetMat( src2, &src2stub, &coi );
        if (coi != 0) CV_ERROR_FROM_CODE(CV_BadCOI);
    }
    if( !CV_IS_MAT(dst) )
    {
        dst = cvGetMat( dst, &dststub, &coi );
        if (coi != 0) CV_ERROR_FROM_CODE(CV_BadCOI);
    }
    if( dim == -1 )
    {
        if( src1->rows == src2->rows && src1->rows == dst->rows )
        {
            dim = 0;
        }
        else if( src1->cols == src2->cols && src1->cols == dst->cols )
        {
            dim = 1;
        }
        else
        {
            CV_ERROR( CV_StsBadArg, "The size of matrices does not fit to concatinate." );
        }
    }
    if( dim == 0 ) // horizontal cat
    {
        CV_ASSERT( src1->rows == src2->rows && src1->rows == dst->rows );
        CV_ASSERT( src1->cols + src2->cols == dst->cols );
    }
    else if( dim == 1 ) // vertical cat
    {
        CV_ASSERT( src1->cols == src2->cols && src1->cols == dst->cols );
        CV_ASSERT( src1->rows + src2->rows == dst->rows );
    }
    else
    {
        CV_ERROR( CV_StsBadArg, "The dim is 0 (horizontal) or 1 (vertical) or -1 (flexible)." );
    }

    if( dim == 0 ) // horizontal cat
    {
        cvSetCols( src1, dst, 0, src1->cols );
        cvSetCols( src2, dst, src1->cols, src1->cols + src2->cols );
    }
    else // vertical cat
    {
        cvSetRows( src1, dst, 0, src1->rows );
        cvSetRows( src2, dst, src1->rows, src1->rows + src2->rows );
    }
    __END__;
}


#endif

