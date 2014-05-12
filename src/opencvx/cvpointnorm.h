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
#ifndef CV_POINTNORM_INCLUDED
#define CV_POINTNORM_INCLUDED

#include "cv.h"
#include "cvaux.h"
#include "cxcore.h"
#include <math.h>
using namespace std;

CV_INLINE double cvPointNorm( CvPoint p1, CvPoint p2, int norm_type = CV_L2 );

/**
// Compute Norm between two points
//
// @param CvPoint p1              A point 1
// @param CVPoint p2              A point 2
// @param int [norm_type = CV_L2] CV_L2 to compute L2 norm (euclidean distance)
//                                CV_L1 to compute L1 norm (abs)
// @return double
*/
CV_INLINE double cvPointNorm( CvPoint p1, CvPoint p2, int norm_type )
{
    if( norm_type == CV_L1 )
        return abs( p2.x - p1.x ) + abs( p2.y - p1.y );
    else
        return sqrt( pow( (double)p2.x - p1.x, 2 ) + pow( (double)p2.y - p1.y, 2 ) );
}


#endif
