/**
* OpenCV versus Matlab C Library (MEX) interfaces
*     verified under OpenCV 1.00 and Matlab 2007b
*
* Type conversion
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
#ifndef CVMX_TYPECONV_INCLUDED
#define CVMX_TYPECONV_INCLUDED


#include "cv.h"
#include "mat.h"
#include "matrix.h"
#include "cvipltocvdepth.h"

/************** Definitions *******************************/
#define cvmxCvToIplDepth(type) (cvCvToIplDepth(type))
#define cvmxIplToCvDepth(depth) (cvIplToCvDepth(depth))
CVAPI(int)          cvmxIplToCvDepth(int depth);
CVAPI(mxClassID)    cvmxClassIDFromIplDepth(int depth);
CVAPI(int)          cvmxClassIDToIplDepth(mxClassID classid);
CV_INLINE mxClassID cvmxClassIDFromCvDepth(int type);
CV_INLINE int       cvmxClassIDToCvDepth(mxClassID classid);

/********************* Memo *******************************/
/*
 * @reference cxcore/src/_cxcore.h#icvIplToCvDepth
 * @reference ./cxcore/src/cxtables.cpp
 */
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
/*
  typedef enum {
  mxUNKNOWN_CLASS,
  mxCELL_CLASS,
  mxSTRUCT_CLASS,
  mxLOGICAL_CLASS,
  mxCHAR_CLASS,
  mxDOUBLE_CLASS,
  mxSINGLE_CLASS,
  mxINT8_CLASS,
  mxUINT8_CLASS,
  mxINT16_CLASS,
  mxUINT16_CLASS,
  mxINT32_CLASS,
  mxUINT32_CLASS,
  mxINT64_CLASS,
  mxUINT64_CLASS,
  mxFUNCTION_CLASS
  } mxClassID; */

/************** Functions *********************************/

/**
* Get mxClassID from IplImage depth
* 
* @param int depth IplImage depth
* @return mxClassID
* @see cvmxClassIDFromCvDepth
* @see cvmxClassIDToCvDepth
* @see cvmxClassIDFromIplDepth
* @see cvmxClassIDToIplDepth
* @see cvmxIplToCvDepth
* @see cvCvToIplDepth
*/
CVAPI(mxClassID) cvmxClassIDFromIplDepth(int depth)
{
    static const unsigned char ClassIDFromIplDepth[] =
    {
        0, 0, mxUINT8_CLASS, mxINT8_CLASS, mxUINT16_CLASS, mxINT16_CLASS, 0, 0,
        mxSINGLE_CLASS, mxINT32_CLASS, 0, 0, 0, 0, 0, 0, mxDOUBLE_CLASS, 0
    };
    if (depth == IPL_DEPTH_1U) {
        return mxLOGICAL_CLASS;
    } else {
        return (mxClassID)ClassIDFromIplDepth[(((depth) & 255) >> 2) + ((depth) < 0)];
    }
}

/**
* Convert mxClassID to IplImage depth 
* 
* @param int depth IplImage's depth
* @return mxClassID
* @see cvmxClassIDFromCvDepth
* @see cvmxClassIDToCvDepth
* @see cvmxClassIDFromIplDepth
* @see cvmxClassIDToIplDepth
* @see cvmxIplToCvDepth
* @see cvCvToIplDepth
*/
CVAPI(int) cvmxClassIDToIplDepth(mxClassID classid)
{
    static const signed int ClassIDToIplDepth[] = {
        0,
        0,
        0,
        IPL_DEPTH_1U,
        0,
        IPL_DEPTH_32F,
        IPL_DEPTH_64F,
        IPL_DEPTH_8S,
        IPL_DEPTH_8U,
        IPL_DEPTH_16S,
        IPL_DEPTH_16U,
        IPL_DEPTH_32S,
        0,
        0,
        0,
        0
    };
    return ClassIDToIplDepth[classid];
}

/**
* Get mxClassID from cvMat type
*
* @param int cvMat type
* @return mxClassID
* @see cvmxClassIDFromCvDepth
* @see cvmxClassIDToCvDepth
* @see cvmxClassIDFromIplDepth
* @see cvmxClassIDToIplDepth
* @see cvmxIplToCvDepth
* @see cvCvToIplDepth
*/
CV_INLINE mxClassID cvmxClassIDFromCvDepth(int type)
{
    return cvmxClassIDFromIplDepth(cvCvToIplDepth(type));
}

/**
* Convert mxClassID to cvMat depth
*
* One may use CV_MAKETYPE(depth, nChannel) to create cvMat type
*
* @param mxClassID classid
* @return int cvMat depth
* @see cvmxClassIDFromCvDepth
* @see cvmxClassIDToCvDepth
* @see cvmxClassIDFromIplDepth
* @see cvmxClassIDToIplDepth
* @see cvmxIplToCvDepth
* @see cvCvToIplDepth
*/
CV_INLINE int cvmxClassIDToCvDepth(mxClassID classid)
{
    return cvmxIplToCvDepth(cvmxClassIDToIplDepth(classid));
}


#endif
