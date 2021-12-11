/*
 * bitmap.c
 *
 *  Created on: Dec. 8, 2021
 *      Author: k-omura
 */

#include "bitmap.h"

struct bitmap_param_t bitmap_param = {0, 0, 0, 0};
uint16_t term_line = 0;

void bitmap_setparam(uint16_t _x_max, uint16_t _y_max, uint8_t _color, uint8_t *_bitmap){
	bitmap_param.width = _x_max;
	bitmap_param.height = _y_max;
	bitmap_param.background = _color;
	bitmap_param.bitmap = _bitmap;
}

void bitmap_clear(){
	if(bitmap_param.width == 0){
		return;
	}
	term_line = 0;

	uint32_t pixels = bitmap_param.width * bitmap_param.height;
	for(uint32_t i = 0; i < pixels; i++){
		bitmap_param.bitmap[i] = bitmap_param.background;
	}
}

void bitmap_pixel(uint16_t _x, uint16_t _y, uint8_t _color) {
	if(bitmap_param.width == 0){
		return;
	}

	bitmap_param.bitmap[(bitmap_param.width * _y) + _x] = _color;
}

/* Bresenham's line algorithm */
void bitmap_line(uint16_t _x0, uint16_t _y0, uint16_t _x1, uint16_t _y1, uint8_t _color) {
	if(bitmap_param.width == 0){
		return;
	}

	int16_t dx = (_x1 > _x0) ? (_x1 - _x0) : (_x0 - _x1);
	int16_t dy = (_y1 > _y0) ? (_y1 - _y0) : (_y0 - _y1);
	int16_t sx, sy, err, e2;

	if (_x0 < _x1) {
		sx = 1;
	} else {
		sx = -1;
	}
	if (_y0 < _y1) {
		sy = 1;
	} else {
		sy = -1;
	}
	err = dx - dy;

	while (1) {
		bitmap_pixel(_x0, _y0, _color);
		if ((_x0 == _x1) && (_y0 == _y1)) {
			break;
		}
		e2 = 2 * err;
		if (e2 > -dy) {
			err -= dy;
			_x0 += sx;
		}
		if (e2 < dx) {
			err += dx;
			_y0 += sy;
		}
	}
}

void bitmap_bezier(uint16_t _x0, uint16_t _y0, uint16_t _x1, uint16_t _y1, uint16_t _x2, uint16_t _y2, uint8_t _color){
	int16_t diff_x0, diff_y0, diff_x1, diff_y1;
	diff_x0 = _x0;
	diff_y0 = _y0;

    for (double t = 0; t <= 1; t += 0.2) {
    	diff_x1 = (int16_t)((1 - t) * (1 - t) * _x0 + 2 * t * (1 - t) * _x1 + t * t * _x2);
    	diff_y1 = (int16_t)((1 - t) * (1 - t) * _y0 + 2 * t * (1 - t) * _y1 + t * t * _y2);

		bitmap_line(diff_x0, diff_y0, diff_x1, diff_y1, _color);
		diff_x0 = diff_x1;
		diff_y0 = diff_y1;
    }
}

void bitmap_rect(uint16_t _x0, uint16_t _y0, uint16_t _x1, uint16_t _y1, uint16_t _width, uint8_t _color) {
	if(bitmap_param.width == 0){
		return;
	}

	for(uint16_t i = 0; i < _width; i++){
		for(uint16_t y = _y0; y <= _y1; y++){
			bitmap_pixel(_x0 + i, y, _color);
			bitmap_pixel(_x1 - i, y, _color);
		}
		for(uint16_t x = _x0; x <= _x1; x++){
			bitmap_pixel(x, _y0 + i, _color);
			bitmap_pixel(x, _y1 - i, _color);
		}
	}
}

void bitmap_fillrect(uint16_t _x0, uint16_t _y0, uint16_t _x1, uint16_t _y1, uint8_t _color) {
	if(bitmap_param.width == 0){
		return;
	}

	for(uint16_t x = _x0; x <= _x1; x++){
		for(uint16_t y = _y0; y <= _y1; y++){
			bitmap_pixel(x, y, _color);
		}
	}
}

void bitmap_roundrect(uint16_t _x0, uint16_t _y0, uint16_t _x1, uint16_t _y1, uint16_t _round, uint16_t _width, uint8_t _color) {
	if(bitmap_param.width == 0){
		return;
	}
	if(_round < 5){
		bitmap_rect(_x0, _y0, _x1, _y1, _width, _color);
		return;
	}

	if(((_x1 - _x0) < (2 * _round)) || ((_y1 - _y0) < (2 * _round))){
		return;
	}

	for(uint16_t i = 0; i < _width; i++){
		for(uint16_t y = (_y0 + _round); y <= (_y1 - _round); y++){
			bitmap_pixel(_x0 + i, y, _color);
			bitmap_pixel(_x1 - i, y, _color);
		}
		for(uint16_t x = (_x0 + _round); x <= (_x1 - _round); x++){
			bitmap_pixel(x, _y0 + i, _color);
			bitmap_pixel(x, _y1 - i, _color);
		}

		bitmap_bezier(_x0 + i, _y0 + _round, _x0 + i, _y0 + i, _x0 + _round, _y0 + i, _color);
		bitmap_bezier(_x0 + i, _y1 - _round, _x0 + i, _y1 - i, _x0 + _round, _y1 - i, _color);
		bitmap_bezier(_x1 - _round, _y0 + i, _x1 - i, _y0 + i, _x1 - i, _y0 + _round, _color);
		bitmap_bezier(_x1 - _round, _y1 - i, _x1 - i, _y1 - i, _x1 - i, _y1 - _round, _color);
	}
}

void bitmap_fillroundrect(uint16_t _x0, uint16_t _y0, uint16_t _x1, uint16_t _y1, uint16_t _round, uint8_t _color) {
	if(bitmap_param.width == 0){
		return;
	}
}

void bitmap_stringBitmap(uint16_t _x, uint16_t _y, const char _character[], uint8_t _size, uint8_t _font, uint8_t _color) {
	if(bitmap_param.width == 0){
		return;
	}

	uint8_t c = 0;
	while (_character[c]) {
		if (_size == 0) {
			bitmap_characterBitmap5(_x + (c * 4), _y, _character[c], _color);
		} else {
			bitmap_characterBitmap8(_x + (c * 8 * _size), _y, _character[c], _size, _font, _color);
		}
		c++;
	}
}

void bitmap_characterBitmap8(uint16_t _x, uint16_t _y, char _character, uint8_t _size, uint8_t _font, uint8_t _color) {
	uint8_t index = 0;

	for (uint8_t y = 0; y < 8; y++) {
		uint16_t fontByte;
		fontByte = FONT8x8[_font][_character - 0x1F][y];

		for (uint8_t x = 0; x < 8; x++) {
			if (fontByte & 0x80) {
				if(_size == 1){
					bitmap_pixel(_x + (index % 8), _y + (index / 8), _color);
				}else{
					uint16_t _x_exp = _x + (index % 8) * _size;
					uint16_t _y_exp = _y + (index / 8) * _size;
					bitmap_fillrect(_x_exp, _y_exp, _x_exp + _size - 1, _y_exp + _size - 1, _color);
				}
			}
			fontByte = fontByte << 1;
			index++;
		}
	}
}

