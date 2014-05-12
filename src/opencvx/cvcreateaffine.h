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
#ifndef CV_CREATEAFFINE_INCLUDED
#define CV_CREATEAFFINE_INCLUDED

#include "cv.h"
#include "cvaux.h"
#include "cxcore.h"
#define _USE_MATH_DEFINES
#include <math.h>

#include "cvrect32f.h"

CVAPI(void) cvCreateAffine( CvMat* affine, 
                            CvRect32f rect = cvRect32f(0,0,1,1,0), 
                            CvPoint2D32f shear = cvPoint2D32f(0,0) );

/**
 * Create an affine transform matrix
 *
 * @param affine    The 2 x 3 CV_32FC1|CV_64FC1 affine matrix to be created
 * @param [rect = cvRect32f(0,0,1,1,0)]
 *                  The translation (x, y) and scaling (width, height) and
 *                  rotation (angle) paramenter in degree
 * @param [shear = cvPoint2D32f(0,0)]
 *                  The shear deformation parameter shx and shy
 * @return void
 * @Book{Hartley2004,
 *    author = "Hartley, R.~I. and Zisserman, A.",
 *    title = "Multiple View Geometry in Computer Vision",
 *    edition = "Second",
 *    year = "2004",
 *    publisher = "Cambridge University Press, ISBN: 0521540518"
 * } 
 */
CVAPI(void) cvCreateAffine( CvMat* affine, CvRect32f rect, CvPoint2D32f shear )
{
    double c, s;
    CvMat *R, *S, *A, hdr;
    CV_FUNCNAME( "cvCreateAffine" );
    __BEGIN__;
    CV_ASSERT( rect.width > 0 && rect.height > 0 );
    CV_ASSERT( affine->rows == 2 && affine->cols == 3 );

    // affine = [ A T ]
    // A = [ a b; c d ]
    // Translation T = [ tx; ty ]
    // (1) A = Rotation * Shear(-phi) * [sx 0; 0 sy] * Shear(phi)
    // (2) A = Rotation * [sx shx; shy sy]
    // Use (2)

    // T = [tx; ty]
    cvmSet( affine, 0, 2, rect.x ); cvmSet( affine, 1, 2, rect.y );

    // R = [cos -sin; sin cos]
    R = cvCreateMat( 2, 2, affine->type );
    /*CvMat* R = cvCreateMat( 2, 3, CV_32FC1 );
      cv2DRotationMatrix( cvPoint2D32f( 0, 0 ), rect.angle, 1.0, R ); */
    c = cos( -M_PI / 180 * rect.angle );
    s = sin( -M_PI / 180 * rect.angle );
    cvmSet( R, 0, 0, c ); cvmSet( R, 0, 1, -s );
    cvmSet( R, 1, 0, s ); cvmSet( R, 1, 1, c );

    // S = [sx shx; shy sy]
    S = cvCreateMat( 2, 2, affine->type );
    cvmSet( S, 0, 0, rect.width ); cvmSet( S, 0, 1, shear.x );
    cvmSet( S, 1, 0, shear.y );    cvmSet( S, 1, 1, rect.height );
    
    // A = R * S
    A = cvGetCols( affine, &hdr, 0, 2 );
    cvMatMul ( R, S, A );

    cvReleaseMat( &R );
    cvReleaseMat( &S );
    __END__;
}


#endif
