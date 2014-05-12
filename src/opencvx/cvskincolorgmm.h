/**
// cvskincolorgmm.h
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
#ifndef CV_SKINCOLOR_GMM_INCLUDED
#define CV_SKINCOLOR_GMM_INCLUDED


#include "cv.h"
#include "cvaux.h"
#define _USE_MATH_DEFINES
#include <math.h>

#include "cvxmat.h"
#include "cvgmmpdf.h"

void cvSkinColorGmm( const IplImage* _img, IplImage* mask, double threshold = 1.0, IplImage* probs = NULL );

/**
// cvSkinColorGMM - Skin Color Detection with GMM model
//
// @param img        Input image
// @param mask       Generated mask image. 1 for skin and 0 for others
// @param threshold  Threshold value for likelihood-ratio test
//     The preferrable threshold = (number of other pixels / number of skin pixels)
//     You may guess this value by looking your image. 
//     You may reduce this number when you can allow larger false alaram rate which
//     results in to reduce reduce miss detection rate.
// @param [probs = NULL] The likelihood-ratio valued array rather than mask if you want
// 
// References)
//  @article{606260,
//      author = {Michael J. Jones and James M. Rehg},
//      title = {Statistical color models with application to skin detection},
//      journal = {Int. J. Comput. Vision},
//      volume = {46},
//      number = {1},
//      year = {2002},
//      issn = {0920-5691},
//      pages = {81--96},
//      doi = {http://dx.doi.org/10.1023/A:1013200319198},
//      publisher = {Kluwer Academic Publishers},
//      address = {Hingham, MA, USA},
//  }
*/
void cvSkinColorGmm( const IplImage* _img, IplImage* mask, double threshold, IplImage* probs )
{
    CV_FUNCNAME( "cvSkinColorGmm" );
    __BEGIN__;
    const int N = _img->height * _img->width;
    const int D = 3;
    const int K = 16;
    IplImage* img;

    double skin_mean[] = {
        73.5300, 249.7100, 161.6800, 186.0700, 189.2600, 247.0000, 150.1000, 206.8500, 212.7800, 234.8700, 151.1900, 120.5200, 192.2000, 214.2900,  99.5700, 238.8800,
        29.9400, 233.9400, 116.2500, 136.6200,  98.3700, 152.2000,  72.6600, 171.0900, 152.8200, 175.4300,  97.7400,  77.5500, 119.6200, 136.0800,  54.3300, 203.0800,
        17.7600, 217.4900,  96.9500, 114.4000,  51.1800,  90.8400,  37.7600, 156.3400, 120.0400, 138.9400,  74.5900,  59.8200,  82.3200,  87.2400,  38.0600, 176.9100 
    };
    double skin_cov[] = { // only diagonal components
        765.4000,  39.9400, 291.0300, 274.9500, 633.1800,  65.2300, 408.6300, 530.0800, 160.5700, 163.8000, 425.4000, 330.4500, 152.7600, 204.9000, 448.1300, 178.3800,
        121.4400, 154.4400,  60.4800,  64.6000, 222.4000, 691.5300, 200.7700, 155.0800,  84.5200, 121.5700,  73.5600,  70.3400,  92.1400, 140.1700,  90.1800, 156.2700,
        112.8000, 396.0500, 162.8500, 198.2700, 250.6900, 609.9200, 257.5700, 572.7900, 243.9000, 279.2200, 175.1100, 151.8200, 259.1500, 270.1900, 151.2900, 404.9900
    };
    double skin_weight[] = {
        0.0294, 0.0331, 0.0654, 0.0756, 0.0554, 0.0314, 0.0454, 0.0469, 0.0956, 0.0763, 0.1100, 0.0676, 0.0755, 0.0500, 0.0667, 0.0749
    };
    double nonskin_mean[] = {
        254.3700, 9.3900,  96.5700, 160.4400,  74.9800, 121.8300, 202.1800, 193.0600,  51.8800,  30.8800,  44.9700, 236.0200, 207.8600,  99.8300, 135.0600, 135.9600,
        254.4100, 8.0900,  96.9500, 162.4900,  63.2300,  60.8800, 154.8800, 201.9300,  57.1400,  26.8400,  85.9600, 236.2700, 191.2000, 148.1100, 131.9200, 103.8900,
        253.8200, 8.5200,  91.5300, 159.0600,  46.3300,  18.3100,  91.0400, 206.5500,  61.5500,  25.3200, 131.9500, 230.7000, 164.1200, 188.1700, 123.1000,  66.8800  
    };
    double nonskin_cov[] = { // only diagonal components
        2.77,  46.84, 280.69, 355.98, 414.84, 2502.2, 957.42, 562.88, 344.11, 222.07, 651.32, 225.03, 494.04, 955.88, 350.35, 806.44,
        2.81,  33.59, 156.79, 115.89, 245.95, 1383.5, 1766.9, 190.23, 191.77, 118.65, 840.52, 117.29, 237.69, 654.95,  130.3,  642.2,
        5.46,  32.48, 436.58, 591.24, 361.27, 237.18, 1582.5, 447.28,  433.4, 182.41, 963.67, 331.95, 533.52,  916.7, 388.43, 350.36
    };
    double nonskin_weight[] = {
        0.0637, 0.0516, 0.0864, 0.0636, 0.0747, 0.0365, 0.0349, 0.0649, 0.0656, 0.1189, 0.0362, 0.0849, 0.0368, 0.0389, 0.0943, 0.0477
    };

    CV_ASSERT( _img->width == mask->width && _img->height == mask->height );
    CV_ASSERT( _img->nChannels >= 3 && mask->nChannels == 1 );
    if( probs )
    {
        CV_ASSERT( _img->width == probs->width && _img->height == probs->height );
        CV_ASSERT( probs->nChannels == 1 );
    }

    img = cvCreateImage( cvGetSize(_img), _img->depth, _img->nChannels );
    cvCvtColor( _img, img, CV_BGR2RGB );

    // transform to CvMat
    CvMat SkinMeans = cvMat( D, K, CV_64FC1, skin_mean );
    CvMat SkinWeights = cvMat( 1, K, CV_64FC1, skin_weight );
    CvMat **SkinCovs = (CvMat**)cvAlloc( K * sizeof( CvMat* ) );
    for( int k = 0; k < K; k++ )
    {
        SkinCovs[k] = cvCreateMat( D, D, CV_64FC1 );
        cvZero( SkinCovs[k] );
        for( int i = 0; i < D; i++ )
        {
            cvmSet( SkinCovs[k], i, i, skin_cov[K * i + k] );
        }
    }

    // transform to CvMat
    CvMat NonSkinMeans = cvMat( D, K, CV_64FC1, nonskin_mean );
    CvMat NonSkinWeights = cvMat( 1, K, CV_64FC1, nonskin_weight );
    CvMat **NonSkinCovs = (CvMat**)cvAlloc( K * sizeof( CvMat* ) );
    for( int k = 0; k < K; k++ )
    {
        NonSkinCovs[k] = cvCreateMat( D, D, CV_64FC1 );
        cvZero( NonSkinCovs[k] );
        for( int i = 0; i < D; i++ )
        {
            cvmSet( NonSkinCovs[k], i, i, nonskin_cov[K * i + k] );
        }
    }

    // reshape IplImage to D (colors) x N matrix of CV_64FC1
    CvMat *Mat = cvCreateMat( D, N, CV_64FC1 );
    CvMat *PreMat, hdr;
    PreMat = cvCreateMat( img->height, img->width, CV_64FC3 );
    cvConvert( img, PreMat ); 
    cvTranspose( cvReshape( PreMat, &hdr, 1, N ), Mat );
    cvReleaseMat( &PreMat );

    // GMM PDF
    CvMat *SkinProbs = cvCreateMat(1, N, CV_64FC1);
    CvMat *NonSkinProbs = cvCreateMat(1, N, CV_64FC1);
    
    cvMatGmmPdf( Mat, &SkinMeans, SkinCovs, &SkinWeights, SkinProbs, true);
    cvMatGmmPdf( Mat, &NonSkinMeans, NonSkinCovs, &NonSkinWeights, NonSkinProbs, true);

    // Likelihood-ratio test
    CvMat *Mask = cvCreateMat( 1, N, CV_8UC1 );
    cvDiv( SkinProbs, NonSkinProbs, SkinProbs );
    cvThreshold( SkinProbs, Mask, threshold, 1, CV_THRESH_BINARY );
    cvConvert( cvReshape( Mask, &hdr, 1, img->height ), mask );
    
    if( probs ) cvConvert( cvReshape( SkinProbs, &hdr, 1, img->height ), probs );

    for( int k = 0; k < K; k++ )
    {
        cvReleaseMat( &SkinCovs[k] );
        cvReleaseMat( &NonSkinCovs[k] );
    }
    cvFree( &SkinCovs );
    cvFree( &NonSkinCovs );

    cvReleaseMat( &SkinProbs );
    cvReleaseMat( &NonSkinProbs );
    cvReleaseMat( &Mask );
    cvReleaseImage( &img );

    __END__;
}


#endif
