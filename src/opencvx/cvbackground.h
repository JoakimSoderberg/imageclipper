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
#ifndef CV_BACKGROUND_INCLUDED
#define CV_BACKGROUND_INCLUDED

#include "cv.h"
#include "cvaux.h"
#include "cxcore.h"

CVAPI(void) cvBackground( const IplImage* _img, const IplImage* _ref, IplImage* _mask, int thresh = 100 );

/**
// Obtain non-background pixels using reference image (such as previous frame in video )
//
// @param mg     The target image
// @param ref    The reference image. Usually the previous frame of video
// @param mask   The generated mask image where 0 is for bg and 1 is for non-bg. Must be 8U and 1 channel
// @param [thresh = 100] The threshold. [0 - 255^2] for single channel image. [0 - 255^2 * 3] for 3 channel image.
// @return void
*/
CVAPI(void) cvBackground( const IplImage* _img, const IplImage* _ref, IplImage* _mask, int thresh )
{
    CV_FUNCNAME( "cvBackground" ); // error handling
    __BEGIN__;
    CV_ASSERT( _img->width == _ref->width );
    CV_ASSERT( _img->width == _mask->width );
    CV_ASSERT( _img->height == _ref->height );
    CV_ASSERT( _img->height == _ref->height );
    CV_ASSERT( _img->nChannels == _ref->nChannels );
    CV_ASSERT( _mask->nChannels == 1 );
    CV_ASSERT( _mask->depth == IPL_DEPTH_8U );

    IplImage *img = cvCreateImage( cvGetSize(_img), IPL_DEPTH_32F, _img->nChannels );
    IplImage *ref = cvCreateImage( cvGetSize(_img), IPL_DEPTH_32F, _img->nChannels );
    IplImage *mask = cvCreateImage( cvGetSize(_img), IPL_DEPTH_32F, 1 );
    cvConvert( _img, img );
    cvConvert( _ref, ref );
    cvSub( img, ref, img );
    cvMul( img, img, img ); // square

    if( img->nChannels > 1 )
    {        
        IplImage* imgB = cvCreateImage( cvGetSize(img), img->depth, 1 );
        IplImage* imgG = cvCreateImage( cvGetSize(img), img->depth, 1 );
        IplImage* imgR = cvCreateImage( cvGetSize(img), img->depth, 1 );
        cvSplit( img, imgB, imgG, imgR, NULL );
        cvAdd( imgB, imgG, mask );
        cvAdd( mask, imgR, mask );
        cvReleaseImage( &imgB );
        cvReleaseImage( &imgG );
        cvReleaseImage( &imgR );
    }
    else
    {
        cvCopy( img, mask );
    }
    cvThreshold( mask, _mask, thresh, 1, CV_THRESH_BINARY );

    cvReleaseImage( &img );
    cvReleaseImage( &ref );
    cvReleaseImage( &mask );
    __END__;
}

/*
//this is a sample for foreground detection functions
//this is not training, but using pre-determined params
//See cvCreateGaussainBGModel in ./cvaux/src/cvbgfg_gaussmix.cpp
//This is setting paramets such as CV_BGFG_MOG_BACKGROUND_THRESHOLD (cvaux.h)
//cvUpdateBGStatModel means just setting bg_model->foreground, background
int main(int argc, char** argv)
{
    IplImage*       tmp_frame = NULL;
    CvCapture*      video = NULL;

    video = cvCaptureFromFile(argv[1]);
    tmp_frame = cvQueryFrame(video);
    if(!tmp_frame)
    {
        printf("bad video \n");
        exit(0);
    }

    cvNamedWindow("BG", 1);
    cvNamedWindow("FG", 1);

    //create BG model   
    CvBGStatModel* bg_model = cvCreateGaussianBGModel( tmp_frame );
     
    for( int fr = 1;tmp_frame; tmp_frame = cvQueryFrame(video), fr++ )
    {
        cvUpdateBGStatModel( tmp_frame, bg_model );
        cvShowImage("BG", bg_model->background);
        cvShowImage("FG", bg_model->foreground);
        int k = cvWaitKey(5);
        if( k == 27 ) break;
        //printf("frame# %d \r", fr);
    }


    cvReleaseBGStatModel( &bg_model );
    cvReleaseCapture(&video);

    return 0;
}*/

#endif
