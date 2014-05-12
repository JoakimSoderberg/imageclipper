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
#ifndef CV_LOGSUM_INCLUDED
#define CV_LOGSUM_INCLUDED

#include "cv.h"
#include "cvaux.h"
#include "cxcore.h"

#include <float.h>
#include <math.h>

CvScalar cvLogSum( const CvArr *arr );

/**
 * cvLogSum
 *
 * Get log(a + b + c) from log(a), log(b), log(c)
 * Useful to take sum of probabilities from log probabilities
 * Useful to avoid loss of precision caused by taking exp
 *
 * @param  arr       array having log values. 32F or 64F
 * @return CvScalar
 */
CvScalar cvLogSum( const CvArr *arr )
{
    IplImage* img = (IplImage*)arr, imgstub;
    IplImage *tmp, *tmp2;
    int row, col, ch;
    CvScalar sumval;
    CvScalar minval, maxval;
    CV_FUNCNAME( "cvLogSum" );
    __BEGIN__;

    if( !CV_IS_IMAGE(img) )
    {
        CV_CALL( img = cvGetImage( img, &imgstub ) );
    }
    tmp = cvCreateImage( cvGetSize(img), img->depth, img->nChannels );
    tmp2 = cvCreateImage( cvGetSize(img), img->depth, img->nChannels );

    // to avoid loss of precision caused by taking exp as much as possible
    // if this trick is not required, cvExp -> cvSum are enough
    for( ch = 0; ch < img->nChannels; ch++ )
    {
        cvSetImageCOI( img, ch + 1 );
        cvMinMaxLoc( img, &minval.val[ch], &maxval.val[ch] );
    }
    cvSetImageCOI( img, 0 );
    cvSubS( img, maxval, tmp );

    cvExp( tmp, tmp2 );
    sumval = cvSum( tmp2 );
    for( ch = 0; ch < img->nChannels; ch++ )
    {
        sumval.val[ch] = log( sumval.val[ch] ) + maxval.val[ch];
    }
    cvReleaseImage( &tmp );
    cvReleaseImage( &tmp2 );
    __END__;
    return sumval;
}


#endif
