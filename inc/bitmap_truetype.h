/*
 * bitmap_truetype.h
 *
 *  Created on: Dec. 11, 2021
 *      Author: k-omura
 */
/*
  Read truetype(.ttf) from SD and generate bitmap.

  TrueType™ Reference Manual
  https://developer.apple.com/fonts/TrueType-Reference-Manual/
*/

#include "stdio.h"
#include "string.h"
#include "bitmap.h"
#include "fatfs.h"

#ifndef INC_BITAMAP_TRUETYPE_H_
#define INC_BITAMAP_TRUETYPE_H_

#define TEXT_ALIGN_LEFT 0
#define TEXT_ALIGN_CENTER 1
#define TEXT_ALIGN_RIGHT 2

#define ROTATE_0 0
#define ROTATE_90 1
#define ROTATE_180 2
#define ROTATE_270 3

//public typedef
struct bitmap_truetype_fs_t{
	FATFS FatFs;
	FIL File;
	FRESULT fr;
};
extern struct bitmap_truetype_fs_t bitmap_truetype_fs;

//Assuming "public"
uint8_t truetype_setTtfFile(uint8_t);
void truetype_setCharacterSpacing(int16_t, uint8_t);
void truetype_setCharacterSize(uint16_t);
void truetype_setTextBoundary(uint16_t, uint16_t, uint16_t);
void truetype_setTextColor(uint8_t, uint8_t, uint8_t);
void truetype_setTextRotation(uint16_t);
void truetype_textDrawL(int32_t, int32_t, wchar_t _character[]);
void truetype_textDraw(int32_t, int32_t, char _character[]);
uint16_t truetype_getStringWidthL(wchar_t _character[]);
uint16_t truetype_getStringWidth(char _character[]);

#endif /* INC_BITAMAP_TRUETYPE_H_ */
