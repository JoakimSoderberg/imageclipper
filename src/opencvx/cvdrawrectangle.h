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
#ifndef CV_DRAWRECTANGLE_INCLUDED
#define CV_DRAWRECTANGLE_INCLUDED

#include "cv.h"
#include "cvaux.h"
#include "cxcore.h"
#define _USE_MATH_DEFINES
#include <math.h>

#include "cvcreateaffine.h"
#include "cvrect32f.h"

CVAPI(void) cvDrawRectangle( IplImage* img, 
                             CvRect32f rect32f = cvRect32f(0,0,1,1,0),
                             CvPoint2D32f shear = cvPoint2D32f(0,0), 
                             CvScalar color = CV_RGB(255, 255, 255), 
                             int thickness = 1, 
                             int line_type = 8,
                             int shift = 0);
CVAPI(void) cvShowImageAndRectangle( const char* w_name, const IplImage* img, 
                                     CvRect32f rect32f = cvRect32f(0,0,1,1,0),
                                     CvPoint2D32f shear = cvPoint2D32f(0,0),
                                     CvScalar color = CV_RGB(255, 255, 0), 
                                     int thickness = 1, int line_type = 8, 
                                     int shift = 0);

/**
 * Draw an rotated and sheared rectangle
 *
 * Use CvBox32f to define rotation center as the center of rectangle,
 * and use cvRect32fBox32( box32f ) to pass argument. 
 *
 * @param img             The image to be drawn rectangle
 * @param [rect32f = cvRect32f(0,0,1,1,0)]
 *                        The rectangle (x,y,width,height) to be shown and
 *                        the rotation angle in degree where the rotation center is (x,y)
 * @param [shear = cvPoint2D32f(0,0)]
 *                        The shear deformation parameter shx and shy
 * @param [color  = CV_RGB(255, 255, 0)] 
 *                        Line color (RGB) or brightness (grayscale image). 
 * @param [thickness = 1] Thickness of lines that make up the rectangle. 
 *                        Negative values, e.g. CV_FILLED, make the function 
 *                        to draw a filled rectangle. 
 * @param [line_type = 8] Type of the line, see cvLine description. 
 * @param [shift = 0]     Number of fractional bits in the point coordinates. 
 * @todo thickness, line_type, and shift are available only when rotate == 0 && shear == 0 currently. 
 * @return void
 * @uses cvRectangle
 */
CVAPI(void) cvDrawRectangle( IplImage* img, 
                             CvRect32f rect32f,
                             CvPoint2D32f shear,
                             CvScalar color,
                             int thickness,
                             int line_type,
                             int shift )
{
    CvRect rect = cvRectFromRect32f( rect32f );
    float angle = rect32f.angle;
    CV_FUNCNAME( "cvDrawRectangle" );
    __BEGIN__;
    CV_ASSERT( rect.width > 0 && rect.height > 0 );

    if( angle == 0 && shear.x == 0 && shear.y == 0 )
    {
        CvPoint pt1 = cvPoint( rect.x, rect.y );
        CvPoint pt2 = cvPoint( rect.x + rect.width - 1, rect.y + rect.height - 1 );
        cvRectangle( img, pt1, pt2, color, thickness, line_type, shift );
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

        for( x = 0; x < rect.width; x++ )
        {
            for( y = 0; y < rect.height; y += max(1, rect.height - 1) )
            {
                xp = (int)( c * x + -s * y + 0.5 ) + rect.x;
                yp = (int)( s * x + c * y + 0.5 ) + rect.y;
                if( xp < 0 || xp >= img->width || yp < 0 || yp >= img->height ) continue;
                for( ch = 0; ch < img->nChannels; ch++ )
                {
                    img->imageData[img->widthStep * yp + xp * img->nChannels + ch] = (char)color.val[ch];
                }
            }
        }

        for( y = 0; y < rect.height; y++ )
        {
            for( x = 0; x < rect.width; x += max( 1, rect.width - 1) )
            {
                xp = (int)( c * x + -s * y + 0.5 ) + rect.x;
                yp = (int)( s * x + c * y + 0.5 ) + rect.y;
                if( xp < 0 || xp >= img->width || yp < 0 || yp >= img->height ) continue;
                for( ch = 0; ch < img->nChannels; ch++ )
                {
                    img->imageData[img->widthStep * yp + xp * img->nChannels + ch] = (char)color.val[ch];
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
        cvmSet( xy, 2, 0, 1.0 );
        cvCreateAffine( affine, rect32f, shear );

        for( x = 0; x < rect.width; x++ )
        {
            cvmSet( xy, 0, 0, x / rect32f.width );
            for( y = 0; y < rect.height; y += max(1, rect.height - 1) )
            {
                cvmSet( xy, 1, 0, y / rect32f.height );
                cvMatMul( affine, xy, xyp );
                xp = (int)( cvmGet( xyp, 0, 0 ) + 0.5 );
                yp = (int)( cvmGet( xyp, 1, 0 ) + 0.5 );
                if( xp < 0 || xp >= img->width || yp < 0 || yp >= img->height ) continue;
                for( ch = 0; ch < img->nChannels; ch++ )
                {
                    img->imageData[img->widthStep * yp + xp * img->nChannels + ch] = (char)color.val[ch];
                }
            }
        }
        for( y = 0; y < rect.height; y++ )
        {
            cvmSet( xy, 1, 0, y / rect32f.height );
            for( x = 0; x < rect.width; x += max( 1, rect.width - 1) )
            {
                cvmSet( xy, 0, 0, x / rect32f.width );
                cvMatMul( affine, xy, xyp );
                xp = (int)( cvmGet( xyp, 0, 0 ) + 0.5 );
                yp = (int)( cvmGet( xyp, 1, 0 ) + 0.5 );
                if( xp < 0 || xp >= img->width || yp < 0 || yp >= img->height ) continue;
                for( ch = 0; ch < img->nChannels; ch++ )
                {
                    img->imageData[img->widthStep * yp + xp * img->nChannels + ch] = (char)color.val[ch];
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
 * Show Image and Rectangle
 *
 * @param w_name          Window name
 * @param img             Image to be shown
 * @param [rect32f = cvRect32f(0,0,1,1,0)]
 *                        Rectangle to be shown and
 *                        Rotation degree of rectangle
 * @param [shear = cvPoint2D32f(0,0)]
 *                        The shear deformation parameter shx and shy
 * @param [color  = CV_RGB(255, 255, 0)] 
 *                        Line color (RGB) or brightness (grayscale image). 
 * @param [thickness = 1] Thickness of lines that make up the rectangle. 
 *                        Negative values, e.g. CV_FILLED, make the function 
 *                        to draw a filled rectangle. 
 * @param [line_type = 8] Type of the line, see cvLine description. 
 * @param [shift = 0]     Number of fractional bits in the point coordinates. 
 * @todo thickness, line_type, and shift are available only when angle == 0 && shear == 0 currently. 
 * @return void
 * @uses cvDrawRectangle
 */
CVAPI(void) cvShowImageAndRectangle( const char* w_name, 
                                     const IplImage* img,
                                     CvRect32f rect32f,
                                     CvPoint2D32f shear,
                                     CvScalar color, int thickness, int line_type, int shift)
{
    CvRect rect  = cvRectFromRect32f( rect32f );
    if( rect.width <= 0 || rect.height <= 0 ) { cvShowImage( w_name, img ); return; }
    IplImage* clone = cvCloneImage( img );
    cvDrawRectangle( clone, rect32f, shear, color, thickness, line_type, shift );
    cvShowImage( w_name, clone );
    cvReleaseImage( &clone );
}


#endif
