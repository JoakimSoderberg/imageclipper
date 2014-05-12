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

#include "cvcreateaffine.h"
#include "cvrect32f.h"

CVAPI(void) cvCropImageROI( IplImage* img, IplImage* dst, 
                            CvRect32f rect32f = cvRect32f(0,0,1,1,0),
                            CvPoint2D32f shear = cvPoint2D32f(0,0) );
CVAPI(void) cvShowCroppedImage( const char* w_name, IplImage* orig, 
                            CvRect32f rect32f = cvRect32f(0,0,1,1,0),
                            CvPoint2D32f shear = cvPoint2D32f(0,0) );

/**
 * Crop image with rotated and sheared rectangle
 *
 * IplImage* dst = cvCreateImage( cvSize( rect.width, rect.height ), img->depth, img->nChannels );
 * Use CvBox32f to define rotation center as the center of rectangle,
 * and use cvRect32fBox32( box32f ) to pass argument. 
 *
 * @param img          The target image
 * @param dst          The cropped image
 * @param [rect32f = cvRect32f(0,0,1,1,0)]
 *                     The rectangle region (x,y,width,height) to crop and 
 *                     the rotation angle in degree where the rotation center is (x,y)
 * @param [shear = cvPoint2D32f(0,0)]
 *                     The shear deformation parameter shx and shy
 * @return void
 */
CVAPI(void) cvCropImageROI( IplImage* img, IplImage* dst, CvRect32f rect32f, CvPoint2D32f shear )
{
    CvRect rect = cvRectFromRect32f( rect32f );
    float angle = rect32f.angle;
    CV_FUNCNAME( "cvCropImageROI" );
    __BEGIN__;
    CV_ASSERT( rect.width > 0 && rect.height > 0 );
    CV_ASSERT( dst->width == rect.width );
    CV_ASSERT( dst->height == rect.height );

    if( angle == 0 && shear.x == 0 && shear.y == 0 && 
        rect.x >= 0 && rect.y >= 0 && 
        rect.x + rect.width < img->width && rect.y + rect.height < img->height )
    {
        cvSetImageROI( img, rect );
        cvCopy( img, dst );
        cvResetImageROI( img );                
    }
    else if( shear.x == 0 && shear.y == 0 )
    {
        int x, y, ch, xp, yp;
        double c = cos( -M_PI / 180 * angle );
        double s = sin( -M_PI / 180 * angle );
        /*CvMat* R = cvCreateMat( 2, 3, CV_32FC1 );
        cv2DRotationMatrix( cvPoint2D32f( 0, 0 ), angle, 1.0, R );
        double c = cvmGet( R, 0, 0 );
        double s = cvmGet( R, 1, 0 );
        cvReleaseMat( &R );*/
        cvZero( dst );

        for( x = 0; x < rect.width; x++ )
        {
            for( y = 0; y < rect.height; y++ )
            {
                xp = (int)( c * x + -s * y + 0.5 ) + rect.x;
                yp = (int)( s * x + c * y + 0.5 ) + rect.y;
                if( xp < 0 || xp >= img->width || yp < 0 || yp >= img->height ) continue;
                for( ch = 0; ch < img->nChannels; ch++ )
                {
                    dst->imageData[dst->widthStep * y + x * dst->nChannels + ch]
                        = img->imageData[img->widthStep * yp + xp * img->nChannels + ch];
                }
            }
        }
    }
    else
    {
        int x, y, ch, xp, yp;
        CvMat* affine = cvCreateMat( 2, 3, CV_32FC1 );
        CvMat* xy     = cvCreateMat( 3, 1, CV_32FC1 );
        CvMat* xyp    = cvCreateMat( 2, 1, CV_32FC1 );
        cvCreateAffine( affine, rect32f, shear );
        cvmSet( xy, 2, 0, 1.0 );
        cvZero( dst );

        for( x = 0; x < rect.width; x++ )
        {
            cvmSet( xy, 0, 0, x / rect32f.width );
            for( y = 0; y < rect.height; y++ )
            {
                cvmSet( xy, 1, 0, y / rect32f.height );
                cvMatMul( affine, xy, xyp );
                xp = (int)( cvmGet( xyp, 0, 0 ) + 0.5 );
                yp = (int)( cvmGet( xyp, 1, 0 ) + 0.5 );
                if( xp < 0 || xp >= img->width || yp < 0 || yp >= img->height ) continue;
                for( ch = 0; ch < img->nChannels; ch++ )
                {
                    dst->imageData[dst->widthStep * y + x * dst->nChannels + ch]
                        = img->imageData[img->widthStep * yp + xp * img->nChannels + ch];
                }
            }
        }
        cvReleaseMat( &affine );
        cvReleaseMat( &xy );
        cvReleaseMat( &xyp );
    }
    __END__;
}

/**
 * Crop and show the Cropped Image
 *
 * @param w_name       Window name
 * @param img          Image to be cropped
 * @param [rect32f = cvRect32f(0,0,1,1,0)]
 *                     The rectangle region (x,y,width,height) to crop and 
 *                     the rotation angle in degree
 * @param [shear = cvPoint2D32f(0,0)]
 *                     The shear deformation parameter shx and shy
 * @return void
 * @uses cvCropImageROI
 */
CVAPI(void) cvShowCroppedImage( const char* w_name, IplImage* img, CvRect32f rect32f, CvPoint2D32f shear )
{
    CvRect rect = cvRectFromRect32f( rect32f );
    if( rect.width <= 0 || rect.height <= 0 ) return;
    IplImage* crop = cvCreateImage( cvSize( rect.width, rect.height ), img->depth, img->nChannels );
    cvCropImageROI( img, crop, rect32f, shear );
    cvShowImage( w_name, crop );
    cvReleaseImage( &crop );
}


#endif
