/**
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
#ifndef CV_IPLTOCVDEPTH_INCLUDED
#define CV_IPLTOCVDEPTH_INCLUDED


#include "cv.h"

CVAPI(int) cvIplToCvDepth(int depth);

/**
 * Convert IplImage depth to CvMat depth
 *
 * Below codes would be useful as a reference:
 * <code>
 * int depth = CV_MAT_DEPTH(mat->type);
 * int nChannels = CV_MAT_CN(mat->type);
 * int type  = CV_MAKETYPE(depth, nChannels);
 * </code>
 *
 * @param int IplImage depth
 * @return int cvMat depth
 * @reference cxcore/src/_cxcore.h#icvIplToCvDepth
 * @reference ./cxcore/src/cxtables.cpp
 * @see cvCvToIplDepth(mat->type)
 */
CVAPI(int) cvIplToCvDepth(int depth)
{
    /*
    #define CV_8U   0
    #define CV_8S   1
    #define CV_16U  2
    #define CV_16S  3
    #define CV_32S  4
    #define CV_32F  5
    #define CV_64F  6
    #define CV_USRTYPE1 7 */
    /*
    #define IPL_DEPTH_SIGN 0x80000000
    #define IPL_DEPTH_1U     1
    #define IPL_DEPTH_8U     8
    #define IPL_DEPTH_16U   16
    #define IPL_DEPTH_32F   32
    #define IPL_DEPTH_64F   64
    #define IPL_DEPTH_8S  (IPL_DEPTH_SIGN| 8)
    #define IPL_DEPTH_16S (IPL_DEPTH_SIGN|16)
    #define IPL_DEPTH_32S (IPL_DEPTH_SIGN|32) */
    static const signed char IplToCvDepth[] =
    {
        -1, -1, CV_8U, CV_8S, CV_16U, CV_16S, -1, -1,
        CV_32F, CV_32S, -1, -1, -1, -1, -1, -1, CV_64F, -1
    };
    return IplToCvDepth[(((depth) & 255) >> 2) + ((depth) < 0)];
}


#endif
