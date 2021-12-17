/*
 * stm32uikit.c
 *
 *  Created on: Dec 17, 2021
 *      Author: k-omura
 */

#include "stm32uikit.h"

#define TOUCH_Z_MIN 300

struct stm32uikit_param_t{
	uint16_t x0;
	uint16_t width;
};
struct stm32uikit_param_t stm32uikit_param = {10, 220};

typedef struct {
	uint8_t in;
	uint8_t out;
	uint8_t cont;
	uint16_t height;
} componentParam_t;

componentParam_t col_string = {0xff, 0xff, 0xff, 30};
componentParam_t col_progress = {0x14, 0x58, 0x3d, 6};
componentParam_t col_button = {0x14, 0x3d, 0x58, 30};
componentParam_t col_pin = {0x96, 0xff, 0xba, 7};

void stm32uikit_setAnchor(uint16_t _x0, uint16_t _width){
	stm32uikit_param.x0 = _x0;
	stm32uikit_param.width = _width;
}

void stm32uikit_componentSetgtings(uint8_t _col_in, uint8_t _col_out, uint8_t _col_cont, uint16_t _height, uint8_t _component){
	componentParam_t temp = {_col_in, _col_out, _col_cont, _height};
	switch(_component){
		case SET_STRING:
			col_string = temp;
			break;
		case SET_PROGRESS:
			col_progress = temp;
			break;
		case SET_BUTTON:
			col_button = temp;
			break;
		case SET_PIN:
			col_pin = temp;
			break;
	}
}

void stm32uikit_roundProgress(uint16_t _y0, uint16_t _val1000){
	uint16_t x1 = stm32uikit_param.x0 + stm32uikit_param.width;
	uint16_t y1 = _y0 + col_progress.height;
	if((stm32uikit_param.width < col_progress.height) || (stm32uikit_param.width < 0) || (col_progress.height < 0)){
		return;
	}

	uint16_t round = col_progress.height / 2;
	bitmap_roundrect(stm32uikit_param.x0, _y0, x1, y1, round, 1, col_progress.out);
	bitmap_fillroundrect(stm32uikit_param.x0 + 1, _y0 + 1, x1 - 1, y1 - 1, round - 1, col_progress.in);
	bitmap_fillroundrect(stm32uikit_param.x0 + 1, _y0 + 1, (stm32uikit_param.x0 + col_progress.height - 1 + ((stm32uikit_param.width - 2 - col_progress.height) * _val1000 / 1000)), y1 - 1, round - 1, col_progress.cont);
}

void stm32uikit_rectProgress(uint16_t _y0, uint16_t _val1000){
	uint16_t x1 = stm32uikit_param.x0 + stm32uikit_param.width;
	uint16_t y1 = _y0 + col_progress.height;
	if((stm32uikit_param.width < col_progress.height) || (stm32uikit_param.width < 0) || (col_progress.height < 0)){
		return;
	}

	bitmap_rect(stm32uikit_param.x0, _y0, x1, y1, 1, col_progress.out);
	bitmap_fillrect(stm32uikit_param.x0 + 1, _y0 + 1, x1 - 1, y1 - 1, col_progress.in);
	bitmap_fillrect(stm32uikit_param.x0 + 1, _y0 + 1, (stm32uikit_param.x0 + 1 + ((stm32uikit_param.width - 2) * _val1000 / 1000)), y1 - 1, col_progress.cont);
}

void stm32uikit_sllideBar(Coordinate_t _touch, uint16_t _y0, uint16_t *_val1000){
	uint16_t progress_radius = (col_progress.height / 2);
	uint16_t center_y = _y0 + progress_radius;

	stm32uikit_roundProgress(_y0, (*_val1000));
	bitmap_fillcircle((stm32uikit_param.x0 + progress_radius + ((stm32uikit_param.width - col_progress.height) * (*_val1000) / 1000)), center_y, col_pin.height, 0, col_pin.in);
	bitmap_circle((stm32uikit_param.x0 + progress_radius + ((stm32uikit_param.width - col_progress.height) * (*_val1000) / 1000)), center_y, col_pin.height, 0, 1, col_pin.out);

	if(TOUCH_Z_MIN > _touch.z){
		return;
	}
	if((_touch.x < stm32uikit_param.x0) || (_touch.x > (stm32uikit_param.x0 + stm32uikit_param.width))){
		return;
	}
	if((_touch.y < (center_y - col_pin.height)) || (_touch.y > (center_y + col_pin.height))){
		return;
	}

	*_val1000 = 1000 * (_touch.x - stm32uikit_param.x0) / (stm32uikit_param.width);
}

uint8_t stm32uikit_roundButton(Coordinate_t _touch, uint16_t _y0, uint8_t val){
	uint8_t col_inside = (val == 1) ? (col_button.cont) : (col_button.in);
	bitmap_roundrect(stm32uikit_param.x0, _y0, stm32uikit_param.x0 + stm32uikit_param.width, _y0 + col_button.height, 8, 1, col_button.out);
	bitmap_fillroundrect(stm32uikit_param.x0 + 1, _y0 + 1, stm32uikit_param.x0 + stm32uikit_param.width - 1, _y0 + col_button.height - 1, 7, col_inside);


	if(TOUCH_Z_MIN > _touch.z){
		return (val == 1) ? (2) : (0);
	}
	if((_touch.x < stm32uikit_param.x0) || (_touch.x > (stm32uikit_param.x0 + stm32uikit_param.width))){
		return (val == 1) ? (2) : (0);
	}
	if((_touch.y < _y0) || (_touch.y > (_y0 + col_button.height))){
		return (val == 1) ? (2) : (0);
	}

	return 1;
}