void bitmap_characterBitmap5(uint16_t _x, uint16_t _y, char _character, uint8_t _color) {
	uint8_t index = 0;

	for (uint8_t array = 0; array < 2; array++) {
		uint16_t fontByte;
		fontByte = FONT5x3[_character - 0x1F][array];

		for (uint8_t i = 0; i < 8; i++) {
			if (fontByte & 0x80) {
				bitmap_pixel(_x + (index % 3), _y + (index / 3), _color);
			}
			fontByte = fontByte << 1;
			index++;
		}
	}
}

void bitmap_terminal(const char _character[], uint8_t _font, uint8_t _color, uint8_t _max_line){
	if(bitmap_param.width == 0){
		return;
	}

	if(term_line >= _max_line){
		uint16_t max_y = 8 * _max_line;
		for(uint16_t y = 8; y < max_y; y++){
			for(uint16_t x = 0; x < bitmap_param.width; x++){
				bitmap_param.bitmap[(y - 8) * bitmap_param.width + x] = bitmap_param.bitmap[y * bitmap_param.width + x];
			}
		}

		for(uint16_t y = (max_y - 8); y < max_y; y++){
			for(uint16_t x = 0; x < bitmap_param.width; x++){
				bitmap_param.bitmap[y * bitmap_param.width + x] = bitmap_param.background;
			}
		}
		term_line = _max_line - 1;
	}

	uint8_t c = 0;
	while (_character[c]) {
		bitmap_characterBitmap8((c * 8), (term_line * 8), _character[c], 1, _font, _color);
		if((++c * 8 + 8) > bitmap_param.width){
			break;
		}
	}

	term_line++;
}

