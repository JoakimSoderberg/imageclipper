/**
* The MIT License
* 
* Copyright (c) 2008, Naotoshi Seo <sonots(at)umd.edu>
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
#ifdef _MSC_VER // MS Visual Studio
#pragma warning(disable:4996)
#pragma warning(disable:4244) // possible loss of data
#pragma warning(disable:4819) // Save the file in Unicode format to prevent data loss
#endif

#include "cv.h"
#include "cvaux.h"
#include "cxcore.h"
#include "highgui.h"
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <string>
#include <vector>
#include "filesystem.h"
#include "icformat.h"
#include "cvdrawwatershed.h"
#include "opencvx/cvrect32f.h"
#include "opencvx/cvdrawrectangle.h"
#include "opencvx/cvcropimageroi.h"
#include "opencvx/cvpointnorm.h"
using namespace std;

/************************************ Structure *************************************/

/**
* A Callback function structure
*/
typedef struct CvCallbackParam {
    const char* w_name;
    const char* miniw_name;
    IplImage* img;
    CvRect rect;
    CvRect circle; // use x, y for center, width as radius. width == 0 means watershed is off
    int rotate;
    CvPoint shear;
    vector<string> imtypes;
    vector<string> filelist; // for image or directory load
    vector<string>::iterator fileiter;
    CvCapture* cap;          // for video load
    int frame;               // video iter
    const char* output_format;
} CvCallbackParam ;

/**
* Command Argument structure
*/
typedef struct ArgParam {
    const char* name;
    string reference;
    const char* imgout_format;
    const char* vidout_format;
    const char* output_format;
    int   frame;
} ArgParam;

/************************* Function Prototypes *********************************/
void arg_parse( int argc, char** argv, ArgParam* arg = NULL );
void usage( const ArgParam* arg );
void gui_usage();
void mouse_callback( int event, int x, int y, int flags, void* _param );
void load_reference( const ArgParam* arg, CvCallbackParam* param );
void key_callback( const ArgParam* arg, CvCallbackParam* param );

/************************* Main ************************************************/

int main( int argc, char *argv[] )
{
    // initialization
    CvCallbackParam init_param = {
        "<S> Save <F> Forward <SPACE> s and f <B> Backward <ESC> Exit",
        "Cropped",
        NULL,
        cvRect(0,0,0,0),
        cvRect(0,0,0,0),
        0,
        cvPoint(0,0),
        vector<string>(),
        vector<string>(),
        vector<string>::iterator(),
        NULL,
        0
    };
    CvCallbackParam* param = &init_param;
    param->imtypes.push_back( "bmp" );
    param->imtypes.push_back( "dib" );
    param->imtypes.push_back( "jpeg" );
    param->imtypes.push_back( "jpg" );
    param->imtypes.push_back( "jpe" );
    param->imtypes.push_back( "png" );
    param->imtypes.push_back( "pbm" );
    param->imtypes.push_back( "pbm" );
    param->imtypes.push_back( "ppm" );
    param->imtypes.push_back( "sr" );
    param->imtypes.push_back( "ras" );
    param->imtypes.push_back( "tiff" );
    param->imtypes.push_back( "exr" );
    param->imtypes.push_back( "jp2" );

    ArgParam init_arg = {
        argv[0],
        ".",
        "%d/imageclipper/%i.%e_%04r_%04x_%04y_%04w_%04h.png",
        "%d/imageclipper/%i.%e_%04f_%04r_%04x_%04y_%04w_%04h.png",
        NULL,
        1
    };
    ArgParam *arg = &init_arg;

    // parse arguments
    arg_parse( argc, argv, arg );
    gui_usage();
    load_reference( arg, param );

    // Mouse and Key callback
    cvNamedWindow( param->w_name, CV_WINDOW_AUTOSIZE );
    cvNamedWindow( param->miniw_name, CV_WINDOW_AUTOSIZE );
    cvSetMouseCallback( param->w_name, mouse_callback, param );
    key_callback( arg, param );
    cvDestroyWindow( param->w_name );
    cvDestroyWindow( param->miniw_name );
}

