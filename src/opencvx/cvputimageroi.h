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
#ifndef CV_CROPIMAGEROI_INCLUDED
#define CV_CROPIMAGEROI_INCLUDED

#include "cv.h"
#include "cvaux.h"
#include "cxcore.h"
#define _USE_MATH_DEFINES
#include <math.h>

#include "cvrect32f.h"
#include "cvcreateaffine.h"
#include "cvcreateaffineimage.h"

CVAPI(void) cvPutImageROI( const IplImage* src,
                           IplImage* dst,
                           CvRect32f rect32f = cvRect32f(0,0,1,1,0),
                           CvPoint2D32f shear = cvPoint2D32f(0,0),
                           const IplImage* mask = NULL,
                           bool circumscribe = 0 );

/**
 * Put a source image on the specified region on a target image 
 *
 * Use CvBox32f to define rotation center as the center of rectangle,
 * and use cvRect32fBox32( box32f ) to pass argument. 
 *
 * @param src          The source image
 * @param dst          The target image
 * @param [rect32f = cvRect32f(0,0,1,1,0)]
 *                     The rectangle region (x,y,width,height) to put
 *                     the rotation angle in degree where the rotation center is (x,y)
 * @param [shear = cvPoint2D32f(0,0)]
 *                     The shear deformation parameter shx and shy
 * @param [mask = NULL] The mask image
 * @param [circumscribe = 0]
 *                     Put a circular (ellipsoidal) image as a circumscribed 
 *                     circle (ellipsoid) rather than a inscribed circle (ellipsoid)
 * @return void
 */
CVAPI(void) cvPutImageROI( const IplImage* src,
                           IplImage* dst, 
                           CvRect32f rect32f, 
                           CvPoint2D32f shear,
                           const IplImage* mask,
                           bool circumscribe )
{
    CvRect rect;
    float tx, ty, sx, sy, angle;
    IplImage* _src = NULL;
    IplImage* _mask = NULL;
    CV_FUNCNAME( "cvPutImageROI" );
    __BEGIN__;
    rect = cvRectFromRect32f( rect32f );
    angle = rect32f.angle;

    CV_ASSERT( rect.width > 0 && rect.height > 0 );
    CV_ASSERT( src->depth == dst->depth );
    CV_ASSERT( src->nChannels == dst->nChannels );
    if( mask != NULL )
        CV_ASSERT( src->width == mask->width && src->height == mask->height );

    if( circumscribe )
    {
        CvBox32f box32f = cvBox32fFromRect32f( rect32f );
        // width and height from center (inscribed ellipsoid's a and b parameters)
        float a = box32f.width / 2.0;
        float b = box32f.height / 2.0;
        // diagonal distance to the corner of rectangle from center
        float d = sqrt( a * a + b * b );
        // get distance to the intersectional point of inscribed ellipsoid 
        // line with rectangle's diagonal line from center
        float cost = a / d;
        float sint = b / d;
        float c = sqrt( pow( a * cost, 2 ) + pow( b * sint, 2 ) );
        // this ratio enables to make circumscribed ellipsoid
        float ratio = d / c;
        box32f.width *= ratio;
        box32f.height *= ratio;
        // ellipsoidal paramter to rectangle parameter
        rect32f = cvRect32fFromBox32f( box32f );
        rect = cvRectFromRect32f( rect32f );
    }

    _src = (IplImage*)src;
    _mask = (IplImage*)mask;
    if( rect.width != src->width && rect.height != src->height )
    {
        _src = cvCreateImage( cvSize( rect.width, rect.height ), src->depth, src->nChannels );
        cvResize( src, _src );
        if( mask != NULL )
        {
            _mask = cvCreateImage( cvSize( rect.width, rect.height ), mask->depth, mask->nChannels );
            cvResize( mask, _mask );
        }
    }

    if( angle == 0 && shear.x == 0 && shear.y == 0 && 
        rect.x >= 0 && rect.y >= 0 && 
        rect.x + rect.width < dst->width && rect.y + rect.height < dst->height )
    {
        cvSetImageROI( dst, rect );
        cvCopy( _src, dst, _mask );
        cvResetImageROI( dst );
    }
    else
    {
        if( _mask == NULL )
        {
            _mask = cvCreateImage( cvGetSize(_src), IPL_DEPTH_8U, 1 );
            cvSet( _mask, cvScalar(1) );
        }

        CvMat* affine = cvCreateMat( 2, 3, CV_32FC1 );
        tx = 0;
        ty = 0;
        sx = rect32f.width / (float)_src->width;
        sy = rect32f.height / (float)_src->height;
        angle = rect32f.angle;
        cvCreateAffine( affine, cvRect32f( tx, ty, sx, sy, angle ), shear );
        
        CvPoint origin;
        IplImage* srctrans = cvCreateAffineImage( _src, affine, CV_AFFINE_FULL, &origin, CV_RGB(0,0,0) );
        IplImage* masktrans  = cvCreateAffineImage( _mask, affine, CV_AFFINE_FULL, NULL, cvScalar(0) );
        for( int xp = 0; xp < srctrans->width; xp++ )
        {
            int x = xp + rect.x + origin.x;
            for( int yp = 0; yp < srctrans->height; yp++ )
            {
                int y = yp + rect.y + origin.y;
                if( x < 0 || x >= dst->width || y < 0 || y >= dst->height ) continue;
                if( CV_IMAGE_ELEM( masktrans, uchar, yp, xp ) == 0 ) continue;
                for( int ch = 0; ch < srctrans->nChannels; ch++ )
                {
                    dst->imageData[dst->widthStep * y + x * dst->nChannels + ch]
                        = srctrans->imageData[srctrans->widthStep * yp + xp * srctrans->nChannels + ch];
                }
            }
        }
        cvReleaseMat( &affine );
        cvReleaseImage( &srctrans );
        cvReleaseImage( &masktrans );
    }
    if( mask != _mask )
        cvReleaseImage( &_mask );
    if( src != _src )
        cvReleaseImage( &_src );
    __END__;
}

#endif
