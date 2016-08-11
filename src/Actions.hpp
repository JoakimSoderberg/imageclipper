#ifndef __ACTIONS_HPP__
#define __ACTIONS_HPP__

#include <opencv2/opencv.hpp>
#include "Global.h"


void act_draw(CvCallbackParam* param, MouseCallbackStatus* status, int x, int y, int flags)
{
	// Is pressing Ctrl?
	if (flags & CV_EVENT_FLAG_CTRLKEY)		// Draw a square
	{
		param->rect.width = param->rect.height = max(abs(status->hit_point.x - x), abs(status->hit_point.y - y));
		param->rect.x = (x > status->hit_point.x) ? status->hit_point.x : status->hit_point.x - param->rect.width;
		param->rect.y = (y > status->hit_point.y) ? status->hit_point.y : status->hit_point.y - param->rect.height;
	}
	else									// Draw a rectangle
	{
		param->rect.x = min(status->hit_point.x, x);
		param->rect.y = min(status->hit_point.y, y);
		param->rect.width = abs(status->hit_point.x - x);
		param->rect.height = abs(status->hit_point.y - y);
	}

	// Is pressing Alt?
	if (flags & CV_EVENT_FLAG_ALTKEY)
	{
		if (x > status->hit_point.x)
			param->rect.x -= param->rect.width;

		if (y > status->hit_point.y)
			param->rect.y -= param->rect.height;

		param->rect.width *= 2;
		param->rect.height *= 2;
	}

	// clean
	param->circle.width = 0; // disable watershed
	param->rotate = 0;
	param->shear.x = param->shear.y = 0;

	// show
	cvShowImageAndRectangle(param->w_name, param->img_display, cvRect32fFromRect(param->rect, param->rotate), cvPointTo32f(param->shear));
	cvShowCroppedImage(param->miniw_name, param->img_src, cvRect32fFromRect(cvScaleRect(param->rect, 1 / param->scale_factor), param->rotate), cvPointTo32f(param->shear), param->cap_scale_factor);
}


void inline act_watershed_begin(CvCallbackParam* param, MouseCallbackStatus* status, int x, int y, int flags)
{
	param->circle.x = x;
	param->circle.y = y;
}

void act_watershed_process(CvCallbackParam* param, MouseCallbackStatus* status, int x, int y, int flags)
{
	param->rotate = 0;
	param->shear.x = param->shear.y = 0;

	param->circle.width = (int)cvPointNorm(cvPoint(param->circle.x, param->circle.y), cvPoint(x, y));
	param->rect = cvShowImageAndWatershed(param->w_name, param->img_display, param->circle);
	cvShowCroppedImage(param->miniw_name, param->img_src, cvRect32fFromRect(cvScaleRect(param->rect, 1 / param->scale_factor), param->rotate), cvPointTo32f(param->shear), param->cap_scale_factor);
}


void act_move_begin(CvCallbackParam* param, MouseCallbackStatus* status, int x, int y, int flags)
{
	status->hit_point = cvPoint(x, y);

	// move watershed
	if (param->circle.width != 0)
	{
		CvPoint center = cvPoint(param->circle.x, param->circle.y);
		int radius = (int)cvPointNorm(center, status->hit_point);
		if (param->circle.width - 1 <= radius && radius <= param->circle.width)
			status->resize_watershed = true;
		else if (radius <= param->circle.width)
			status->move_watershed = true;
	}

	// move reatangle
	else if ((param->rect.x < x && x < param->rect.x + param->rect.width) && (param->rect.y < y && y < param->rect.y + param->rect.height))
		status->move_rect = true;

	else
	{
		if (x <= param->rect.x)
			status->resize_rect_left = true;
		else if (x >= param->rect.x + param->rect.width)
			status->resize_rect_right = true;

		if (y <= param->rect.y)
			status->resize_rect_top = true;
		else if (y >= param->rect.y + param->rect.height)
			status->resize_rect_bottom = true;
	}
}

void act_move_process(CvCallbackParam* param, MouseCallbackStatus* status, int x, int y, int flags)
{
	if (status->move_watershed)
	{
		CvPoint move = cvPoint(x - status->hit_point.x, y - status->hit_point.y);
		param->circle.x += move.x;
		param->circle.y += move.y;

		param->rect = cvShowImageAndWatershed(param->w_name, param->img_display, param->circle);
		cvShowCroppedImage(param->miniw_name, param->img_src, cvRect32fFromRect(cvScaleRect(param->rect, 1 / param->scale_factor), param->rotate), cvPointTo32f(param->shear), param->cap_scale_factor);

		status->hit_point = cvPoint(x, y);
	}
	else if (status->resize_watershed)
	{
		param->circle.width = (int)cvPointNorm(cvPoint(param->circle.x, param->circle.y), cvPoint(x, y));
		param->rect = cvShowImageAndWatershed(param->w_name, param->img_display, param->circle);
		cvShowCroppedImage(param->miniw_name, param->img_src, cvRect32fFromRect(cvScaleRect(param->rect, 1 / param->scale_factor), param->rotate), cvPointTo32f(param->shear), param->cap_scale_factor);
	}
	else
	{
		if (status->move_rect)
		{
			CvPoint move = cvPoint(x - status->hit_point.x, y - status->hit_point.y);
			param->rect.x += move.x;
			param->rect.y += move.y;
		}
		if (status->resize_rect_left)
		{
			int move_x = x - status->hit_point.x;
			param->rect.x += move_x;
			param->rect.width -= move_x;
		}
		else if (status->resize_rect_right)
		{
			int move_x = x - status->hit_point.x;
			param->rect.width += move_x;
		}
		if (status->resize_rect_top)
		{
			int move_y = y - status->hit_point.y;
			param->rect.y += move_y;
			param->rect.height -= move_y;
		}
		else if (status->resize_rect_bottom)
		{
			int move_y = y - status->hit_point.y;
			param->rect.height += move_y;
		}

		// assure width is positive
		if (param->rect.width <= 0)
		{
			param->rect.x += param->rect.width;
			param->rect.width *= -1;
			bool tmp = status->resize_rect_right;
			status->resize_rect_right = status->resize_rect_left;
			status->resize_rect_left = tmp;
		}
		// assure height is positive
		if (param->rect.height <= 0)
		{
			param->rect.y += param->rect.height;
			param->rect.height *= -1;
			bool tmp = status->resize_rect_top;
			status->resize_rect_top = status->resize_rect_bottom;
			status->resize_rect_bottom = tmp;
		}

		cvShowImageAndRectangle(param->w_name, param->img_display, cvRect32fFromRect(param->rect, param->rotate), cvPointTo32f(param->shear));
		cvShowCroppedImage(param->miniw_name, param->img_src, cvRect32fFromRect(cvScaleRect(param->rect, 1 / param->scale_factor), param->rotate), cvPointTo32f(param->shear), param->cap_scale_factor);
		status->hit_point = cvPoint(x, y);
	}
}

#endif // __ACTIONS_HPP__
