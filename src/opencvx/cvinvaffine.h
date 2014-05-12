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
#ifndef CV_INVAFFINE_INCLUDED
#define CV_INVAFFINE_INCLUDED

#include "cv.h"
#include "cvaux.h"
#include "cxcore.h"

#include "cvsetrow.h"

CVAPI(void) cvInvAffine( const CvMat* affine, CvMat* invaffine );

/**
 * Create an inv affine transform matrix from an affine transform matrix
 *
 * @param affine    The 2 x 3 CV_32FC1|CV_64FC1 affine matrix
 * @param invaffine The 2 x 3 CV_32FC1|CV_64FC1 affine matrix to be created
 */
CVAPI(void) cvInvAffine( const CvMat* affine, CvMat* invaffine )
{
    CV_FUNCNAME( "cvCreateAffine" );
    __BEGIN__;
    CV_ASSERT( affine->rows == 2 && affine->cols == 3 );
    CV_ASSERT( invaffine->rows == 2 && invaffine->cols == 3 );
    CV_ASSERT( affine->type == invaffine->type );
    
    CvMat* Affine = cvCreateMat( 3, 3, affine->type );
    CvMat* InvAffine = cvCreateMat( 3, 3, affine->type );
    CvMat invaffinehdr;
    cvSetIdentity( Affine );
    cvSetRows( affine, Affine, 0, 2 );
    cvInv( Affine, InvAffine );
    cvSetRows( InvAffine, invaffine, 0, 2 );
    cvReleaseMat( &Affine );
    cvReleaseMat( &InvAffine );
    __END__;
}

#endif
