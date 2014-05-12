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
#ifndef CV_CLOSING_INCLUDED
#define CV_CLOSING_INCLUDED

#include "cv.h"
#include "cvaux.h"
#include "cxcore.h"

CVAPI(void) cvSandwichFill( const IplImage* src, IplImage* dst );

/**
// cvSandwichFill - Search boundary (non-zero pixel) from both side and fill inside
//
// @param IplImage* src One channel image with 0 or 1 value (mask image)
// @param IplImage* dst
// @see cvSmooth( src, dst, CV_MEDIAN, 3 )
// @see cvClosing( src, dst, NULL, 3 )
*/
CVAPI(void) cvSandwichFill( const IplImage* src, IplImage* dst )
{
    cvCopy( src, dst );
    for( int y = 0; y < dst->height; y++ )
    {
        int start = -1;
        int end = -1;
        for( int x = 0; x < dst->width - 1; x++)
        {
            int p1 = dst->imageData[dst->widthStep * y + x];
            int p2 = dst->imageData[dst->widthStep * y + x + 1];
            if( p1 > 0 && p2 > 0 )
            {
                start = x;
                break;
            }
        }
        for( int x = dst->width - 1; x > start; x--)
        {
            int p1 = dst->imageData[dst->widthStep * y + x];
            int p2 = dst->imageData[dst->widthStep * y + x + 1];
            if( p1 > 0 && p2 > 0 )
            {
                end = x;
                break;
            }
        }
        if( start != -1 && end != -1 )
        {
            for( int x = start; x <= end; x++)
            {
                dst->imageData[dst->widthStep * y + x] = 1;
            }
        }
    }
    for( int x = 0; x < dst->width; x++ )
    {
        int start = -1;
        int end = -1;
        for( int y = 0; y < dst->height - 1; y++)
        {
            int p1 = dst->imageData[dst->widthStep * y + x];
            int p2 = dst->imageData[dst->widthStep * y + x + 1];
            if( p1 > 0 && p2 > 0 )
            {
                start = y;
                break;
            }
        }
        for( int y = dst->height - 1; y > start; y--)
        {
            int p1 = dst->imageData[dst->widthStep * y + x];
            int p2 = dst->imageData[dst->widthStep * y + x + 1];
            if( p1 > 0 && p2 > 0 )
            {
                end = y;
                break;
            }
        }
        if( start != -1 && end != -1 )
        {
            for( int y = start; y <= end; y++)
            {
                dst->imageData[dst->widthStep * y + x] = 1;
            }
        }
    }
    //// Tried to use cvFindContours, but did not work for disconnected contours.
    //CvMemStorage* storage = cvCreateMemStorage(0);
    //CvSeq* contour = 0;
    //cvFindContours( dst, storage, &contour, 
    //    sizeof(CvContour), CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE );
    //for( ; contour != 0; contour = contour->h_next )
    //{
    //    CvScalar color = cvScalar(255);
    //    cvDrawContours( dst, contour, color, color, -1, CV_FILLED, 8 );
    //}
}


#endif