void load_reference( const ArgParam* arg, CvCallbackParam* param )
{
    bool is_directory = fs::is_directory( arg->reference );
    bool is_image = fs::match_extensions( arg->reference, param->imtypes );
    bool is_video = !is_directory & !is_image;
    param->output_format = ( arg->output_format != NULL ? arg->output_format : 
        ( is_video ? arg->vidout_format : arg->imgout_format ) );
    param->frame = arg->frame;

    if( is_directory || is_image )
    {
        if( is_directory )
        {
            cerr << "Now reading a directory..... ";

            param->filelist = fs::filelist( arg->reference, param->imtypes, "file" );
            if( param->filelist.empty() )
            {
                usage( arg );
                cerr << "No image file exist under the directory " << fs::realpath( arg->reference ) << endl << endl;
                exit(1);
            }

            if ( (param->filelist.begin() + arg->frame) > param->filelist.end() )
            {
                usage( arg );
                cerr << "Specified frame " << arg->frame << " is past the end of the list of "
                     << param->filelist.size() << " files " << endl;
                exit(1);
            }

            param->fileiter = param->filelist.begin() + arg->frame;
        }
        else
        {
            if( !fs::exists( arg->reference ) )
            {
                usage( arg );
                cerr << "The image file " << fs::realpath( arg->reference ) << " does not exist." << endl << endl;
                exit(1);
            }
            param->filelist = fs::filelist( fs::dirname( arg->reference ), param->imtypes, "file" );

            // step up till specified file
            for( param->fileiter = param->filelist.begin(); param->fileiter != param->filelist.end(); param->fileiter++ )
            {
                if( fs::realpath( *param->fileiter ) == fs::realpath( arg->reference ) ) break;
            }
        }

        cerr << "Done!" << endl;
        cerr << arg->frame << " Now showing " << fs::realpath( *param->fileiter ) << endl;
        param->img = cvLoadImage( fs::realpath( *param->fileiter ).c_str() );
    }
    else if( is_video )
    {
        if ( !fs::exists( arg->reference ) )
        {
            usage( arg );
            cerr << "The file " << fs::realpath( arg->reference ) << " does not exist or is not readable." << endl << endl;
            exit(1);
        }
        cerr << "Now reading a video..... ";
        param->cap = cvCaptureFromFile( fs::realpath( arg->reference ).c_str() );
        cvSetCaptureProperty( param->cap, CV_CAP_PROP_POS_FRAMES, arg->frame - 1 );
        param->img = cvQueryFrame( param->cap );
        if( param->img == NULL )
        {
            usage( arg );
            cerr << "The file " << fs::realpath( arg->reference ) << " was assumed as a video, but not loadable." << endl << endl;
            exit(1);
        }
        cerr << "Done!" << endl;
        cerr << cvGetCaptureProperty( param->cap, CV_CAP_PROP_FRAME_COUNT ) << " frames totally." << endl;
        cerr << " Now showing " << fs::realpath( arg->reference ) << " " << arg->frame << endl;
    }
    else
    {
        usage( arg );
        cerr << "The directory " << fs::realpath( arg->reference ) << " does not exist." << endl << endl;
        exit(1);
    }
}