//--------
const unsigned char FONT8x8[][97][8] = {
	{
		{ 0x08, 0x08, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00 }, // columns, rows, num_bytes_per_char
		{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, // space 0x20
		{ 0x30, 0x78, 0x78, 0x30, 0x30, 0x00, 0x30, 0x00 }, // !
		{ 0x6C, 0x6C, 0x6C, 0x00, 0x00, 0x00, 0x00, 0x00 }, // "
		{ 0x6C, 0x6C, 0xFE, 0x6C, 0xFE, 0x6C, 0x6C, 0x00 }, // #
		//{ 0x18, 0x3E, 0x60, 0x3C, 0x06, 0x7C, 0x18, 0x00 }, // $
		{ 0x18, 0x3c, 0x66, 0x00, 0x00, 0x66, 0x3c, 0x18 }, //^_ instead of "$"
		{ 0x00, 0x63, 0x66, 0x0C, 0x18, 0x33, 0x63, 0x00 }, // %
		{ 0x1C, 0x36, 0x1C, 0x3B, 0x6E, 0x66, 0x3B, 0x00 }, // &
		{ 0x30, 0x30, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00 }, // '
		{ 0x0C, 0x18, 0x30, 0x30, 0x30, 0x18, 0x0C, 0x00 }, // (
		{ 0x30, 0x18, 0x0C, 0x0C, 0x0C, 0x18, 0x30, 0x00 }, // )
		{ 0x00, 0x66, 0x3C, 0xFF, 0x3C, 0x66, 0x00, 0x00 }, // *
		{ 0x00, 0x30, 0x30, 0xFC, 0x30, 0x30, 0x00, 0x00 }, // +
		{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x30 }, // ,
		{ 0x00, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x00, 0x00 }, // -
		{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00 }, // .
		{ 0x03, 0x06, 0x0C, 0x18, 0x30, 0x60, 0x40, 0x00 }, // / (forward slash)
		{ 0x3E, 0x63, 0x63, 0x6B, 0x63, 0x63, 0x3E, 0x00 }, // 0 0x30
		{ 0x18, 0x38, 0x58, 0x18, 0x18, 0x18, 0x7E, 0x00 }, // 1
		{ 0x3C, 0x66, 0x06, 0x1C, 0x30, 0x66, 0x7E, 0x00 }, // 2
		{ 0x3C, 0x66, 0x06, 0x1C, 0x06, 0x66, 0x3C, 0x00 }, // 3
		{ 0x0E, 0x1E, 0x36, 0x66, 0x7F, 0x06, 0x0F, 0x00 }, // 4
		{ 0x7E, 0x60, 0x7C, 0x06, 0x06, 0x66, 0x3C, 0x00 }, // 5
		{ 0x1C, 0x30, 0x60, 0x7C, 0x66, 0x66, 0x3C, 0x00 }, // 6
		{ 0x7E, 0x66, 0x06, 0x0C, 0x18, 0x18, 0x18, 0x00 }, // 7
		{ 0x3C, 0x66, 0x66, 0x3C, 0x66, 0x66, 0x3C, 0x00 }, // 8
		{ 0x3C, 0x66, 0x66, 0x3E, 0x06, 0x0C, 0x38, 0x00 }, // 9
		{ 0x00, 0x18, 0x18, 0x00, 0x00, 0x18, 0x18, 0x00 }, // :
		{ 0x00, 0x18, 0x18, 0x00, 0x00, 0x18, 0x18, 0x30 }, // ;
		{ 0x0C, 0x18, 0x30, 0x60, 0x30, 0x18, 0x0C, 0x00 }, // <
		{ 0x00, 0x00, 0x7E, 0x00, 0x00, 0x7E, 0x00, 0x00 }, // =
		{ 0x30, 0x18, 0x0C, 0x06, 0x0C, 0x18, 0x30, 0x00 }, // >
		{ 0x3C, 0x66, 0x06, 0x0C, 0x18, 0x00, 0x18, 0x00 }, // ?
		//{ 0x3E, 0x63, 0x6F, 0x69, 0x6F, 0x60, 0x3E, 0x00 }, // @ 0x40
		{ 0x00, 0x24, 0x66, 0xC3, 0xC3, 0x66, 0x24, 0x00 }, //<> instead of "@"
		{ 0x18, 0x3C, 0x66, 0x66, 0x7E, 0x66, 0x66, 0x00 }, // A
		{ 0x7E, 0x33, 0x33, 0x3E, 0x33, 0x33, 0x7E, 0x00 }, // B
		{ 0x1E, 0x33, 0x60, 0x60, 0x60, 0x33, 0x1E, 0x00 }, // C
		{ 0x7C, 0x36, 0x33, 0x33, 0x33, 0x36, 0x7C, 0x00 }, // D
		{ 0x7F, 0x31, 0x34, 0x3C, 0x34, 0x31, 0x7F, 0x00 }, // E
		{ 0x7F, 0x31, 0x34, 0x3C, 0x34, 0x30, 0x78, 0x00 }, // F
		{ 0x1E, 0x33, 0x60, 0x60, 0x67, 0x33, 0x1F, 0x00 }, // G
		{ 0x66, 0x66, 0x66, 0x7E, 0x66, 0x66, 0x66, 0x00 }, // H
		{ 0x3C, 0x18, 0x18, 0x18, 0x18, 0x18, 0x3C, 0x00 }, // I
		{ 0x0F, 0x06, 0x06, 0x06, 0x66, 0x66, 0x3C, 0x00 }, // J
		{ 0x73, 0x33, 0x36, 0x3C, 0x36, 0x33, 0x73, 0x00 }, // K
		{ 0x78, 0x30, 0x30, 0x30, 0x31, 0x33, 0x7F, 0x00 }, // L
		{ 0x63, 0x77, 0x7F, 0x7F, 0x6B, 0x63, 0x63, 0x00 }, // M
		{ 0x63, 0x73, 0x7B, 0x6F, 0x67, 0x63, 0x63, 0x00 }, // N
		{ 0x3E, 0x63, 0x63, 0x63, 0x63, 0x63, 0x3E, 0x00 }, // O
		{ 0x7E, 0x33, 0x33, 0x3E, 0x30, 0x30, 0x78, 0x00 }, // P 0x50
		{ 0x3C, 0x66, 0x66, 0x66, 0x6E, 0x3C, 0x0E, 0x00 }, // Q
		{ 0x7E, 0x33, 0x33, 0x3E, 0x36, 0x33, 0x73, 0x00 }, // R
		{ 0x3C, 0x66, 0x30, 0x18, 0x0C, 0x66, 0x3C, 0x00 }, // S
		{ 0x7E, 0x5A, 0x18, 0x18, 0x18, 0x18, 0x3C, 0x00 }, // T
		{ 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x7E, 0x00 }, // U
		{ 0x66, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x18, 0x00 }, // V
		{ 0x63, 0x63, 0x63, 0x6B, 0x7F, 0x77, 0x63, 0x00 }, // W
		{ 0x63, 0x63, 0x36, 0x1C, 0x1C, 0x36, 0x63, 0x00 }, // X
		{ 0x66, 0x66, 0x66, 0x3C, 0x18, 0x18, 0x3C, 0x00 }, // Y
		{ 0x7F, 0x63, 0x46, 0x0C, 0x19, 0x33, 0x7F, 0x00 }, // Z
		{ 0x3C, 0x30, 0x30, 0x30, 0x30, 0x30, 0x3C, 0x00 }, // [
		{ 0x60, 0x30, 0x18, 0x0C, 0x06, 0x03, 0x01, 0x00 }, // \ (back slash)
		{ 0x3C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x3C, 0x00 }, // ]
		{ 0x08, 0x1C, 0x36, 0x63, 0x00, 0x00, 0x00, 0x00 }, // ^
		{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF }, // _
		{ 0x18, 0x18, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00 }, // ` 0x60
		{ 0x00, 0x00, 0x3C, 0x06, 0x3E, 0x66, 0x3B, 0x00 }, // a
		{ 0x70, 0x30, 0x3E, 0x33, 0x33, 0x33, 0x6E, 0x00 }, // b
		{ 0x00, 0x00, 0x3C, 0x66, 0x60, 0x66, 0x3C, 0x00 }, // c
		{ 0x0E, 0x06, 0x3E, 0x66, 0x66, 0x66, 0x3B, 0x00 }, // d
		{ 0x00, 0x00, 0x3C, 0x66, 0x7E, 0x60, 0x3C, 0x00 }, // e
		{ 0x1C, 0x36, 0x30, 0x78, 0x30, 0x30, 0x78, 0x00 }, // f
		{ 0x00, 0x00, 0x3B, 0x66, 0x66, 0x3E, 0x06, 0x7C }, // g
		{ 0x70, 0x30, 0x36, 0x3B, 0x33, 0x33, 0x73, 0x00 }, // h
		{ 0x18, 0x00, 0x38, 0x18, 0x18, 0x18, 0x3C, 0x00 }, // i
		{ 0x06, 0x00, 0x06, 0x06, 0x06, 0x66, 0x66, 0x3C }, // j
		{ 0x70, 0x30, 0x33, 0x36, 0x3C, 0x36, 0x73, 0x00 }, // k
		{ 0x38, 0x18, 0x18, 0x18, 0x18, 0x18, 0x3C, 0x00 }, // l
		{ 0x00, 0x00, 0x66, 0x7F, 0x7F, 0x6B, 0x63, 0x00 }, // m
		{ 0x00, 0x00, 0x7C, 0x66, 0x66, 0x66, 0x66, 0x00 }, // n
		{ 0x00, 0x00, 0x3C, 0x66, 0x66, 0x66, 0x3C, 0x00 }, // o
		{ 0x00, 0x00, 0x6E, 0x33, 0x33, 0x3E, 0x30, 0x78 }, // p
		{ 0x00, 0x00, 0x3B, 0x66, 0x66, 0x3E, 0x06, 0x0F }, // q
		{ 0x00, 0x00, 0x6E, 0x3B, 0x33, 0x30, 0x78, 0x00 }, // r
		{ 0x00, 0x00, 0x3E, 0x60, 0x3C, 0x06, 0x7C, 0x00 }, // s
		{ 0x08, 0x18, 0x3E, 0x18, 0x18, 0x1A, 0x0C, 0x00 }, // t
		{ 0x00, 0x00, 0x66, 0x66, 0x66, 0x66, 0x3B, 0x00 }, // u
		{ 0x00, 0x00, 0x66, 0x66, 0x66, 0x3C, 0x18, 0x00 }, // v
		{ 0x00, 0x00, 0x63, 0x6B, 0x7F, 0x7F, 0x36, 0x00 }, // w
		{ 0x00, 0x00, 0x63, 0x36, 0x1C, 0x36, 0x63, 0x00 }, // x
		{ 0x00, 0x00, 0x66, 0x66, 0x66, 0x3E, 0x06, 0x7C }, // y
		{ 0x00, 0x00, 0x7E, 0x4C, 0x18, 0x32, 0x7E, 0x00 }, // z
		{ 0x0E, 0x18, 0x18, 0x70, 0x18, 0x18, 0x0E, 0x00 }, // {
		{ 0x0C, 0x0C, 0x0C, 0x00, 0x0C, 0x0C, 0x0C, 0x00 }, // |
		{ 0x70, 0x18, 0x18, 0x0E, 0x18, 0x18, 0x70, 0x00 }, // }
		{ 0x3B, 0x6E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, // ~
		{ 0x1C, 0x36, 0x36, 0x1C, 0x00, 0x00, 0x00, 0x00 } // DEL
		//{ 0xcc, 0xde, 0x33, 0x30, 0x30, 0x33, 0x1e, 0x0c }, //℃ instead of ""
	},{
			{ 0x08, 0x08, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00 }, // columns, rows, num_bytes_per_char
			{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, // space 0x20
			{0b00000000,0b00001000,0b00001000,0b00001000,0b00001000,0b00000000,0b00001000,0b00000000}, //!
			{0b00000000,0b00010100,0b00010100,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000}, //"
			{0b00000000,0b00010100,0b00111110,0b00010100,0b00010100,0b00111110,0b00010100,0b00000000}, //#
			{0b00001000,0b00011100,0b00101010,0b00011000,0b00001100,0b00101010,0b00011100,0b00001000}, //$
			{0b00100001,0b01010010,0b00100100,0b00001000,0b00010010,0b00100101,0b01000010,0b00000000}, //%
			{0b00011000,0b00100100,0b00101000,0b00010000,0b00101010,0b00100100,0b00011010,0b00000000}, //&
			{0b00000000,0b00001000,0b00001000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000}, //'
			{0b00000100,0b00001000,0b00001000,0b00010000,0b00010000,0b00001000,0b00001000,0b00000100}, //(
			{0b00100000,0b00010000,0b00010000,0b00001000,0b00001000,0b00010000,0b00010000,0b00100000}, //)
			{0b00000000,0b01010100,0b01010100,0b00111000,0b01010100,0b01010100,0b00000000,0b00000000}, // *
			{0b00000000,0b00001000,0b00001000,0b00111110,0b00001000,0b00001000,0b00000000,0b00000000}, //+
			{0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00010000,0b00010000,0b00100000}, //,
			{0b00000000,0b00000000,0b00000000,0b00111110,0b00000000,0b00000000,0b00000000,0b00000000}, //-
			{0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00010000,0b00000000}, //.
			{0b00000010,0b00000010,0b00000100,0b00000100,0b00001000,0b00001000,0b00010000,0b00010000}, ///(forward slash)
			{0b00011100,0b00100010,0b00100010,0b00100010,0b00100010,0b00100010,0b00011100,0b00000000}, //0 0x30
			{0b00001000,0b00011000,0b00001000,0b00001000,0b00001000,0b00001000,0b00001000,0b00000000}, //1
			{0b00011100,0b00100010,0b00000010,0b00000100,0b00001000,0b00010000,0b00111110,0b00000000}, //2
			{0b00011100,0b00100010,0b00000010,0b00001100,0b00000010,0b00100010,0b00011100,0b00000000}, //3
			{0b00000100,0b00001100,0b00010100,0b00100100,0b00111110,0b00000100,0b00000100,0b00000000}, //4
			{0b00111110,0b00100000,0b00111100,0b00000010,0b00000010,0b00100010,0b00011100,0b00000000}, //5
			{0b00011100,0b00100010,0b00100000,0b00111100,0b00100010,0b00100010,0b00011100,0b00000000}, //6
			{0b00111110,0b00000010,0b00000100,0b00000100,0b00001000,0b00001000,0b00001000,0b00000000}, //7
			{0b00011100,0b00100010,0b00100010,0b00011100,0b00100010,0b00100010,0b00011100,0b00000000}, //8
			{0b00011100,0b00100010,0b00100010,0b00011110,0b00000010,0b00100010,0b00011100,0b00000000}, //9
			{0b00000000,0b00001000,0b00000000,0b00000000,0b00000000,0b00001000,0b00000000,0b00000000}, //:
			{0b00000000,0b00001000,0b00000000,0b00000000,0b00000000,0b00001000,0b00010000,0b00000000}, //;
			{0b00000100,0b00001000,0b00010000,0b00100000,0b00010000,0b00001000,0b00000100,0b00000000}, //<
			{0b00000000,0b00000000,0b00111100,0b00000000,0b00000000,0b00111100,0b00000000,0b00000000}, //=
			{0b00100000,0b00010000,0b00001000,0b00000100,0b00001000,0b00010000,0b00100000,0b00000000}, //>
			{0b00011100,0b00100010,0b00000010,0b00000100,0b00001000,0b00000000,0b00001000,0b00000000}, //?
			{0b00011100,0b00100010,0b00101110,0b00110100,0b00110100,0b00101010,0b00011100,0b00000000}, //@ 0x40
			{0b00011100,0b00100010,0b00100010,0b00111110,0b00100010,0b00100010,0b00100010,0b00000000}, //A
			{0b00111100,0b00100010,0b00100010,0b00111100,0b00100010,0b00100010,0b00111100,0b00000000}, //B
			{0b00011100,0b00100010,0b00100000,0b00100000,0b00100000,0b00100010,0b00011100,0b00000000}, //C
			{0b00111100,0b00100010,0b00100010,0b00100010,0b00100010,0b00100010,0b00111100,0b00000000}, //D
			{0b00111110,0b00100000,0b00100000,0b00111100,0b00100000,0b00100000,0b00111110,0b00000000}, //E
			{0b00111110,0b00100000,0b00100000,0b00111100,0b00100000,0b00100000,0b00100000,0b00000000}, //F
			{0b00011100,0b00100010,0b00100000,0b00100110,0b00100010,0b00100010,0b00011100,0b00000000}, //G
			{0b00100010,0b00100010,0b00100010,0b00111110,0b00100010,0b00100010,0b00100010,0b00000000}, //H
			{0b00001000,0b00001000,0b00001000,0b00001000,0b00001000,0b00001000,0b00001000,0b00000000}, //I
			{0b00000010,0b00000010,0b00000010,0b00000010,0b00100010,0b00100010,0b00011100,0b00000000}, //J
			{0b00100010,0b00100100,0b00101000,0b00110000,0b00101000,0b00100100,0b00100010,0b00000000}, //K
			{0b00100000,0b00100000,0b00100000,0b00100000,0b00100000,0b00100000,0b00111110,0b00000000}, //L
			{0b00100010,0b00110110,0b00101010,0b00100010,0b00100010,0b00100010,0b00100010,0b00000000}, //M
			{0b00100010,0b00110010,0b00101010,0b00100110,0b00100010,0b00100010,0b00100010,0b00000000}, //N
			{0b00011100,0b00100010,0b00100010,0b00100010,0b00100010,0b00100010,0b00011100,0b00000000}, //O
			{0b00111100,0b00100010,0b00100010,0b00111100,0b00100000,0b00100000,0b00100000,0b00000000}, //P 0x50
			{0b00011100,0b00100010,0b00100010,0b00100010,0b00100010,0b00100010,0b00011100,0b00000110}, //Q
			{0b00111100,0b00100010,0b00100010,0b00111100,0b00100010,0b00100010,0b00100010,0b00000000}, //R
			{0b00011100,0b00100010,0b00100000,0b00011100,0b00000010,0b00100010,0b00011100,0b00000000}, //S
			{0b00111110,0b00001000,0b00001000,0b00001000,0b00001000,0b00001000,0b00001000,0b00000000}, //T
			{0b00100010,0b00100010,0b00100010,0b00100010,0b00100010,0b00100010,0b00011100,0b00000000}, //U
			{0b00100010,0b00100010,0b00100010,0b00100010,0b00100010,0b00010100,0b00001000,0b00000000}, //V
			{0b00100010,0b00100010,0b00100010,0b00100010,0b00101010,0b00110110,0b00100010,0b00000000}, //W
			{0b00100010,0b00010100,0b00001000,0b00001000,0b00001000,0b00010100,0b00100010,0b00000000}, //X
			{0b00100010,0b00010100,0b00001000,0b00001000,0b00001000,0b00001000,0b00001000,0b00000000}, //Y
			{0b00111110,0b00000010,0b00000100,0b00001000,0b00010000,0b00100000,0b00111110,0b00000000}, //Z
			{0b00001100,0b00001000,0b00001000,0b00001000,0b00001000,0b00001000,0b00001100,0b00000000}, //[
			{0b00010000,0b00010000,0b00001000,0b00001000,0b00000100,0b00000100,0b00000010,0b00000010}, //\ (back slash)
			{0b00110000,0b00010000,0b00010000,0b00010000,0b00010000,0b00010000,0b00110000,0b00000000}, //]
			{0b00001000,0b00010100,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000}, //^
			{0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00111110,0b00000000}, //_
			{0b00001000,0b00000100,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000}, //` 0x60
			{0b00000000,0b00000000,0b00011110,0b00100010,0b00100010,0b00100110,0b00011010,0b00000000}, //a
			{0b00100000,0b00100000,0b00111100,0b00100010,0b00100010,0b00100010,0b00011100,0b00000000}, //b
			{0b00000000,0b00000000,0b00011100,0b00100010,0b00100000,0b00100000,0b00011110,0b00000000}, //c
			{0b00000010,0b00000010,0b00011110,0b00100010,0b00100010,0b00100010,0b00011100,0b00000000}, //d
			{0b00000000,0b00000000,0b00011100,0b00100010,0b00111100,0b00100000,0b00011110,0b00000000}, //e
			{0b00000000,0b00000110,0b00001000,0b00011100,0b00001000,0b00001000,0b00001000,0b00000000}, //f
			{0b00000000,0b00000000,0b00011100,0b00100010,0b00100010,0b00011110,0b00000010,0b00011100}, //g
			{0b00010000,0b00010000,0b00010000,0b00011100,0b00010010,0b00010010,0b00010010,0b00000000}, //h
			{0b00000000,0b00001000,0b00000000,0b00001000,0b00001000,0b00001000,0b00001000,0b00000000}, //i
			{0b00000000,0b00000100,0b00000000,0b00000100,0b00000100,0b00000100,0b00000100,0b00011000}, //j
			{0b00010000,0b00010000,0b00010010,0b00010100,0b00011000,0b00010100,0b00010010,0b00000000}, //k
			{0b00001000,0b00001000,0b00001000,0b00001000,0b00001000,0b00001000,0b00001000,0b00000000}, //l
			{0b00000000,0b00000000,0b00111100,0b00101010,0b00101010,0b00101010,0b00101010,0b00000000}, //m
			{0b00000000,0b00000000,0b00101100,0b00110010,0b00100010,0b00100010,0b00100010,0b00000000}, //n
			{0b00000000,0b00000000,0b00011100,0b00100010,0b00100010,0b00100010,0b00011100,0b00000000}, //o
			{0b00000000,0b00011100,0b00100010,0b00100010,0b00100010,0b00111100,0b00100000,0b00100000}, //p
			{0b00000000,0b00011100,0b00100010,0b00100010,0b00100010,0b00011110,0b00000010,0b00000010}, //q
			{0b00000000,0b00000000,0b00101100,0b00110010,0b00100000,0b00100000,0b00100000,0b00000000}, //r
			{0b00000000,0b00000000,0b00001110,0b00010000,0b00001100,0b00000010,0b00011100,0b00000000}, //s
			{0b00000000,0b00001000,0b00011100,0b00001000,0b00001000,0b00001000,0b00001100,0b00000000}, //t
			{0b00000000,0b00000000,0b00100010,0b00100010,0b00100010,0b00100110,0b00011010,0b00000000}, //u
			{0b00000000,0b00000000,0b00100010,0b00100010,0b00100010,0b00010100,0b00001000,0b00000000}, //v
			{0b00000000,0b00000000,0b00101010,0b00101010,0b00101010,0b00101010,0b00010100,0b00000000}, //w
			{0b00000000,0b00000000,0b00100010,0b00010100,0b00001000,0b00010100,0b00100010,0b00000000}, //x
			{0b00000000,0b00000000,0b00100010,0b00100010,0b00100010,0b00011110,0b00000010,0b00011100}, //y
			{0b00000000,0b00000000,0b00111110,0b00000100,0b00001000,0b00010000,0b00111110,0b00000000}, //z
			{0b00000100,0b00001000,0b00001000,0b00011000,0b00001000,0b00001000,0b00000100,0b00000000}, //{
			{0b00001000,0b00001000,0b00001000,0b00001000,0b00001000,0b00001000,0b00001000,0b00001000}, //|
			{0b00010000,0b00001000,0b00001000,0b00001100,0b00001000,0b00001000,0b00010000,0b00000000}, //}
			{0b00000000,0b00000000,0b00000100,0b00101010,0b00010000,0b00000000,0b00000000,0b00000000}, //~
			{ 0x1C, 0x36, 0x36, 0x1C, 0x00, 0x00, 0x00, 0x00 } // DEL
	}/*,{
			{ 0x08, 0x08, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00 }, // columns, rows, num_bytes_per_char
			{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},//
			{0x00,0x08,0x08,0x08,0x08,0x08,0x00,0x08},//!
			{0x00,0x28,0x28,0x00,0x00,0x00,0x00,0x00},//"
			{0x00,0x14,0x14,0x3e,0x14,0x3e,0x14,0x14},//#
			{0x00,0x08,0x3e,0x28,0x3e,0x0a,0x3e,0x08},//$
			{0x00,0x62,0x64,0x08,0x10,0x26,0x46,0x00},//%
			{0x00,0x3c,0x20,0x24,0x3e,0x24,0x3c,0x00},//&
			{0x00,0x20,0x20,0x00,0x00,0x00,0x00,0x00},//'
			{0x00,0x08,0x10,0x10,0x10,0x10,0x08,0x00},//(
			{0x00,0x10,0x08,0x08,0x08,0x08,0x10,0x00},//)
			{0x00,0x08,0x2a,0x1c,0x08,0x1c,0x2a,0x08},// *
			{0x00,0x00,0x08,0x08,0x3e,0x08,0x08,0x00},//+
			{0x00,0x00,0x00,0x00,0x00,0x60,0x20,0x40},//,
			{0x00,0x00,0x00,0x00,0x3e,0x00,0x00,0x00},//-
			{0x00,0x00,0x00,0x00,0x00,0x30,0x30,0x00},//.
			{0x00,0x02,0x04,0x08,0x10,0x20,0x40,0x00},///
			{0x00,0x1e,0x12,0x12,0x12,0x12,0x12,0x1e},//0
			{0x00,0x02,0x02,0x02,0x02,0x02,0x02,0x02},//1
			{0x00,0x1e,0x02,0x02,0x1e,0x10,0x10,0x1e},//2
			{0x00,0x1e,0x02,0x02,0x1e,0x02,0x02,0x1e},//3
			{0x00,0x12,0x12,0x12,0x1e,0x02,0x02,0x02},//4
			{0x00,0x1e,0x10,0x10,0x1e,0x02,0x02,0x1e},//5
			{0x00,0x1e,0x10,0x10,0x1e,0x12,0x12,0x1e},//6
			{0x00,0x1e,0x12,0x12,0x02,0x02,0x02,0x02},//7
			{0x00,0x3c,0x24,0x24,0x3c,0x24,0x24,0x3c},//8
			{0x00,0x1e,0x12,0x12,0x1e,0x02,0x02,0x1e},//9
			{0x00,0x00,0x30,0x30,0x00,0x30,0x30,0x00},//:
			{0x00,0x00,0x30,0x30,0x00,0x30,0x10,0x20},//;
			{0x00,0x04,0x08,0x10,0x20,0x10,0x08,0x04},//<
			{0x00,0x00,0x00,0x3e,0x00,0x3e,0x00,0x00},//=
			{0x00,0x10,0x08,0x04,0x02,0x04,0x08,0x10},//>
			{0x00,0x3e,0x22,0x02,0x0e,0x08,0x00,0x08},//?
			{0x00,0x3e,0x61,0x4d,0x55,0x5b,0x40,0x3e},//@
			{0x00,0x3e,0x22,0x22,0x3e,0x22,0x22,0x22},//A
			{0x00,0x3c,0x24,0x24,0x3e,0x22,0x22,0x3e},//B
			{0x00,0x3e,0x22,0x20,0x20,0x20,0x22,0x3e},//C
			{0x00,0x78,0x44,0x44,0x44,0x44,0x48,0x70},//D
			{0x00,0x3e,0x20,0x20,0x3c,0x20,0x20,0x3e},//E
			{0x00,0x3e,0x20,0x20,0x3c,0x20,0x20,0x20},//F
			{0x00,0x3e,0x20,0x20,0x26,0x22,0x22,0x3e},//G
			{0x00,0x22,0x22,0x22,0x3e,0x22,0x22,0x22},//H
			{0x00,0x3e,0x08,0x08,0x08,0x08,0x08,0x3e},//I
			{0x00,0x3e,0x04,0x04,0x04,0x04,0x24,0x3c},//J
			{0x00,0x22,0x24,0x28,0x30,0x28,0x24,0x22},//K
			{0x00,0x20,0x20,0x20,0x20,0x20,0x20,0x3e},//L
			{0x00,0x22,0x36,0x2a,0x22,0x22,0x22,0x22},//M
			{0x00,0x22,0x22,0x32,0x2a,0x26,0x22,0x22},//N
			{0x00,0x3e,0x22,0x22,0x22,0x22,0x22,0x3e},//O
			{0x00,0x7c,0x44,0x44,0x7c,0x40,0x40,0x40},//P
			{0x00,0x3e,0x22,0x22,0x22,0x2a,0x24,0x3a},//Q
			{0x00,0x3e,0x22,0x22,0x3e,0x28,0x24,0x22},//R
			{0x00,0x3e,0x22,0x20,0x3e,0x02,0x22,0x3e},//S
			{0x00,0x3e,0x08,0x08,0x08,0x08,0x08,0x08},//T
			{0x00,0x22,0x22,0x22,0x22,0x22,0x22,0x3e},//U
			{0x00,0x22,0x22,0x22,0x22,0x24,0x28,0x30},//V
			{0x00,0x22,0x22,0x22,0x2a,0x2a,0x36,0x22},//W
			{0x00,0x22,0x22,0x14,0x08,0x14,0x22,0x22},//X
			{0x00,0x22,0x22,0x14,0x08,0x08,0x08,0x08},//Y
			{0x00,0x3e,0x02,0x04,0x08,0x10,0x20,0x3e},//Z
			{0x00,0x18,0x10,0x10,0x10,0x10,0x18,0x00},//[
			{0x00,0x44,0x28,0x10,0x7c,0x10,0x7c,0x10},//¥
			{0x00,0x18,0x08,0x08,0x08,0x08,0x18,0x00},//]
			{0x00,0x10,0x28,0x00,0x00,0x00,0x00,0x00},//^
			{0x00,0x00,0x00,0x00,0x00,0x00,0x3c,0x00},//_
			{0x00,0x20,0x10,0x00,0x00,0x00,0x00,0x00},//`
			{0x00,0x00,0x00,0x1c,0x24,0x24,0x3a,0x00},//a
			{0x00,0x20,0x20,0x3c,0x24,0x24,0x3c,0x00},//b
			{0x00,0x00,0x00,0x3c,0x20,0x20,0x3c,0x00},//c
			{0x00,0x04,0x04,0x3c,0x24,0x24,0x3c,0x00},//d
			{0x00,0x00,0x00,0x3c,0x24,0x3c,0x20,0x3c},//e
			{0x00,0x1c,0x10,0x3c,0x10,0x10,0x10,0x00},//f
			{0x00,0x00,0x00,0x3c,0x24,0x3c,0x04,0x3c},//g
			{0x00,0x40,0x40,0x78,0x48,0x48,0x48,0x00},//h
			{0x00,0x08,0x00,0x08,0x08,0x08,0x08,0x00},//i
			{0x00,0x08,0x00,0x08,0x08,0x08,0x28,0x38},//j
			{0x00,0x20,0x20,0x28,0x30,0x28,0x24,0x00},//k
			{0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x00},//l
			{0x00,0x00,0x00,0x3e,0x2a,0x2a,0x2a,0x00},//m
			{0x00,0x00,0x00,0x3c,0x24,0x24,0x24,0x00},//n
			{0x00,0x00,0x00,0x3c,0x24,0x24,0x3c,0x00},//o
			{0x00,0x00,0x00,0x3c,0x24,0x3c,0x20,0x20},//p
			{0x00,0x00,0x00,0x3c,0x24,0x3c,0x04,0x04},//q
			{0x00,0x00,0x00,0x3c,0x24,0x20,0x20,0x20},//r
			{0x00,0x00,0x00,0x3c,0x20,0x3c,0x04,0x3c},//s
			{0x00,0x00,0x20,0x78,0x20,0x20,0x38,0x00},//t
			{0x00,0x00,0x00,0x24,0x24,0x24,0x3c,0x00},//u
			{0x00,0x00,0x00,0x24,0x24,0x28,0x30,0x00},//v
			{0x00,0x00,0x00,0x2a,0x2a,0x2a,0x3e,0x00},//w
			{0x00,0x00,0x00,0x22,0x14,0x08,0x14,0x22},//x
			{0x00,0x00,0x00,0x24,0x24,0x3c,0x04,0x3c},//y
			{0x00,0x00,0x00,0x3c,0x08,0x10,0x3c,0x00},//z
			{0x00,0x08,0x10,0x10,0x20,0x10,0x10,0x08},//{
			{0x00,0x08,0x08,0x08,0x08,0x08,0x08,0x08},//|
			{0x00,0x10,0x08,0x08,0x04,0x08,0x08,0x10},//}
			{0x00,0x00,0x00,0x32,0x4c,0x00,0x00,0x00},//~
			{0x00,0x3e,0x3e,0x3e,0x3e,0x3e,0x3e,0x3e},//
	},{
			{ 0x08, 0x08, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00 }, // columns, rows, num_bytes_per_char
			{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},//
			{0x3c,0x3c,0x3c,0x3c,0x3c,0x00,0x3c,0x3c},//!
			{0x00,0x28,0x28,0x00,0x00,0x00,0x00,0x00},//"
			{0x00,0x14,0x14,0x3e,0x14,0x3e,0x14,0x14},//#
			{0x00,0x08,0x3e,0x28,0x3e,0x0a,0x3e,0x08},//$
			{0x00,0x62,0x64,0x08,0x10,0x26,0x46,0x00},//%
			{0x00,0x3c,0x20,0x24,0x3e,0x24,0x3c,0x00},//&
			{0x00,0x20,0x20,0x00,0x00,0x00,0x00,0x00},//'
			{0x00,0x08,0x10,0x10,0x10,0x10,0x08,0x00},//(
			{0x00,0x10,0x08,0x08,0x08,0x08,0x10,0x00},//)
			{0x00,0x08,0x2a,0x1c,0x08,0x1c,0x2a,0x08},// *
			{0x00,0x00,0x08,0x08,0x3e,0x08,0x08,0x00},//+
			{0x00,0x00,0x00,0x00,0x00,0x60,0x20,0x40},//,
			{0x00,0x00,0x00,0x00,0x3e,0x00,0x00,0x00},//-
			{0x00,0x00,0x00,0x00,0x00,0x30,0x30,0x00},//.
			{0x00,0x02,0x04,0x08,0x10,0x20,0x40,0x00},///
			{0x7c,0xee,0xee,0xee,0xee,0xee,0xee,0x7c},//0
			{0x78,0x78,0x38,0x38,0x38,0x38,0x7c,0x7c},//1
			{0x7c,0xfe,0xce,0x1c,0x38,0x70,0xfe,0xfe},//2
			{0x7c,0xfe,0xc6,0x3e,0x3c,0xc6,0xfe,0x7c},//3
			{0xee,0xee,0xee,0xfe,0xfe,0x0e,0x0e,0x0e},//4
			{0xfe,0xfe,0xe0,0xfc,0xfe,0x0e,0xfe,0xfc},//5
			{0x7c,0xfc,0xe0,0xfc,0xee,0xee,0xee,0x7c},//6
			{0xfe,0xee,0xee,0x0e,0x0e,0x0e,0x0e,0x0e},//7
			{0x7c,0xee,0xee,0x7c,0x7c,0xee,0xee,0x7c},//8
			{0x7c,0xee,0xee,0xee,0x7e,0x0e,0x0e,0x0e},//9
			{0x00,0x00,0x30,0x30,0x00,0x30,0x30,0x00},//:
			{0x00,0x00,0x30,0x30,0x00,0x30,0x10,0x20},//;
			{0x00,0x04,0x08,0x10,0x20,0x10,0x08,0x04},//<
			{0x00,0x00,0x00,0x3e,0x00,0x3e,0x00,0x00},//=
			{0x00,0x10,0x08,0x04,0x02,0x04,0x08,0x10},//>
			{0x7c,0xfe,0xce,0x1c,0x38,0x00,0x38,0x38},//?
			{0x00,0x3e,0x61,0x4d,0x55,0x5b,0x40,0x3e},//@
			{0x7c,0xee,0xee,0xfe,0xfe,0xee,0xee,0xee},//A
			{0xfc,0xee,0xee,0xfc,0xfc,0xee,0xee,0xfc},//B
			{0x7c,0xee,0xee,0xe0,0xe0,0xee,0xee,0x7c},//C
			{0xfc,0xee,0xee,0xee,0xee,0xee,0xee,0xfc},//D
			{0xfe,0xfe,0xe0,0xfe,0xfe,0xe0,0xfe,0xfe},//E
			{0xfe,0xfe,0xe0,0xfe,0xfe,0xe0,0xe0,0xe0},//F
			{0x7c,0xee,0xee,0xe0,0xee,0xee,0xe6,0x7c},//G
			{0xee,0xee,0xee,0xfe,0xfe,0xee,0xee,0xee},//H
			{0xfe,0xfe,0xfe,0x38,0x38,0xfe,0xfe,0xfe},//I
			{0xfe,0xfe,0xfe,0x1c,0xdc,0xdc,0xdc,0xfc},//J
			{0xee,0xee,0xec,0xf8,0xf8,0xec,0xee,0xee},//K
			{0xf0,0xf0,0xf0,0xf0,0xf0,0xfe,0xfe,0xfe},//L
			{0xc6,0xee,0xfe,0xfe,0xfe,0xfe,0xd6,0xc6},//M
			{0xe6,0xf6,0xf6,0xfe,0xfe,0xde,0xde,0xce},//N
			{0x7c,0xee,0xee,0xee,0xee,0xee,0xee,0x7c},//O
			{0xfc,0xee,0xee,0xee,0xec,0xe0,0xe0,0xe0},//P
			{0x7c,0xee,0xee,0xc6,0xda,0xdc,0xee,0x76},//Q
			{0xfc,0xee,0xee,0xec,0xe8,0xec,0xee,0xe6},//R
			{0x7c,0xfe,0xe6,0x70,0x1c,0xce,0xfe,0x7c},//S
			{0xfe,0xfe,0xfe,0x3c,0x3c,0x3c,0x3c,0x3c},//T
			{0xee,0xee,0xee,0xee,0xee,0xee,0xee,0x7c},//U
			{0xde,0xde,0xde,0xde,0xde,0xdc,0xd8,0xf0},//V
			{0xae,0xae,0xae,0xae,0xae,0xae,0xfe,0x6c},//W
			{0xee,0xee,0x6c,0x38,0x38,0x6c,0xee,0xee},//X
			{0xee,0xee,0xee,0x6e,0x3e,0xde,0xee,0x7c},//Y
			{0xfe,0xfe,0x1e,0x3c,0x78,0xf0,0xfe,0xfe},//Z
			{0x00,0x18,0x10,0x10,0x10,0x10,0x18,0x00},//[
			{0x00,0x44,0x28,0x10,0x7c,0x10,0x7c,0x10},//¥
			{0x00,0x18,0x08,0x08,0x08,0x08,0x18,0x00},//]
			{0x00,0x10,0x28,0x00,0x00,0x00,0x00,0x00},//^
			{0x00,0x00,0x00,0x00,0x00,0x00,0x3c,0x00},//_
			{0x00,0x20,0x10,0x00,0x00,0x00,0x00,0x00},//`
			{0x00,0x00,0x00,0x1c,0x24,0x24,0x3a,0x00},//a
			{0x00,0x20,0x20,0x3c,0x24,0x24,0x3c,0x00},//b
			{0x00,0x00,0x00,0x3c,0x20,0x20,0x3c,0x00},//c
			{0x00,0x04,0x04,0x3c,0x24,0x24,0x3c,0x00},//d
			{0x00,0x00,0x00,0x3c,0x24,0x3c,0x20,0x3c},//e
			{0x00,0x1c,0x10,0x3c,0x10,0x10,0x10,0x00},//f
			{0x00,0x00,0x00,0x3c,0x24,0x3c,0x04,0x3c},//g
			{0x00,0x40,0x40,0x78,0x48,0x48,0x48,0x00},//h
			{0x00,0x08,0x00,0x08,0x08,0x08,0x08,0x00},//i
			{0x00,0x08,0x00,0x08,0x08,0x08,0x28,0x38},//j
			{0x00,0x20,0x20,0x28,0x30,0x28,0x24,0x00},//k
			{0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x00},//l
			{0x00,0x00,0x00,0x3e,0x2a,0x2a,0x2a,0x00},//m
			{0x00,0x00,0x00,0x3c,0x24,0x24,0x24,0x00},//n
			{0x00,0x00,0x00,0x3c,0x24,0x24,0x3c,0x00},//o
			{0x00,0x00,0x00,0x3c,0x24,0x3c,0x20,0x20},//p
			{0x00,0x00,0x00,0x3c,0x24,0x3c,0x04,0x04},//q
			{0x00,0x00,0x00,0x3c,0x24,0x20,0x20,0x20},//r
			{0x00,0x00,0x00,0x3c,0x20,0x3c,0x04,0x3c},//s
			{0x00,0x00,0x20,0x78,0x20,0x20,0x38,0x00},//t
			{0x00,0x00,0x00,0x24,0x24,0x24,0x3c,0x00},//u
			{0x00,0x00,0x00,0x24,0x24,0x28,0x30,0x00},//v
			{0x00,0x00,0x00,0x2a,0x2a,0x2a,0x3e,0x00},//w
			{0x00,0x00,0x00,0x22,0x14,0x08,0x14,0x22},//x
			{0x00,0x00,0x00,0x24,0x24,0x3c,0x04,0x3c},//y
			{0x00,0x00,0x00,0x3c,0x08,0x10,0x3c,0x00},//z
			{0x00,0x08,0x10,0x10,0x20,0x10,0x10,0x08},//{
			{0x00,0x08,0x08,0x08,0x08,0x08,0x08,0x08},//|
			{0x00,0x10,0x08,0x08,0x04,0x08,0x08,0x10},//}
			{0x00,0x00,0x00,0x32,0x4c,0x00,0x00,0x00},//~
			{0x00,0x3e,0x3e,0x3e,0x3e,0x3e,0x3e,0x3e},//
	}:*/
};

const unsigned char FONT5x3[][2] = {
		{ 0b11110110, 0b11011110 }, // columns, rows, num_bytes_per_char
		{ 0b00000000, 0b00000000 }, // space 0x20
		{ 0b01001001, 0b00000100 }, // !
		{ 0b10110100, 0b00000000 }, // "
		{ 0b10111110, 0b11111010 }, // #
		{ 0b01111001, 0b00111100 }, // $
		{ 0b10100101, 0b01001010 }, // %
		{ 0b11110001, 0b11011110 }, // &
		{ 0b01001000, 0b00000000 }, // '
		{ 0b01010010, 0b01000100 }, // (
		{ 0b01000100, 0b10010100 }, // )
		{ 0b11101011, 0b10101110 }, // *
		{ 0b00001011, 0b10100000 }, // +
		{ 0b00000000, 0b00110010 }, // ,
		{ 0b00000011, 0b10000000 }, // -
		{ 0b00000000, 0b00000100 }, // .
		{ 0b00100101, 0b01001000 }, // / (forward slash)
		{ 0b11110110, 0b11011110 }, // 0 0x30
		{ 0b11001001, 0b00101110 }, // 1
		{ 0b11100111, 0b11001110 }, // 2
		{ 0b11100111, 0b10011110 }, // 3
		{ 0b10110111, 0b10010010 }, // 4
		{ 0b11110011, 0b10011110 }, // 5
		{ 0b10010011, 0b11011110 }, // 6
		{ 0b11100100, 0b10010010 }, // 7
		{ 0b11110111, 0b11011110 }, // 8
		{ 0b11110111, 0b10010010 }, // 9
		{ 0b00001000, 0b00100000 }, // :
		{ 0b00001000, 0b00101000 }, // ;
		{ 0b00001010, 0b00100000 }, // <
		{ 0b00011100, 0b01110000 }, //=
		{ 0b00001000, 0b10100000 }, // >
		{ 0b11000101, 0b00000100 }, // ?
		{ 0b11110110, 0b11111100 }, // @ 0x40
		{ 0b01010111, 0b11011010, }, // A
		{ 0b11010111, 0b01011100 }, // B
		{ 0b01010110, 0b01010100 }, // C
		{ 0b11010110, 0b11011100 }, // D
		{ 0b11110011, 0b01001110 }, // E
		{ 0b11110011, 0b01001000 }, // F
		{ 0b01110110, 0b11010110 }, // G
		{ 0b10110111, 0b11011010 }, // H
		{ 0b11101001, 0b00101110 }, // I
		{ 0b00100110, 0b11010100 }, // J
		{ 0b10111010, 0b01101010 }, // K
		{ 0b10010010, 0b01001110 }, // L
		{ 0b10111111, 0b11011010 }, // M
		{ 0b11010110, 0b11011010 }, // N
		{ 0b01010110, 0b11010100 }, // O
		{ 0b11010111, 0b01001000 }, // P 0x50
		{ 0b01010110, 0b11100110 }, // Q
		{ 0b11010111, 0b01011010 }, // R
		{ 0b01110001, 0b00011100 }, // S
		{ 0b11101001, 0b00100100 }, // T
		{ 0b10110110, 0b11010110 }, // U
		{ 0b10110110, 0b11010100 }, // V
		{ 0b10110111, 0b11111010 }, // W
		{ 0b10110101, 0b01011010 }, // X
		{ 0b10110101, 0b00100100 }, // Y
		{ 0b11100101, 0b01001110 }, // Z
		{ 0b11010010, 0b01001100 }, // [
		{ 0b10010001, 0b00010010 }, // \ (back slash)
		{ 0b01100100, 0b10010110 }, // ]
		{ 0b01010100, 0b00000000 }, // ^
		{ 0b00000000, 0b00001110 }, // _
		{ 0b01000100, 0b00000000 }, // ` 0x60
		{ 0b00011000, 0b11111010 }, // a
		{ 0b10011010, 0b11011100 }, // b
		{ 0b00001010, 0b11000110 }, // c
		{ 0b00101110, 0b11010110 }, // d
		{ 0b00001011, 0b11000110 }, // e
		{ 0b00101001, 0b10100100 }, // f
		{ 0b00011011, 0b10011100 }, // g
		{ 0b10011010, 0b11011010 }, // h
		{ 0b01000001, 0b00100100 }, // i
		{ 0b00001100, 0b11010100 }, // j
		{ 0b10010111, 0b01101010 }, // k
		{ 0b01001001, 0b00100100 }, // l
		{ 0b00010111, 0b11111010 }, // m
		{ 0b00011010, 0b11011010 }, // n
		{ 0b00001010, 0b11010100 }, // o
		{ 0b00011010, 0b01101000 }, // p
		{ 0b00001110, 0b10110010 }, // q
		{ 0b00000101, 0b00100100 }, // r
		{ 0b00000101, 0b00010100 }, // s
		{ 0b01001101, 0b00100010 }, // t
		{ 0b00010110, 0b11010110 }, // u
		{ 0b00010110, 0b11010100 }, // v
		{ 0b00010111, 0b11111010 }, // w
		{ 0b00010101, 0b01011010 }, // x
		{ 0b00010101, 0b10011100 }, // y
		{ 0b00001100, 0b10100110 }, // z
		{ 0b01101010, 0b00100110 }, // {
		{ 0b01001001, 0b00100100 }, // |
		{ 0b11001000, 0b10101100 }, // }
		{ 0b00010011, 0b10010000 }, // ~
		{ 0b11110110, 0b11011110 } // DEL
};
