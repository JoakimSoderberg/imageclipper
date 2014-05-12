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
#ifndef CV_SETCOL_INCLUDED
#define CV_SETCOL_INCLUDED


#include "cv.h"
#include "cvaux.h"

CV_INLINE void cvSetCols( const CvArr* src, CvArr* dst,
                          int start_col, int end_col );
#define cvSetCol(src, dst, col) (cvSetCols( src, dst, col, col + 1))

/**
 * Set array col or col span
 *
 * Following code is faster than using this function because it does not 
 * require cvCopy()
 * <code>
 * CvMat* submat, submathdr;
 * submat = cvGetCols( mat, &submathdr, start_col, end_col, delta_col );
 * // Write on submat
 * </code>
 *
 * @param src       Source array
 * @param dst       Target array. Either of array must be size of setting cols.
 * @param start_col Zero-based index of the starting col (inclusive) of the span. 
 * @param end_col   Zero-based index of the ending col (exclusive) of the span. 
 * @return void
 * @see cvSetCol( src, dst, col ) // cvSetCols( src, dst, col, col + 1 )
 */
CV_INLINE void cvSetCols( const CvArr* src, CvArr* dst,
                          int start_col, int end_col )
{
    int coi;
    CvMat *srcmat = (CvMat*)src, srcmatstub;
    CvMat *dstmat = (CvMat*)dst, dstmatstub;
    CvMat *refmat, refmathdr;
    int cols;
    CV_FUNCNAME( "cvSetCols" );
    __BEGIN__;
    if( !CV_IS_MAT(dstmat) )
    {
        CV_CALL( dstmat = cvGetMat( dstmat, &dstmatstub, &coi ) );
        if (coi != 0) CV_ERROR_FROM_CODE(CV_BadCOI);
    }
    if( !CV_IS_MAT(srcmat) )
    {
        CV_CALL( srcmat = cvGetMat( srcmat, &srcmatstub, &coi ) );
        if (coi != 0) CV_ERROR_FROM_CODE(CV_BadCOI);
    }
    cols = end_col - start_col;
    CV_ASSERT( srcmat->cols == cols || dstmat->cols == cols );
    if( srcmat->cols == cols )
    {
        refmat = cvGetCols( dstmat, &refmathdr, start_col, end_col );
        cvCopy( srcmat, refmat );
    }
    else
    {
        refmat = cvGetCols( srcmat, &refmathdr, start_col, end_col );
        cvCopy( refmat, dstmat );
    }
    __END__;
}

/*
CVAPI( void ) cvSetCols( const CvArr* subarr, CvArr* arr, int start_col, int end_col )
{
CV_FUNCNAME( "cvSetCols" );
    __BEGIN__;
    int col, row, elem;
    int coi = 0;
    CvMat* submat = (CvMat*)subarr, submatstub;
    CvMat* mat = (CvMat*)arr, matstub;

    if( !CV_IS_MAT(submat) )
    {
        CV_CALL( submat = cvGetMat( submat, &submatstub, &coi ) );
        if (coi != 0) CV_ERROR_FROM_CODE(CV_BadCOI);
    }
    if( !CV_IS_MAT(mat) )
    {
        CV_CALL( mat = cvGetMat( mat, &matstub, &coi ) );
        if (coi != 0) CV_ERROR_FROM_CODE(CV_BadCOI);
    }
    CV_ARE_TYPES_EQ( submat, mat );
    CV_ARE_DEPTHS_EQ( submat, mat );
    CV_ASSERT( 0 <= start_col && end_col <= mat->cols );
    CV_ASSERT( end_col - start_col == submat->cols );
    CV_ASSERT( mat->rows == submat->rows );

    int elem_size = CV_ELEM_SIZE( mat->type );
    for( col = start_col; col < end_col; col++ )
    {
        for( row = 0; row < mat->rows; row++ )
        {
            for( elem = 0; elem < elem_size; elem++ )
            {
                (mat->data.ptr + ((size_t)mat->step * row) + (elem_size * col))[elem] = 
                    (submat->data.ptr + ((size_t)submat->step * row) + (elem_size * (col - start_col)))[elem];
            }
        }
    }
    __END__;
}
*/

#endif