void key_callback( const ArgParam* arg, CvCallbackParam* param )
{
    string filename = param->cap == NULL ? *param->fileiter : arg->reference;

    cvShowCroppedImage( param->miniw_name, param->img, 
        cvRect32fFromRect( param->rect, param->rotate ), 
        cvPointTo32f( param->shear ) );
    cvShowImageAndRectangle( param->w_name, param->img, 
        cvRect32fFromRect( param->rect, param->rotate ), 
        cvPointTo32f( param->shear ) );

    while( true ) // key callback
    {
        char key = cvWaitKey( 0 );
        cout << "Key pressed: " << (int)key << endl;

        // 32 is SPACE
        if( key == 's' || key == 32 ) // Save
        {
            if( param->rect.width > 0 && param->rect.height > 0 )
            {
                string output_path = icFormat( 
                    param->output_format, fs::dirname( filename ), 
                    fs::filename( filename ), fs::extension( filename ),
                    param->rect.x, param->rect.y, param->rect.width, param->rect.height, 
                    param->frame, param->rotate );

                if( !fs::match_extensions( output_path, param->imtypes ) )
                {
                    cerr << "The image type " << fs::extension( output_path ) << " is not supported." << endl;
                    exit(1);
                }
                fs::create_directories( fs::dirname( output_path ) );

                IplImage* crop = cvCreateImage( 
                    cvSize( param->rect.width, param->rect.height ), 
                    param->img->depth, param->img->nChannels );
                cvCropImageROI( param->img, crop, 
                                cvRect32fFromRect( param->rect, param->rotate ), 
                                cvPointTo32f( param->shear ) );
                cvSaveImage( fs::realpath( output_path ).c_str(), crop );
                cout << fs::realpath( output_path ) << endl;
                cvReleaseImage( &crop );
            }
        }
        // Forward
        if( key == 'f' || key == 32 ) // 32 is SPACE
        {
            if( param->cap )
            {
                IplImage* tmpimg = cvQueryFrame( param->cap );
                if( tmpimg != NULL )
                //if( frame < cvGetCaptureProperty( param->cap, CV_CAP_PROP_FRAME_COUNT ) )
                {
                    param->img = tmpimg;
                    param->frame++;
                    cout << "Now showing " << fs::realpath( filename ) << " " <<  param->frame << endl;
                }
            }
            else
            {
                if( param->fileiter + 1 != param->filelist.end() )
                {
                    cvReleaseImage( &param->img );
                    param->fileiter++;
                    filename = *param->fileiter;
                    param->img = cvLoadImage( fs::realpath( filename ).c_str() );
                    cout << (param->fileiter - param->filelist.begin()) << " Now showing " << fs::realpath( filename ) << endl;
                }
            }
        }
        // Backward
        else if( key == 'b' )
        {
            if( param->cap )
            {
                IplImage* tmpimg;
                param->frame = max( 1, param->frame - 1 );
                cvSetCaptureProperty( param->cap, CV_CAP_PROP_POS_FRAMES, param->frame - 1 );
                if( (tmpimg = cvQueryFrame( param->cap )) )
                {
                    param->img = tmpimg;
                    cout << "Now showing " << fs::realpath( filename ) << " " <<  param->frame << endl;
                }
            }
            else
            {
                if( param->fileiter != param->filelist.begin() ) 
                {
                    cvReleaseImage( &param->img );
                    param->fileiter--;
                    filename = *param->fileiter;
                    param->img = cvLoadImage( fs::realpath( filename ).c_str() );
                    cout << "Now showing " << fs::realpath( filename ) << endl;
                }
            }
        }
        // Exit
        else if( key == 'q' || key == 27 ) // 27 is ESC
        {
            break;
        }

        if( param->circle.width > 0 ) // watershed
        {
            // Rectangle Movement (Vi like hotkeys)
            if( key == 'h' ) // Left
            {
                param->circle.x -= 1;
            }
            else if( key == 'j' ) // Down
            {
                param->circle.y += 1;
            }
            else if( key == 'k' ) // Up
            {
                param->circle.y -= 1;
            }
            else if( key == 'l' ) // Right
            {
                param->circle.x += 1;
            }
            // Rectangle Resize
            else if( key == 'y' ) // Shrink width
            {
                param->circle.width -= 1;
            }
            else if( key == 'u' ) // Expand height
            {
                param->circle.width += 1;
            }
            else if( key == 'i' ) // Shrink height
            {
                param->circle.width -= 1;
            }
            else if( key == 'o' ) // Expand width
            {
                param->circle.width += 1;
            }
            // Shear Deformation
            else if( key == 'n' ) // Left
            {
                param->shear.x -= 1;
            }
            else if( key == 'm' ) // Down
            {
                param->shear.y += 1;
            }
            else if( key == ',' ) // Up
            {
                param->shear.y -= 1;
            }
            else if( key == '.' ) // Right
            {
                param->shear.x += 1;
            }
            // Rotation
            else if( key == 'R' ) // Counter-Clockwise
            {
                param->rotate += 1;
                param->rotate = (param->rotate >= 360) ? param->rotate - 360 : param->rotate;
            }
            else if( key == 'r' ) // Clockwise
            {
                param->rotate -= 1;
                param->rotate = (param->rotate < 0) ? 360 + param->rotate : param->rotate;
            }
            else if( key == 'e' ) // Expand
            {
                param->circle.width += 1;
            }
            else if( key == 'E' ) // Shrink
            {
                param->circle.width -= 1;
            }

            if( param->img )
            {
                param->rect = cvShowImageAndWatershed( param->w_name, param->img, param->circle );
                cvShowCroppedImage( param->miniw_name, param->img, 
                                    cvRect32fFromRect( param->rect, param->rotate ), 
                                    cvPointTo32f( param->shear ) );
            }
        }
        else
        {
            #define SMALL 1
            #define BIG 10

            switch (key)
            {
                //
                // Rectangle Movement (Vi like hotkeys).
                //

                // Left.
                case 29:
                case 'h': param->rect.x -= SMALL; break;
                case 'H': param->rect.x -= BIG; break;
                // Down.
                case 31:
                case 'j': param->rect.y += SMALL; break;
                case 'J': param->rect.y += BIG; break;
                // Up.
                case 30:
                case 'k': param->rect.y -= SMALL; break;
                case 'K': param->rect.y -= BIG; break;
                // Right.
                case 28:
                case 'l': param->rect.x += SMALL; break;
                case 'L': param->rect.x += BIG; break;

                //
                // Rectangle Resize.
                //

                // Shrink width.
                case 'y':
                    param->rect.width = max( 0, param->rect.width - 1 );
                    break;
                // Expand height.
                case 'u':
                    param->rect.height += 1;
                    break;
                // Shrink height.
                case 'i':
                    param->rect.height = max( 0, param->rect.height - 1 );
                    break;
                // Expand width.
                case 'o':
                    param->rect.width += 1;
                    break;

                //
                // Shear Deformation.
                //

                // Left.
                case 'n':
                    param->shear.x -= 1;
                    break;
                // Down.
                case 'm':
                    param->shear.y += 1;
                    break;
                // Up.
                case ',':
                    param->shear.y -= 1;
                    break;
                // Right.
                case '.':
                    param->shear.x += 1;
                    break;

                //
                // Rotation.
                //

                // Counter-Clockwise.
                case 'R':
                    param->rotate += 1;
                    param->rotate = (param->rotate >= 360) ? param->rotate - 360 : param->rotate;
                    break;
                // Clockwise.
                case 'r':
                    param->rotate -= 1;
                    param->rotate = (param->rotate < 0) ? 360 + param->rotate : param->rotate;
                    break;
                // Expand.
                case 'e':
                    param->rect.x = max( 0, param->rect.x - 1 );
                    param->rect.width += 2;
                    param->rect.y = max( 0, param->rect.y - 1 );
                    param->rect.height += 2;
                    break;
                // Shrink.
                case 'E':
                    param->rect.x = min( param->img->width, param->rect.x + 1 );
                    param->rect.width = max( 0, param->rect.width - 2 );
                    param->rect.y = min( param->img->height, param->rect.y + 1 );
                    param->rect.height = max( 0, param->rect.height - 2 );
                    break;
            }

            /*
              if( key == 'e' || key == 'E' ) // Expansion and Shrink so that ratio does not change
              {
              if( param->rect.height != 0 && param->rect.width != 0 ) 
              {
              int gcd, a = param->rect.width, b = param->rect.height;
              while( 1 )
              {
              a = a % b;
              if( a == 0 ) { gcd = b; break; }
              b = b % a;
              if( b == 0 ) { gcd = a; break; }
              }
              int ratio_width = param->rect.width / gcd;
              int ratio_height = param->rect.height / gcd;
              if( key == 'e' ) gcd += 1;
              else if( key == 'E' ) gcd -= 1;
              if( gcd > 0 )
              {
              cout << ratio_width << ":" << ratio_height << " * " << gcd << endl;
              param->rect.width = ratio_width * gcd;
              param->rect.height = ratio_height * gcd; 
              cvShowImageAndRectangle( param->w_name, param->img, 
              cvRect32fFromRect( param->rect, param->rotate ), 
              cvPointTo32f( param->shear ) );
              }
              }
              }*/

            if( param->img )
            {
                cvShowImageAndRectangle( param->w_name, param->img, 
                                         cvRect32fFromRect( param->rect, param->rotate ), 
                                         cvPointTo32f( param->shear ) );
                cvShowCroppedImage( param->miniw_name, param->img, 
                                    cvRect32fFromRect( param->rect, param->rotate ), 
                                    cvPointTo32f( param->shear ) );
            }
        }
    }
}

