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
#ifndef CV_GAUSSPDF_INCLUDED
#define CV_GAUSSPDF_INCLUDED


#include "cv.h"
#include "cvaux.h"
#define _USE_MATH_DEFINES
#include <math.h>

/**
// cvMatGaussPdf - compute multivariate gaussian pdf for a set of sample vectors
//
// Example)
//    double vs[] = { 3, 4, 5,
//                    3, 4, 5 }; // col vectors
//    double m[] = { 5, 
//                   5 };
//    double a[] = { 1, 0,
//                   0, 1 };
//    CvMat vecs = cvMat(2, 3, CV_64FC1, vs);
//    CvMat mean = cvMat(2, 1, CV_64FC1, m);
//    CvMat cov  = cvMat(2, 2, CV_64FC1, a);
//    CvMat *probs = cvCreateMat(1, 3, CV_64FC1);
//    cvMatGaussPdf( &vecs, &mean, &cov, probs, false, false);
//    cvMatPrint( probs ); // 0.018316 0.367879 1.000000
//    cvMatGaussPdf( &vecs, &mean, &cov, probs, true, false);
//    cvMatPrint( probs ); // 0.002915 0.058550 0.159155
//    cvMatGaussPdf( &vecs, &mean, &cov, probs, false, true);
//    cvMatPrint( probs ); // -4.000000 -1.000000 -0.000000
//    cvMatGaussPdf( &vecs, &mean, &cov, probs, true, true);
//    cvMatPrint( probs ); // -5.837877 -2.837877 -1.837877
//    cvReleaseMat( &probs );
//
// @param samples   D x N data vectors where D is the number of
//                  dimensions and N is the number of data
//                  (Note: not N x D for clearness of matrix operation)
// @param mean      D x 1 mean vector
// @param cov       D x D covariance matrix
// @param probs     1 x N computed probabilites
// @param [normalize = false] Compute normalization term or not
// @param [logprob   = false] Log probability or not
// @return void
// @see cvCalcCovarMatrix, cvAvg
*/
void cvMatGaussPdf( const CvMat* samples, const CvMat* mean, const CvMat* cov, CvMat* probs, bool normalize = false, bool logprob = false )
{
    int D = samples->rows;
    int N = samples->cols;
    int type = samples->type;
    CV_FUNCNAME( "cvMatGaussPdf" ); // error handling
    __BEGIN__;
    CV_ASSERT( CV_IS_MAT(samples) );
    CV_ASSERT( CV_IS_MAT(mean) );
    CV_ASSERT( CV_IS_MAT(cov) );
    CV_ASSERT( CV_IS_MAT(probs) );
    CV_ASSERT( D == mean->rows && 1 == mean->cols );
    CV_ASSERT( D == cov->rows && D == cov->cols );
    CV_ASSERT( 1 == probs->rows && N == probs->cols );

    CvMat *invcov = cvCreateMat( D, D, type );
    cvInvert( cov, invcov, CV_SVD );

    CvMat *sample = cvCreateMat( D, 1, type );
    CvMat *subsample   = cvCreateMat( D, 1, type );
    CvMat *subsample_T = cvCreateMat( 1, D, type );
    CvMat *value       = cvCreateMat( 1, 1, type );
    double prob;
    for( int n = 0; n < N; n++ )
    {
        cvGetCol( samples, sample, n );

        cvSub( sample, mean, subsample );
        cvTranspose( subsample, subsample_T );
        cvMatMul( subsample_T, invcov, subsample_T );
        cvMatMul( subsample_T, subsample, value );
        prob = -0.5 * cvmGet(value, 0, 0);
        if( !logprob ) prob = exp( prob );

        cvmSet( probs, 0, n, prob );
    }
    if( normalize )
    {
        double norm = pow( 2* M_PI, D/2.0 ) * sqrt( cvDet( cov ) );
        if( logprob ) cvSubS( probs, cvScalar( log( norm ) ), probs );
        else cvConvertScale( probs, probs, 1.0 / norm );
    }
    
    cvReleaseMat( &invcov );
    cvReleaseMat( &sample );
    cvReleaseMat( &subsample );
    cvReleaseMat( &subsample_T );
    cvReleaseMat( &value );

    __END__;
}

/**
// cvGaussPdf - compute multivariate gaussian pdf
//
// Example)
//    double v[] = { 3, 
//                   3 };
//    double m[] = { 5, 
//                   5 };
//    double a[] = { 1, 0,
//                   0, 1 };
//    CvMat vec  = cvMat(2, 1, CV_64FC1, v);
//    CvMat mean = cvMat(2, 1, CV_64FC1, m);
//    CvMat cov  = cvMat(2, 2, CV_64FC1, a);
//    std::cout << cvGaussPdf( &vec, &mean, &cov, false ) << std::endl;
//
// @param sample    D x 1 data vector
// @param mean      D x 1 mean vector
// @param cov       D x D covariance matrix
// @param [normalize = false] Compute normalization term or not
// @param [logprob   = false] Log probability or not
// @return double   probability
// @see cvCalcCovarMatrix, cvAvg
// @uses cvMatGaussPdf
*/
double cvGaussPdf( const CvMat* sample, const CvMat* mean, const CvMat* cov, bool normalize = false, bool logprob = false )
{
    double prob;
    CvMat *_probs  = cvCreateMat( 1, 1, sample->type );

    cvMatGaussPdf( sample, mean, cov, _probs, normalize, logprob );
    prob = cvmGet(_probs, 0, 0);

    cvReleaseMat( &_probs );
    return prob;
}


#endif
