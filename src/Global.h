#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

/************************************ Structure *************************************/

/**
* A Callback function structure
*/
typedef struct CvCallbackParam {
	const char* w_name;								// Title - interactive window's title 
	const char* miniw_name;							// Title - the title of the window showing crop result 
	IplImage* img_src;								// Cache - Current image pointer
	CvRect rect;									// Cache - Current selected pointer
	CvRect circle;									// Cache - Current watershed.	use x, y for center, width as radius. width == 0 means watershed is off
	int rotate;										// Cache - Current roteted degree
	CvPoint shear;
	std::vector<std::string> imtypes;				// Enum - image types
	std::vector<std::string> filelist;				// for image or directory load
	std::vector<std::string>::iterator fileiter;
	CvCapture* cap;									// Cache - for video load
	int frame;										// Cache - video iter
	const char* output_format;						// Pattern - output pattern

	CvSize screen_size;								// Cache - screen resolution
	IplImage* img_display;							// Cache - Current image pointer
	float scale_factor;								// Cache - global scale factor
	float cap_scale_factor;							// Cache - scale factor of capture
} CvCallbackParam;


/**
* MouseCallback param structure
*/
typedef struct MouseCallbackStatus {
	CvPoint hit_point;
	bool move_rect;
	bool resize_rect_left;
	bool resize_rect_right;
	bool resize_rect_top;
	bool resize_rect_bottom;
	bool move_watershed;
	bool resize_watershed;
} MouseCallbackStatus;

/**
* Command Argument structure
*/
typedef struct ArgParam {
	const char* name;								// Path - absolute path of executor
	std::string reference;							// Path - path of reference folder
	const char* imgout_format;						// Pattern - default output pattern of image
	const char* vidout_format;						// Pattern - default output pattern of video
	const char* output_format;						// Pattern - user specifying output pattern
	int frame;										// Num - how much frame does the program need read from video
} ArgParam;

#endif // __GLOBAL_H__
