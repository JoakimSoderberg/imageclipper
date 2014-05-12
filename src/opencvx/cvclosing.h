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

CVAPI(void) cvClosing( const CvArr* src, CvArr* dst, IplConvKernel* element = NULL, int iterations = 1 );

/**
// cvClosing - closing morphological operation
//
// closing operation would help to fill disconnected contour
//
// @param src Input Array
// @param dst Output Array
// @param [element = NULL] Kernel shape. see cvErode or cvDilate
// @param [iterations = 1]
// @return void
//
// References)
//  R. Gonzalez, R. Woods, "Digital Image Processing," chapter 9
*/
CVAPI(void) cvClosing( const CvArr* src, CvArr* dst, IplConvKernel* element, int iterations )
{
    cvDilate( dst, dst, element, iterations );
    cvErode( src, dst, element, iterations );
}


#endif
