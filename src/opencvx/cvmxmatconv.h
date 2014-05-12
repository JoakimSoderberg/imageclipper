/**
* OpenCV versus Matlab C Library (MEX) interfaces
*     verified under OpenCV 1.00 and Matlab 2007b
*
* Matrix conversion
*
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
#ifndef CVMX_MATCONV_INCLUDED
#define CVMX_MATCONV_INCLUDED


#include "cv.h"
#include "mat.h"
#include "matrix.h"
#include "cvmxtypeconv.h"

/************** Definitions *******************************/
CVAPI(mxArray*)     cvmxArrayFromCvArr(const CvArr* arr);
#define cvmxArrayFromIplImage(img) (cvmxArrayFromCvArr(img))
#define cvmxArrayFromCvMat(mat) (cvmxArrayFromCvArr(mat))
CVAPI(IplImage*)    cvmxArrayToIplImage(const mxArray* mxarr);
CVAPI(CvMat*)       cvmxArrayToCvMat(const mxArray* mxarr);

/************** Functions *********************************/
/**
* Convert CvArr to mxArray. Allocate memory too
*
* Currently support only uint8, float, double
* Currently support only 1 channel (grayscale) and 3 channels (rgb etc)
*
* @param CvArr* arr
* @return mxArray*
*/
CVAPI(mxArray*) cvmxArrayFromCvArr(const CvArr* arr)
{
    CV_FUNCNAME( "cvmxArraFromCvArr" );
    IplImage imghdr, *img = (IplImage*)arr;
    int nChannel, nRow, nCol, nDim;
    mwSize dims[3];
    mxClassID classid; mxArray* mxarr = NULL;
    int row, col, ch;

    __BEGIN__;
    if (!CV_IS_IMAGE(img)) {
        CV_CALL(img = cvGetImage(img, &imghdr));
    }
    nChannel = img->nChannels;
    nRow     = img->height;
    nCol     = img->width;
    nDim     = 2;
    classid  = cvmxClassIDFromIplDepth(img->depth);
    dims[0]  = nRow; dims[1] = nCol; dims[2] = nChannel;
    if (nChannel > 1) nDim = 3;
    mxarr    = mxCreateNumericArray(nDim, dims, classid, mxREAL);

    if (classid == mxUINT8_CLASS) {
        unsigned char *mxData = (unsigned char*)mxGetData(mxarr);
        for (ch = 0; ch < nChannel; ch++) {
            for (row = 0; row < dims[0]; row++) {
                for (col = 0; col < dims[1]; col++) {
                    mxData[dims[0] * dims[1] * ch + dims[0] * col + row]
                        = CV_IMAGE_ELEM(img, unsigned char, row, nChannel * col + ch);
                }
            }
        }
    } else if (classid == mxDOUBLE_CLASS) {
        double *mxData = (double*)mxGetData(mxarr);
        for (ch = 0; ch < nChannel; ch++) {
            for (row = 0; row < dims[0]; row++) {
                for (col = 0; col < dims[1]; col++) {
                    mxData[dims[0] * dims[1] * ch + dims[0] * col + row]
                        = CV_IMAGE_ELEM(img, double, row, nChannel * col + ch);
                }
            }
        }
    } else if (classid == mxSINGLE_CLASS) {
        float *mxData = (float*)mxGetData(mxarr);
        for (ch = 0; ch < nChannel; ch++) {
            for (row = 0; row < dims[0]; row++) {
                for (col = 0; col < dims[1]; col++) {
                    mxData[dims[0] * dims[1] * ch + dims[0] * col + row]
                        = CV_IMAGE_ELEM(img, float, row, nChannel * col + ch);
                }
            }
        }
    }
    __END__;
    return mxarr;
}

/**
* Convert mxArray to IplImage. Allocate memory too
*
* Currently support only uint8, float, double
* Currently support only 1 channel (grayscale) and 3 channels (rgb etc)
*
* @param mxArray* mxarr
* @return IplImage*
*/
CVAPI(IplImage*) cvmxArrayToIplImage(const mxArray* mxarr)
{
    CV_FUNCNAME( "cvmxArrayToCvArr" );
    IplImage *img = NULL;
    int depth;
    mwSize nChannel = 1;
    mwSize nDim;
    const mwSize *dims;
    mxClassID classid;
    int row, col, ch;

    __BEGIN__;
    classid = mxGetClassID(mxarr);
    nDim = mxGetNumberOfDimensions(mxarr);
    dims = mxGetDimensions(mxarr);
    if (nDim >= 3) nChannel = dims[2];
    depth = cvmxClassIDToIplDepth(mxGetClassID(mxarr));
    img   = cvCreateImage(cvSize(dims[1], dims[0]), depth, nChannel);

    if (classid == mxUINT8_CLASS) {
        unsigned char *mxData = (unsigned char*)mxGetData(mxarr);
        for (ch = 0; ch < nChannel; ch++) {
            for (row = 0; row < dims[0]; row++) {
                for (col = 0; col < dims[1]; col++) {
                    CV_IMAGE_ELEM(img, unsigned char, row, nChannel * col + ch)
                        = mxData[dims[0] * dims[1] * ch + dims[0] * col + row];
                }
            }
        }
    } else if (classid == mxDOUBLE_CLASS) {
        double *mxData = (double*)mxGetData(mxarr);
        for (ch = 0; ch < nChannel; ch++) {
            for (row = 0; row < dims[0]; row++) {
                for (col = 0; col < dims[1]; col++) {
                    CV_IMAGE_ELEM(img, double, row, nChannel * col + ch)
                        = mxData[dims[0] * dims[1] * ch + dims[0] * col + row];
                }
            }
        }
    } else if (classid == mxSINGLE_CLASS) {
        float *mxData = (float*)mxGetData(mxarr);
        for (ch = 0; ch < nChannel; ch++) {
            for (row = 0; row < dims[0]; row++) {
                for (col = 0; col < dims[1]; col++) {
                    CV_IMAGE_ELEM(img, float, row, nChannel * col + ch)
                        = mxData[dims[0] * dims[1] * ch + dims[0] * col + row];
                }
            }
        }
    }
    __END__;
    return img;
}

/**
* Convert mxArray to CvMat. Allocate memory too
*
* Currently support only uint8, float, double
* Currently support only 1 channel (grayscale) and 3 channels (rgb etc)
*
* @param mxArray* mxarr
* @return CvMat*
*/
CVAPI(CvMat*) cvmxArrayToCvMat(const mxArray* mxarr)
{
    IplImage* img = cvmxArrayToIplImage(mxarr);
    CvMat *mat = cvCreateMat(img->height, img->width, CV_MAKETYPE(cvmxIplToCvDepth(img->depth), img->nChannels));
    cvConvert(img, mat);
    cvReleaseImage(&img);
    // @todo: why cvGetMat results in error?
    //int coi = 0;
    //IplImage *img = cvmxArrayToIplImage(mxarr);
    //CvMat *mat, mathdr;
    //mat = cvGetMat(mat, &mathdr, &coi, 0);
    return mat;
}



#endif
