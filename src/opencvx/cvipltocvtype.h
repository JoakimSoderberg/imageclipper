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

CV_INLINE int cvIplToCvType(int ipl_depth, int nChannels);

/**
 * Convert IplImage depth and nChannels to CvMat type
 *
 * @param depth     IplImage depth. img->depth
 * @param nChannels Number of channels. img->nChannels
 * @return int      CvMat type. mat->type
 */
CV_INLINE int cvIplToCvType(int ipl_depth, int nChannels)
{
    int cvmat_depth = cvIplToCvDepth( ipl_depth );
    return CV_MAKETYPE( cvmat_depth, nChannels );
}


#endif
