/**
// cvskincolorgauss.h
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
#ifndef CV_SKINCOLOR_GAUSS_INCLUDED
#define CV_SKINCOLOR_GAUSS_INCLUDED


#include "cv.h"
#include "cvaux.h"
#define _USE_MATH_DEFINES
#include <math.h>

void cvSkinColorGauss( const IplImage* _img, IplImage* mask, double factor = 2.5 );

/**
// cvSkinColorGauss - Skin Color Detection with a Gaussian model
//
// @param img    Input image
// @param mask   Generated mask image. 1 for skin and 0 for others
// @param [factor = 2.5] A factor to determine threshold value.
//     The default threshold is -2.5 * sigma and 2.5 * sigma which
//     supports more than 95% region of Gaussian PDF. 
// 
// References)
//  [1] @INPROCEEDINGS{Yang98skin-colormodeling,
//     author = {Jie Yang and Weier Lu and Alex Waibel},
//     title = {Skin-color modeling and adaptation},
//     booktitle = {},
//     year = {1998},
//     pages = {687--694}
//  }
//  [2] C. Stauffer, and W. E. L. Grimson, “Adaptive
//  background mixture models for real-time tracking”, In:
//  Proceedings 1999 IEEE Computer Society Conference
//  on Computer Vision and Pattern Recognition (Cat. No
//  PR00149), IEEE Comput. Soc. Part vol. 2, 1999.
*/
void cvSkinColorGauss( const IplImage* _img, IplImage* mask, double factor )
{
    double mean[] = { 188.9069, 142.9157, 115.1863 };
    double sigma[] = { 58.3542, 45.3306, 43.397 };
    double subdata[3];

    IplImage* img = cvCreateImage( cvGetSize(_img), _img->depth, _img->nChannels );
    cvCvtColor( _img, img, CV_BGR2RGB );
    CvMat* mat = cvCreateMat( img->height, img->width, CV_64FC3 );
    cvConvert( img, mat );

    for( int i = 0; i < mat->rows; i++ )
    {
        for( int j = 0; j < mat->cols; j++ )
        {
            bool skin;
            CvScalar data = cvGet2D( mat, i, j );
            subdata[0] = data.val[0] - mean[0];
            subdata[1] = data.val[1] - mean[1];
            subdata[2] = data.val[2] - mean[2];
            //if( CV_SKINCOLOR_GAUSS_CUBE ) // cube-like judgement
            //{
                skin = 1;
                // 2 * sigma => 95% confidence region, 2.5 gives more
                skin &= ( - factor *sigma[0] < subdata[0] && subdata[0] <  factor *sigma[0] );
                skin &= ( - factor *sigma[1] < subdata[1] && subdata[1] <  factor *sigma[1] );
                skin &= ( - factor *sigma[2] < subdata[2] && subdata[2] <  factor *sigma[2] );
            //}
            //else if( CV_SKINCOLOR_GAUSS_ELLIPSOID ) // ellipsoid-like judgement
            //{
            //    double val = 0;
            //    val += pow( subdata[0] / sigma[0] , 2);
            //    val += pow( subdata[1] / sigma[1] , 2);
            //    val += pow( subdata[2] / sigma[2] , 2);
            //    skin = ( val <= pow(  factor , 2 ) );
            //}
            //else if( CV_SKINCOLOR_GAUSS_LIKELIHOODRATIO ) // likelihood-ratio test (need data for non-skin gaussain model)
            //{
            //}
            mask->imageData[mask->widthStep * i + j] = skin;
        }
    }
    cvReleaseMat( &mat );
    cvReleaseImage( &img );
}


#endif
