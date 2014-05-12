/** @file
// cvgmmpdf.h (Gaussian Mixture Model)
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
//
// @requirements cvgausspdf.h
*/
#ifndef CV_GMMPDF_INCLUDED
#define CV_GMMPDF_INCLUDED


#include "cv.h"
#include "cvaux.h"
#include <stdio.h>
#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>

#include "cvgausspdf.h"

/**
// cvMatGmmPdf - compute gaussian mixture pdf for a set of sample vectors
//
// Example)
//    const int D = 2;
//    const int N = 3;
//    const int K = 2;
//
//    double vs[] = { 3, 4, 5,
//                    3, 4, 5 }; // col vectors
//    double ms[] = { 3, 5, 
//                    3, 5 }; // col vectors
//    double cs0[] = { 1, 0,
//                     0, 1 };
//    double cs1[] = { 1, 0.1,
//                     0.1, 1 };
//    double ws[] = { 0.5, 0.5 };
//
//    CvMat vecs = cvMat(D, N, CV_64FC1, vs);
//    CvMat means = cvMat(D, K, CV_64FC1, ms);
//    CvMat **covs = (CvMat**)cvAlloc( K * sizeof(*covs) );
//    covs[0] = &cvMat(D, D, CV_64FC1, cs0);
//    covs[1] = &cvMat(D, D, CV_64FC1, cs0);
//    CvMat weights = cvMat( 1, K, CV_64FC1, ws);
//    CvMat *probs = cvCreateMat(K, N, CV_64FC1);
//    cvMatGmmPdf( &vecs, &means, covs, &weights, probs, false);
//    cvMatPrint( probs );
//    cvReleaseMat( &probs );
//    cvFree( &covs );
//
// @param samples   D x N data vector (Note: not N x D for clearness of matrix operation)
// @param means     D x K mean vector
// @param covs      (D x D) x K covariance matrix for each cluster
// @param weights   1 x K weights
// @param probs     K x N or 1 x N computed probabilites
// @param [normalize = false] Compute normalization term or not
// @uses cvMatGaussPdf
*/
void cvMatGmmPdf( const CvMat* samples, const CvMat* means, CvMat** covs, const CvMat* weights, CvMat* probs, bool normalize = false )
{
    int D = samples->rows;
    int N = samples->cols;
    int K = means->cols;
    int type = samples->type;
    CV_FUNCNAME( "cvMatGmmPdf" ); // error handling
    __BEGIN__;
    CV_ASSERT( CV_IS_MAT(samples) );
    CV_ASSERT( CV_IS_MAT(means) );
    for( int k = 0; k < K; k++ )
        CV_ASSERT( CV_IS_MAT(covs[k]) );
    CV_ASSERT( CV_IS_MAT(weights) );
    CV_ASSERT( CV_IS_MAT(probs) );
    CV_ASSERT( D == means->rows ); 
    for( int k = 0; k < K; k++ )
        CV_ASSERT( D == covs[k]->rows && D == covs[k]->cols ); // D x D
    CV_ASSERT( 1 == weights->rows && K == weights->cols ); // 1 x K
    CV_ASSERT( ( 1 == probs->rows || K == probs->rows ) && N == probs->cols ); // 1 x N or K x N

    CvMat *mean = cvCreateMat( D, 1, type );
    const CvMat *cov;
    CvMat *_probs = cvCreateMat( 1, N, type );
    cvZero( probs );
    for( int k = 0; k < K; k++ )
    {
        cvGetCol( means, mean, k );
        cov = covs[k];
        cvMatGaussPdf( samples, mean, cov, _probs, normalize );
        cvConvertScale( _probs, _probs, cvmGet( weights, 0, k ) );
        if( 1 == probs->rows )
        {
            cvAdd( probs, _probs, probs );
        }
        else
        {
            for( int n = 0; n < N; n++ )
            {
                cvmSet( probs, k, n, cvmGet( _probs, 0, n ) );
            }
        }
    }

    cvReleaseMat( &mean );
    cvReleaseMat( &_probs );

    __END__;
}

/**
// cvGmmPdf - compute gaussian mixture pdf
//
// @param sample    D x 1 sample vector
// @param means     D x K mean vector for each cluster
// @param covs      (D x D) x K covariance matrix for each cluster
// @param weights   1 x K weights
// @param probs     K x 1 probabilities for each cluster if want
// @param [normalize = false] use normalization term or not
// @return double prob
// @uses cvMatGmmPdf
*/
double cvGmmPdf( const CvMat* sample, const CvMat* means, CvMat** covs, const CvMat* weights, CvMat* probs = NULL, bool normalize = false )
{
    double prob;
    CvMat* _probs;
    int K = means->cols;
    if( probs ) 
        _probs = probs;
    else
        _probs = cvCreateMat( K, 1, sample->type );

    cvMatGmmPdf( sample, means, covs, weights, _probs, normalize );
    prob = cvSum( _probs ).val[0];

    if( !probs )
        cvReleaseMat( &probs );
    return prob;
}


#endif