/**
* cvSetMouseCallback function
*/
void mouse_callback( int event, int x, int y, int flags, void* _param )
{
    CvCallbackParam* param = (CvCallbackParam*)_param;
    static CvPoint point0          = cvPoint( 0, 0 );
    static bool move_rect          = false;
    static bool resize_rect_left   = false;
    static bool resize_rect_right  = false;
    static bool resize_rect_top    = false;
    static bool resize_rect_bottom = false;
    static bool move_watershed     = false;
    static bool resize_watershed   = false;

    if( !param->img )
        return;

    if( x >= 32768 ) x -= 65536; // change left outsite to negative
    if( y >= 32768 ) y -= 65536; // change top outside to negative

    // MBUTTON or LBUTTON + SHIFT is to draw wathershed
    if( (event == CV_EVENT_MBUTTONDOWN) || 
        ( (event == CV_EVENT_LBUTTONDOWN) && (flags & CV_EVENT_FLAG_SHIFTKEY) ) ) // initialization
    {
        param->circle.x = x;
        param->circle.y = y;
    }
    else if( ((event == CV_EVENT_MOUSEMOVE) && (flags & CV_EVENT_FLAG_MBUTTON)) ||
        ( (event == CV_EVENT_MOUSEMOVE) && (flags & CV_EVENT_FLAG_LBUTTON) && (flags & CV_EVENT_FLAG_SHIFTKEY) ) )
    {
        param->rotate  = 0;
        param->shear.x = param->shear.y = 0;

        param->circle.width = (int) cvPointNorm( cvPoint( param->circle.x, param->circle.y ), cvPoint( x, y ) );
        param->rect = cvShowImageAndWatershed( param->w_name, param->img, param->circle );
        cvShowCroppedImage( param->miniw_name, param->img, 
                            cvRect32fFromRect( param->rect, param->rotate ), 
                            cvPointTo32f( param->shear ) );
    }

    // LBUTTON is to draw rectangle
    else if( event == CV_EVENT_LBUTTONDOWN ) // initialization
    {
        point0 = cvPoint( x, y );
    }
    else if( event == CV_EVENT_MOUSEMOVE && flags & CV_EVENT_FLAG_LBUTTON )
    {
        param->circle.width = 0; // disable watershed
        param->rotate       = 0;
        param->shear.x      = param->shear.y = 0;

        param->rect.x = min( point0.x, x );
        param->rect.y = min( point0.y, y );
        param->rect.width =  abs( point0.x - x );
        param->rect.height = abs( point0.y - y );

        cvShowImageAndRectangle( param->w_name, param->img, 
                                 cvRect32fFromRect( param->rect, param->rotate ), 
                                 cvPointTo32f( param->shear ) );
        cvShowCroppedImage( param->miniw_name, param->img, 
                            cvRect32fFromRect( param->rect, param->rotate ), 
                            cvPointTo32f( param->shear ) );
    }

    // RBUTTON to move rentangle or watershed marker
    else if( event == CV_EVENT_RBUTTONDOWN )
    {
        point0 = cvPoint( x, y );

        if( param->circle.width != 0 )
        {
            CvPoint center = cvPoint( param->circle.x, param->circle.y );
            int radius = (int) cvPointNorm( center, point0 );
            if( param->circle.width - 1 <= radius && radius <= param->circle.width )
            {
                resize_watershed = true;
            }
            else if( radius <= param->circle.width )
            {
                move_watershed = true;
            }
        }
        if( !resize_watershed && !move_watershed )
        {
            param->circle.width = 0;
            if( ( param->rect.x < x && x < param->rect.x + param->rect.width ) && 
                ( param->rect.y < y && y < param->rect.y + param->rect.height ) )
            {
                move_rect = true;
            }
            if( x <= param->rect.x )
            {
                resize_rect_left = true; 
            }
            else if( x >= param->rect.x + param->rect.width )
            {
                resize_rect_right = true;
            }
            if( y <= param->rect.y )
            {
                resize_rect_top = true; 
            }
            else if( y >= param->rect.y + param->rect.height )
            {
                resize_rect_bottom = true;
            }
        }
    }
    else if( event == CV_EVENT_MOUSEMOVE && flags & CV_EVENT_FLAG_RBUTTON && param->circle.width != 0 ) // Move or resize for watershed
    {
        if( move_watershed )
        {
            CvPoint move = cvPoint( x - point0.x, y - point0.y );
            param->circle.x += move.x;
            param->circle.y += move.y;

            param->rect = cvShowImageAndWatershed( param->w_name, param->img, param->circle );
            cvShowCroppedImage( param->miniw_name, param->img, 
                                cvRect32fFromRect( param->rect, param->rotate ),
                                cvPointTo32f( param->shear ) );

            point0 = cvPoint( x, y );
        }
        else if( resize_watershed )
        {
            param->circle.width = (int) cvPointNorm( cvPoint( param->circle.x, param->circle.y ), cvPoint( x, y ) );
            param->rect = cvShowImageAndWatershed( param->w_name, param->img, param->circle );
            cvShowCroppedImage( param->miniw_name, param->img, 
                                cvRect32fFromRect( param->rect, param->rotate ), 
                                cvPointTo32f( param->shear ) );
        }
    }
    else if( event == CV_EVENT_MOUSEMOVE && flags & CV_EVENT_FLAG_RBUTTON ) // Move or resize for rectangle
    {
        if( move_rect )
        {
            CvPoint move = cvPoint( x - point0.x, y - point0.y );
            param->rect.x += move.x;
            param->rect.y += move.y;
        }
        if( resize_rect_left )
        {
            int move_x = x - point0.x;
            param->rect.x += move_x;
            param->rect.width -= move_x;
        }
        else if( resize_rect_right )
        {
            int move_x = x - point0.x;
            param->rect.width += move_x;
        }
        if( resize_rect_top )
        {
            int move_y = y - point0.y;
            param->rect.y += move_y;
            param->rect.height -= move_y;
        }
        else if( resize_rect_bottom )
        {
            int move_y = y - point0.y;
            param->rect.height += move_y;
        }

        // assure width is positive
        if( param->rect.width <= 0 )
        {
            param->rect.x += param->rect.width;
            param->rect.width *= -1;
            bool tmp = resize_rect_right;
            resize_rect_right = resize_rect_left;
            resize_rect_left  = tmp;
        }
        // assure height is positive
        if( param->rect.height <= 0 )
        {
            param->rect.y += param->rect.height;
            param->rect.height *= -1;
            bool tmp = resize_rect_top;
            resize_rect_top    = resize_rect_bottom;
            resize_rect_bottom = tmp;
        }

        cvShowImageAndRectangle( param->w_name, param->img, 
                                 cvRect32fFromRect( param->rect, param->rotate ), 
                                 cvPointTo32f( param->shear ) );
        cvShowCroppedImage( param->miniw_name, param->img, 
                            cvRect32fFromRect( param->rect, param->rotate ), 
                            cvPointTo32f( param->shear ) );
        point0 = cvPoint( x, y );
    }

    // common finalization
    else if( event == CV_EVENT_LBUTTONUP || event == CV_EVENT_MBUTTONUP || event == CV_EVENT_RBUTTONUP )
    {
        move_rect          = false;
        resize_rect_left   = false;
        resize_rect_right  = false;
        resize_rect_top    = false;
        resize_rect_bottom = false;
        move_watershed     = false;
        resize_watershed   = false;
    }
}

