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
#ifndef CV_SETROW_INCLUDED
#define CV_SETROW_INCLUDED


#include "cv.h"
#include "cvaux.h"

CV_INLINE void cvSetRows( const CvArr* src, CvArr* dst,
                         int start_row, int end_row, int delta_row = 1 );
#define cvSetRow(src, dst, row) (cvSetRows( src, dst, row, row + 1))

/**
 * Set array row or row span
 *
 * Following code is faster than using this function because it does not 
 * require cvCopy()
 * <code>
 * CvMat* submat, submathdr;
 * submat = cvGetRows( mat, &submathdr, start_row, end_row, delta_row );
 * // Write on submat
 * </code>
 *
 * @param src       Source array
 * @param dst       Target array. Either of array must be size of setting rows.
 * @param start_row Zero-based index of the starting row (inclusive) of the span. 
 * @param end_row   Zero-based index of the ending row (exclusive) of the span. 
 * @param [delta_row = 1]
 *                  Index step in the row span. That is, the function extracts every 
 *                  delta_row-th row from start_row and up to (but not including) end_row. 
 * @return void
 * @see cvSetRow( src, dst, row ) // cvSetCols( src, dst, row, row + 1 )
 */
CV_INLINE void cvSetRows( const CvArr* src, CvArr* dst,
                         int start_row, int end_row, int delta_row )
{
    int coi;
    CvMat *srcmat = (CvMat*)src, srcmatstub;
    CvMat *dstmat = (CvMat*)dst, dstmatstub;
    CvMat *refmat, refmathdr;
    int rows;
    CV_FUNCNAME( "cvSetRows" );
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
    rows = cvFloor( ( end_row - start_row ) / delta_row );
    CV_ASSERT( srcmat->rows == rows || dstmat->rows == rows );
    if( srcmat->rows == rows )
    {
        refmat = cvGetRows( dstmat, &refmathdr, start_row, end_row, delta_row );
        cvCopy( srcmat, refmat );
    }
    else
    {
        refmat = cvGetRows( srcmat, &refmathdr, start_row, end_row, delta_row );
        cvCopy( refmat, dstmat );
    }
    __END__;
}
/*
CVAPI( void ) cvSetRows( const CvArr* subarr, CvArr* arr, int start_row, int end_row )
{
    CV_FUNCNAME( "cvSetRows" );
    __BEGIN__;
    int row, col, elem;
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
    CV_ARE_TYPES_EQ( submat, mat ); // cxtypes.h
    //CV_ARE_DEPTHS_EQ( submat, mat );
    //CV_ARE_CNS_EQ( submat, mat );
    //CV_ARE_SIZES_EQ( submat, mat );
    CV_ASSERT( 0 <= start_row && end_row <= mat->rows );
    CV_ASSERT( end_row - start_row == submat->rows );
    CV_ASSERT( mat->cols == submat->cols );

    int elem_size = CV_ELEM_SIZE( mat->type ); 
    // FYI: CV_ELEM_SIZE returns not number_of_channels, but number_of_uchar*
    // nChannels = CV_MAT_CN( mat->type ); refer cxarray.cpp#cvRawDataToScalar to separate into channel values
    for( row = start_row; row < end_row; row++ )
    {
        for( col = 0; col < mat->cols; col++ )
        {
            //cvSet2D( mat, row, col, cvGet2D( submat, row - start_row, col ) );
            for( elem = 0; elem < elem_size; elem++ )
            {
                // cvPtr2D( mat, row, col )[elem] = cvPtr2D( submat, row - start_row, col )[elem];
                (mat->data.ptr + ((size_t)mat->step * row) + (elem_size * col))[elem] = 
                    (submat->data.ptr + ((size_t)submat->step * (row - start_row)) + (elem_size * col))[elem];
            }
        }
    }
    __END__;
}
*/

#endif
