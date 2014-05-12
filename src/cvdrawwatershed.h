/** @file
* 
* The MIT License
* 
* Copyright (c) 2008, Naotoshi Seo <sonots(at)umd.edu>
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

#ifndef CV_DRAWWATERSHED_INCLUDED
#define CV_DRAWWATERSHED_INCLUDED

#include <stdio.h>
#include <string>

// marker's shape is like circle
// just for imageclipper.cpp for now
CvRect cvDrawWatershed( IplImage* img, const CvRect circle )
{
    IplImage* markers  = cvCreateImage( cvGetSize( img ), IPL_DEPTH_32S, 1 );
    CvPoint center = cvPoint( circle.x, circle.y );
    int radius = circle.width;

    // Set watershed markers. Now, marker's shape is like circle
    // Set (1 * radius) - (3 * radius) region as ambiguous region (0), intuitively
    cvSet( markers, cvScalarAll( 1 ) );
    cvCircle( markers, center, 3 * radius, cvScalarAll( 0 ), CV_FILLED, 8, 0 );
    cvCircle( markers, center, radius, cvScalarAll( 2 ), CV_FILLED, 8, 0 );
    cvWatershed( img, markers );

    // Draw watershed markers and rectangle surrounding watershed markers
    cvCircle( img, center, radius, cvScalarAll (255), 2, 8, 0);

    CvPoint minpoint = cvPoint( markers->width, markers->height );
    CvPoint maxpoint = cvPoint( 0, 0 );
    for (int y = 1; y < markers->height-1; y++) { // looks outer boundary is always -1. 
        for (int x = 1; x < markers->width-1; x++) {
            int* idx = (int *) cvPtr2D (markers, y, x, NULL);
            if (*idx == -1) { // watershed marker -1
                cvSet2D (img, y, x, cvScalarAll (255));
                if( x < minpoint.x ) minpoint.x = x;
                if( y < minpoint.y ) minpoint.y = y;
                if( x > maxpoint.x ) maxpoint.x = x;
                if( y > maxpoint.y ) maxpoint.y = y;
            }
        }
    }
    return cvRect( minpoint.x, minpoint.y, maxpoint.x - minpoint.x, maxpoint.y - minpoint.y );
}

inline CvRect cvShowImageAndWatershed( const char* w_name, const IplImage* img, const CvRect &circle )
{
    IplImage* clone = cvCloneImage( img );
    CvRect rect = cvDrawWatershed( clone, circle );
    cvRectangle( clone, cvPoint( rect.x, rect.y ), cvPoint( rect.x + rect.width, rect.y + rect.height ), CV_RGB(255, 255, 0), 1 );
    cvShowImage( w_name, clone );
    cvReleaseImage( &clone );
    return rect;
}

#endif