/**
 * Arguments Processing
 */
void arg_parse( int argc, char** argv, ArgParam *arg )
{
    arg->name = argv[0];
    for( int i = 1; i < argc; i++ )
    {
        if( !strcmp( argv[i], "-h" ) || !strcmp( argv[i], "--help" ) )
        {
            usage( arg );
            return;
        } 
        else if( !strcmp( argv[i], "-o" ) || !strcmp( argv[i], "--output_format" ) )
        {
            arg->output_format = argv[++i];
        }
        else if( !strcmp( argv[i], "-i" ) || !strcmp( argv[i], "--imgout_format" ) )
        {
            arg->imgout_format = argv[++i];
        }
        else if( !strcmp( argv[i], "-v" ) || !strcmp( argv[i], "--vidout_format" ) )
        {
            arg->vidout_format = argv[++i];
        }
        else if( !strcmp( argv[i], "-f" ) || !strcmp( argv[i], "--frame" ) )
        {
            arg->frame = atoi( argv[++i] );
        }
        else
        {
            arg->reference = string( argv[i] );
        }
    }
}

/**
* Print out usage
*/
void usage( const ArgParam* arg )
{
    cout << "ImageClipper - image clipping helper tool." << endl;
    cout << "Command Usage: " << fs::basename( arg->name );
    cout << " [option]... [arg_reference]" << endl;
    cout << "  <arg_reference = " << arg->reference << ">" << endl;
    cout << "    <arg_reference> would be a directory or an image or a video filename." << endl;
    cout << "    For a directory, image files in the directory will be read sequentially." << endl;
    cout << "    For an image, it starts to read a directory from the specified image file. " << endl;
    cout << "    (A file is judged as an image based on its filename extension.)" << endl;
    cout << "    A file except images is tried to be read as a video and read frame by frame. " << endl;
    cout << endl;
    cout << "  Options" << endl;
    cout << "    -o <output_format = imgout_format or vidout_format>" << endl;
    cout << "        Determine the output file path format." << endl;
    cout << "        This is a syntax sugar for -i and -v. " << endl;
    cout << "        Format Expression)" << endl;
    cout << "            %d - dirname of the original" << endl;
    cout << "            %i - filename of the original without extension" << endl;
    cout << "            %e - filename extension of the original" << endl;
    cout << "            %x - upper-left x coord" << endl;
    cout << "            %y - upper-left y coord" << endl;
    cout << "            %w - width" << endl;
    cout << "            %h - height" << endl;
    cout << "            %r - rotation degree" << endl;
    cout << "            %. - shear deformation in x coord" << endl;
    cout << "            %, - shear deformation in y coord" << endl;
    cout << "            %f - frame number (for video)" << endl;
    cout << "        Example) ./$i_%04x_%04y_%04w_%04h.%e" << endl;
    cout << "            Store into software directory and use image type of the original." << endl;
    cout << "    -i <imgout_format = " << arg->imgout_format << ">" << endl;
    cout << "        Determine the output file path format for image inputs." << endl;
    cout << "    -v <vidout_format = " << arg->vidout_format << ">" << endl;
    cout << "        Determine the output file path format for a video input." << endl;
    cout << "    -f" << endl;
    cout << "    --frame <frame = 1> (video)" << endl;
    cout << "        Determine the frame number of video to start to read." << endl;
    cout << "    -h" << endl;
    cout << "    --help" << endl;
    cout << "        Show this help" << endl;
    cout << endl;
    cout << "  Supported Image Types" << endl;
    cout << "      bmp|dib|jpeg|jpg|jpe|png|pbm|pgm|ppm|sr|ras|tiff|exr|jp2" << endl;
}

