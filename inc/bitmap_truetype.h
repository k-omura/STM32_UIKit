/*
 * bitmap.h
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

#define FLAG_ONCURVE (1 << 0)
#define FLAG_XSHORT (1 << 1)
#define FLAG_YSHORT (1 << 2)
#define FLAG_REPEAT (1 << 3)
#define FLAG_XSAME (1 << 4)
#define FLAG_YSAME (1 << 5)

#define TEXT_ALIGN_LEFT 0
#define TEXT_ALIGN_CENTER 1
#define TEXT_ALIGN_RIGHT 2

#define ROTATE_0 0
#define ROTATE_90 1
#define ROTATE_180 2
#define ROTATE_270 3

struct bitmap_truetype_fs_t{
	FATFS FatFs;
	FIL File;
	FRESULT fr;
};
extern struct bitmap_truetype_fs_t bitmap_truetype_fs;

struct bitmap_truetype_param_t{
	uint16_t characterSize;
	uint8_t kerningOn;
	int16_t characterSpace;
	int16_t start_x;
	int16_t end_x;
	int16_t end_y;
	uint16_t displayWidth;
	uint16_t displayHeight;
	uint16_t displayWidthFrame;
	uint8_t stringRotation;
	uint8_t colorLine;
	uint8_t colorInside;
	uint8_t fillInside;
};

typedef struct {
	char name[5];
	uint32_t checkSum;
	uint32_t offset;
	uint32_t length;
} ttTable_t;

typedef struct {
	uint32_t version;
	uint32_t revision;
	uint32_t checkSumAdjustment;
	uint32_t magicNumber;
	uint16_t flags;
	uint16_t unitsPerEm;
	char     created[8];
	char     modified[8];
	int16_t  xMin;
	int16_t  yMin;
	int16_t  xMax;
	int16_t  yMax;
	uint16_t macStyle;
	uint16_t lowestRecPPEM;
	int16_t fontDirectionHint;
	int16_t indexToLocFormat;
	int16_t glyphDataFormat;
} ttHeadttTable_t;

typedef struct {
	uint8_t flag;
	int16_t x;
	int16_t y;
} ttPoint_t;

typedef struct {
	int16_t numberOfContours;
	int16_t xMin;
	int16_t yMin;
	int16_t xMax;
	int16_t yMax;
	uint16_t *endPtsOfContours;
	uint16_t numberOfPoints;
	ttPoint_t *points;
} ttGlyph_t;

typedef struct {
	int16_t dx;
	int16_t dy;
	uint8_t enableScale;
	uint16_t scale_x;
	uint16_t scale_y;
} ttGlyphTransformation_t;

/* currently only support format4 cmap tables */
typedef struct {
	uint16_t version;
	uint16_t numberSubtables;
} ttCmapIndex_t;

typedef struct {
	uint16_t platformId;
	uint16_t platformSpecificId;
	uint16_t offset;
} ttCmapEncoding_t;

typedef struct {
	uint16_t format;
	uint16_t length;
	uint16_t language;
	uint16_t segCountX2;
	uint16_t searchRange;
	uint16_t entrySelector;
	uint16_t rangeShift;
	uint32_t offset;
	uint32_t endCodeOffset;
	uint32_t startCodeOffset;
	uint32_t idDeltaOffset;
	uint32_t idRangeOffsetOffset;
	uint32_t glyphIndexArrayOffset;
} ttCmapFormat4_t;

/* currently only support format0 kerning tables */
typedef struct {
	uint32_t version; //The version number of the kerning table (0x00010000 for the current version).
	uint32_t nTables; //The number of subtables included in the kerning table.
} ttKernHeader_t;

	typedef struct {
	uint32_t length; //The length of this subtable in bytes, including this header.
	uint16_t coverage; //Circumstances under which this table is used. See below for description.
} ttKernSubtable_t;

typedef struct {
	uint16_t nPairs; //The number of kerning pairs in this subtable.
	uint16_t searchRange; //The largest power of two less than or equal to the value of nPairs, multiplied by the size in bytes of an entry in the subtable.
	uint16_t entrySelector; //This is calculated as log2 of the largest power of two less than or equal to the value of nPairs. This value indicates how many iterations of the search loop have to be made. For example, in a list of eight items, there would be three iterations of the loop.
	uint16_t rangeShift; //The value of nPairs minus the largest power of two less than or equal to nPairs. This is multiplied by the size in bytes of an entry in the table.
} ttKernFormat0_t;

typedef struct {
	int16_t x;
	int16_t y;
} ttCoordinate_t;

typedef struct {
	uint16_t advanceWidth;
	int16_t leftSideBearing;
} ttHMetric_t;

//Assuming "public"
uint8_t truetype_setTtfFile(uint8_t);
void truetype_setCharacterSpacing(int16_t, uint8_t);
void truetype_setCharacterSize(uint16_t);
void truetype_setTextBoundary(uint16_t, uint16_t, uint16_t);
void truetype_setTextColor(uint8_t, uint8_t, uint8_t);
void truetype_setTextRotation(uint16_t);
void truetype_textDraw(int16_t, int16_t, const wchar_t _character[]);
/*
uint16_t getStringWidth(const wchar_t _character[]);
*/
//Assuming "private"
uint8_t getUInt8t();
int16_t getInt16t();
uint16_t getUInt16t();
uint32_t getUInt32t();
int32_t map(int32_t, int32_t, int32_t, int32_t, int32_t);
//int16_t swap_int16(int16_t);
//uint16_t swap_uint16(uint16_t);
//uint32_t swap_uint32(uint32_t);

//basic
uint8_t readTableDirectory(uint8_t);
uint32_t calculateCheckSum(uint32_t, uint32_t);
uint32_t seekToTable(const char *name);
void readHeadTable();
void readCoords(char, uint16_t);
//Glyph
uint32_t getGlyphOffset(uint16_t);
uint16_t codeToGlyphId(uint16_t);
uint8_t readSimpleGlyph(uint8_t);
uint8_t readCompoundGlyph();

//cmap. maps character codes to glyph indices
uint8_t readCmapFormat4();
uint8_t readCmap();
//hmtx. metric information for the horizontal layout each of the glyphs
uint8_t readHMetric();
ttHMetric_t getHMetric(uint16_t);
//kerning.
uint8_t readKern();
int16_t getKerning(uint16_t, uint16_t);
//glyf
void generateOutline(int16_t, int16_t, uint16_t);
void freePointsAll();
uint8_t isInside(int16_t, int16_t);
uint8_t readGlyph(uint16_t, uint8_t);
void freeGlyph();
void addPoint(int16_t, int16_t);
void freePoints();
void addBeginPoint(uint16_t);
void freeBeginPoints();
void addEndPoint(uint16_t);
void freeEndPoints();
int32_t isLeft(ttCoordinate_t, ttCoordinate_t, ttCoordinate_t);
void addLine(int16_t, int16_t, int16_t, int16_t);

#endif /* INC_BITAMAP_TRUETYPE_H_ */
