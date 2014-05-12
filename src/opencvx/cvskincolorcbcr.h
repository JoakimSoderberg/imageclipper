/**
// cvskincolorcbcr.h
//
// Copyright (c) 2008, Naotoshi Seo. All rights reserved.
//
// The program is free to use for non-commercial academic purposes,
// but for course works, you must understand what is going inside to 
// use. The program can be used, modified, or re-distributed for any 
// purposes only if you or one of your group understand not only 
// programming codes but also theory and math behind (if any). 
// Please contact the authors if you are interested in using the 
// program without meeting the above conditions.
*/
#ifndef CV_SKINCOLOR_CBCR_INCLUDED
#define CV_SKINCOLOR_CBCR_INCLUDED


#include "cv.h"
#include "cvaux.h"
#define _USE_MATH_DEFINES
#include <math.h>

void cvSkinColorCrCb( const IplImage* _img, IplImage* mask, CvArr* distarr = NULL );

/**
// cvSkinColorCbCr - Skin Color Detection in (Cb, Cr) space by [1][2]
//
// @param img  Input image
// @param mask Generated mask image. 1 for skin and 0 for others
// @param [dist = NULL] The distortion valued array rather than mask if you want
// 
// References)
//  [1] R.L. Hsu, M. Abdel-Mottaleb, A.K. Jain, "Face Detection in Color Images," 
//  IEEE Transactions on Pattern Analysis and Machine Intelligence ,vol. 24, no. 5,  
//  pp. 696-706, May, 2002. (Original)
//  [2] P. Peer, J. Kovac, J. and F. Solina, ”Human skin colour
//  clustering for face detection”, In: submitted to EUROCON –
//  International Conference on Computer as a Tool , 2003. (Tuned)
*/
void cvSkinColorCrCb( const IplImage* _img, IplImage* mask, CvArr* distarr )
{
    CV_FUNCNAME( "cvSkinColorCbCr" );
    __BEGIN__;
    int width  = _img->width;
    int height = _img->height;
    CvMat* dist = (CvMat*)distarr, diststub;
    int coi = 0;
    IplImage* img;

    double Wcb = 46.97;
    double WLcb = 23;
    double WHcb = 14;
    double Wcr = 38.76;
    double WLcr = 20;
    double WHcr = 10;
    double Kl = 125;
    double Kh = 188;
    double Ymin = 16;
    double Ymax = 235;
    double alpha = 0.56;

    double Cx = 109.38;
    double Cy = 152.02;
    double theta = 2.53; 
    double ecx = 1.6;
    double ecy = 2.41;
    double a = 25.39;
    double b = 14.03;

    CV_ASSERT( width == mask->width && height == mask->height );
    CV_ASSERT( _img->nChannels >= 3 && mask->nChannels == 1 );

    if( dist && !CV_IS_MAT(dist) )
    {
        CV_CALL( dist = cvGetMat( dist, &diststub, &coi ) );
        if (coi != 0) CV_ERROR_FROM_CODE(CV_BadCOI);
        CV_ASSERT( width == dist->cols && height == dist->rows );
        CV_ASSERT( CV_MAT_TYPE(dist->type) == CV_32FC1 || CV_MAT_TYPE(dist->type) == CV_64FC1 );
    }

    img = cvCreateImage( cvGetSize(_img), IPL_DEPTH_8U, 3 );
    cvCvtColor( _img, img, CV_BGR2YCrCb );        

    cvSet( mask, cvScalarAll(0) );
    for( int row = 0; row < height; row++ )
    {
        for( int col = 0; col < width; col++ )
        {
            uchar Y  = img->imageData[img->widthStep * row + col * 3];
            uchar Cr = img->imageData[img->widthStep * row + col * 3 + 1];
            uchar Cb = img->imageData[img->widthStep * row + col * 3 + 2];

            double Cb_Y, Cr_Y, Wcb_Y, Wcr_Y;
            if( Y < Kl )
                Wcb_Y = WLcb + (Y - Ymin) * (Wcb - WLcb) / (Kl - Ymin);
            else if( Kh < Y )
                Wcb_Y = WHcb + (Ymax - Y) * (Wcb - WHcb) / (Ymax - Kh);
            else
                Wcb_Y = Wcb;

            if( Y < Kl )
                Wcr_Y = WLcr + (Y - Ymin) * (Wcr - WLcr) / (Kl - Ymin);
            else if( Kh < Y )
                Wcr_Y = WHcr + (Ymax - Y) * (Wcr - WHcr) / (Ymax - Kh);
            else
                Wcr_Y = Wcr;

            if( Y < Kl )
                Cb_Y = 108 + (Kl - Y) * 10 / ( Kl - Ymin );
            else if( Kh < Y )
                Cb_Y = 108 + (Y - Kh) * 10 / ( Ymax - Kh );
            else
                Cb_Y = 108;

            if( Y < Kl )
                Cr_Y = 154 - (Kl - Y) * 10 / ( Kl - Ymin );
            else if( Kh < Y )
                Cr_Y = 154 + (Y - Kh) * 22 / ( Ymax - Kh );
            else
                Cr_Y = 108;

            double x = cos(theta) * (Cb - Cx) + sin(theta) * (Cr - Cy);
            double y = -1 * sin(theta) * (Cb - Cx) + cos(theta) * (Cr - Cy);

            double distort = pow(x-ecx,2) / pow(a,2) + pow(y-ecy,2) / pow(b,2);
            if( dist )
                cvmSet( dist, row, col, distort );

            if( distort <= 1 )
            {
                mask->imageData[mask->widthStep * row + col] = 1;
            }
        }
    }
    __END__;
}


#endif