/**
* Print Application Usage
*/
void gui_usage()
{
    cout << "Application Usage:" << endl;
    cout << "  Mouse Usage:" << endl;
    cout << "    Left  (select)          : Select or initialize a rectangle region." << endl;
    cout << "    Right (move or resize)  : Move by dragging inside the rectangle." << endl;
    cout << "                              Resize by draggin outside the rectangle." << endl;
    cout << "    Middle or SHIFT + Left  : Initialize the watershed marker. Drag it. " << endl;
    cout << "  Keyboard Usage:" << endl;
    cout << "    s (save)                : Save the selected region as an image." << endl;
    cout << "    f (forward)             : Forward. Show next image." << endl;
    cout << "    SPACE                   : Save and Forward." << endl;
    cout << "    b (backward)            : Backward. " << endl;
    cout << "    q (quit) or ESC         : Quit. " << endl;
    cout << "    r (rotate) R (counter)  : Rotate rectangle in clockwise." << endl;
    cout << "    e (expand) E (shrink)   : Expand the recntagle size." << endl;
    cout << "    h (left) j (down) k (up) l (right) : Move rectangle. (vi-like keybinds)" << endl;
    cout << "                                         (+shift to move faster)" << endl;
    cout << "    y (left) u (down) i (up) o (right) : Resize rectangle (Move boundaries)." << endl;
    cout << "    n (left) m (down) , (up) . (right) : Shear deformation." << endl;
}

