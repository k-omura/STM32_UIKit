/*
 * bitmap_truetype.h
 *
 *  Created on: Dec. 11, 2021
 *      Author: k-omura
 */

#include "bitmap_truetype.h"

//private define
#define FLAG_ONCURVE (1 << 0)
#define FLAG_XSHORT (1 << 1)
#define FLAG_YSHORT (1 << 2)
#define FLAG_REPEAT (1 << 3)
#define FLAG_XSAME (1 << 4)
#define FLAG_YSAME (1 << 5)
//private define end

//private struct typedef
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

typedef struct {
	uint16_t p1;
	uint16_t p2;
	uint8_t up;
} ttWindIntersect_t;
//private struct typedef end


//private variable
uint32_t bytesread;
const int numTablesPos = 4;
const int tablePos = 12;

uint16_t numTables;
ttTable_t *table;
ttHeadttTable_t headTable;

uint16_t charCode;
int16_t xMin, xMax, yMin, yMax;

ttCmapIndex_t cmapIndex;
ttCmapEncoding_t *cmapEncoding;
ttCmapFormat4_t cmapFormat4;

uint32_t hmtxTablePos = 0;

ttKernHeader_t kernHeader;
ttKernSubtable_t kernSubtable;
ttKernFormat0_t kernFormat0;
uint32_t kernTablePos = 0;

ttGlyph_t glyph;
ttGlyphTransformation_t glyphTransformation;

ttCoordinate_t *points;
uint16_t numPoints;
uint16_t *beginPoints;
uint16_t numBeginPoints;
uint16_t *endPoints;
uint16_t numEndPoints;
ttWindIntersect_t *pointsToFill;

struct bitmap_truetype_fs_t bitmap_truetype_fs;
struct bitmap_truetype_param_t bitmap_truetype_param = {20, 1, 0, 10, 280, 320, 280, 320, 280, 0, 0x00, 0xff};
//private variable end

//private function prototype
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
void generateOutline(int32_t, int32_t, uint16_t);
void freePointsAll();
int16_t isInside(uint16_t, uint16_t);
uint8_t fillGlyph(uint16_t, uint16_t, uint16_t);
uint8_t readGlyph(uint16_t, uint8_t);
void freeGlyph();
uint8_t addPoint(uint16_t, uint16_t);
void freePoints();
uint8_t addBeginPoint(uint16_t);
void freeBeginPoints();
uint8_t addEndPoint(uint16_t);
void freeEndPoints();
void addLine(uint16_t, uint16_t, uint16_t, uint16_t);
int32_t isLeft(ttCoordinate_t *_p0, ttCoordinate_t *_p1, ttCoordinate_t *_point);
//private function prototype end

//----------
uint8_t truetype_setTtfFile(uint8_t _checkCheckSum){
	if(readTableDirectory(_checkCheckSum) == 0){
		f_close(&bitmap_truetype_fs.File);
		return 1;
	}

	if (readCmap() == 0) {
		f_close(&bitmap_truetype_fs.File);
		return 2;
	}

	if (readHMetric() == 0) {
		f_close(&bitmap_truetype_fs.File);
		return 3;
	}

	readKern();
	readHeadTable();

	return 0;
}

void truetype_setCharacterSize(uint16_t _characterSize){
	bitmap_truetype_param.characterSize = _characterSize;
}

void truetype_setCharacterSpacing(int16_t _characterSpace, uint8_t _kerning){
	bitmap_truetype_param.characterSpace = _characterSpace;
	bitmap_truetype_param.kerningOn = _kerning;
}

void truetype_setTextBoundary(uint16_t _start_x, uint16_t _end_x, uint16_t _end_y){
	bitmap_truetype_param.start_x = _start_x;
	bitmap_truetype_param.end_x = _end_x;
	bitmap_truetype_param.end_y = _end_y;
}

void truetype_setTextColor(uint8_t _onLine, uint8_t _inside, uint8_t _fill){
	bitmap_truetype_param.colorLine = _onLine;
	bitmap_truetype_param.colorInside = _inside;
	bitmap_truetype_param.fillInside = _fill;
}

void truetype_setTextRotation(uint16_t _rotation){
	switch(_rotation){
		case ROTATE_90:
		case 90:
			_rotation = 1;
			break;
		case ROTATE_180:
		case 180:
			_rotation = 2;
			break;
		case ROTATE_270:
		case 270:
			_rotation = 3;
			break;
		default:
			_rotation = 0;
			break;
	}
	bitmap_truetype_param.stringRotation = _rotation;
}

void truetype_textDraw(int32_t _x, int32_t _y, char _character[]){
	uint16_t length = 0;
	while(_character[length] != '\0'){
		length++;
	}
	wchar_t *wcharacter = (wchar_t *)calloc(sizeof(wchar_t), length + 1);
	for(uint16_t i = 0; i < length; i++){
		wcharacter[i] = _character[i];
	}
	truetype_textDrawL(_x, _y, wcharacter);
	free(wcharacter);
}

void truetype_textDrawL(int32_t _x, int32_t _y, wchar_t _character[]){
	uint8_t c = 0;
	uint16_t prev_code = 0;

	while (_character[c] != '\0') {
		//space (half-width, full-width)
		if((_character[c] == ' ') || (_character[c] == L'　')){
			prev_code = 0;
			_x += bitmap_truetype_param.characterSize / 4;
			c++;
			continue;
		}

		charCode = codeToGlyphId(_character[c]);
		readGlyph(charCode, 0);

		//sprintf(string, "%c:%d,%d", _character[c], charCode, glyph.numberOfContours);
		//bitmap_terminal(string, 0, 0xff, TERMINAL_LINE_MAX);

		_x += bitmap_truetype_param.characterSpace;
		if(prev_code != 0 && bitmap_truetype_param.kerningOn){
			int16_t kern = getKerning(prev_code, charCode); //space between charctor
			_x += (kern * (int16_t)bitmap_truetype_param.characterSize) / (yMax - yMin);
		}
		prev_code = charCode;

		ttHMetric_t hMetric = getHMetric(charCode);
		uint16_t width = bitmap_truetype_param.characterSize * (glyph.xMax - glyph.xMin) / (yMax - yMin);

		//Line breaks when reaching the edge of the display
		if((hMetric.leftSideBearing + width + _x) > bitmap_truetype_param.end_x){
			_x = bitmap_truetype_param.start_x;
			_y += bitmap_truetype_param.characterSize;
			if(_y > bitmap_truetype_param.end_y){
				break;
			}
		}

		//Line breaks with line feed code
		if(_character[c] == '\n'){
			_x = bitmap_truetype_param.start_x;
			_y += bitmap_truetype_param.characterSize;
			if(_y > bitmap_truetype_param.end_y){
				break;
			}
			continue;
		}

		//Not compatible with Compound glyphs now
		if(glyph.numberOfContours >= 0){
			//write framebuffer
			generateOutline(hMetric.leftSideBearing + _x, _y, width);

			//fill charctor
			if(bitmap_truetype_param.fillInside){
				fillGlyph(hMetric.leftSideBearing + _x, _y, width);
			}
		}
		freePointsAll();
		freeGlyph();

		_x += (hMetric.advanceWidth) ? (hMetric.advanceWidth) : (width);
		c++;
	}
}

uint8_t readTableDirectory(uint8_t _checkCheckSum){
	bitmap_truetype_fs.fr = f_lseek(&bitmap_truetype_fs.File, numTablesPos);
	numTables = getUInt16t();

	table = (ttTable_t *)malloc(sizeof(ttTable_t) * numTables);

	bitmap_truetype_fs.fr = f_lseek(&bitmap_truetype_fs.File, tablePos);

	//bitmap_terminal("---table list---", 0, 0xff, TERMINAL_LINE_MAX);
	for (int i = 0; i < numTables; i++) {
		for (int j = 0; j < 4; j++) {
			table[i].name[j] = getUInt8t();
		}

		table[i].name[4] = '\0';
		table[i].checkSum = getUInt32t();
		table[i].offset = getUInt32t();
		table[i].length = getUInt32t();

		//sprintf(string, "%2d:%3s", i, table[i].name);
		//bitmap_terminal(string, 0, 0xff, TERMINAL_LINE_MAX);
	}

	if (_checkCheckSum) {
		for (uint8_t i = 0; i < numTables; i++) {
			if (strcmp(table[i].name, "head") != 0) { // checksum of "head" is invalid
				uint32_t c = calculateCheckSum(table[i].offset, table[i].length);
				if (table[i].checkSum != c) {
					return 0;
				}
			}
		}
	}
	return 1;
}

uint16_t truetype_getStringWidthL(wchar_t _character[]){
	uint16_t prev_code = 0;
	uint16_t c = 0;
	uint16_t output = 0;

	while (_character[c] != '\0') {
		//space (half-width, full-width)
		if((_character[c] == ' ') || (_character[c] == L'　')){
			prev_code = 0;
			output += bitmap_truetype_param.characterSize / 4;
			c++;
			continue;
		}
		uint16_t code = codeToGlyphId(_character[c]);
		readGlyph(code, 1);

		output += bitmap_truetype_param.characterSpace;
		if(prev_code != 0 && bitmap_truetype_param.kerningOn){
			int16_t kern = getKerning(prev_code, code); //space between charctor
			output += (kern * (int16_t)bitmap_truetype_param.characterSize) / (yMax - yMin);
		}
		prev_code = code;

		ttHMetric_t hMetric = getHMetric(code);
		uint16_t width = bitmap_truetype_param.characterSize * (glyph.xMax - glyph.xMin) / (yMax - yMin);
		output += (hMetric.advanceWidth) ? (hMetric.advanceWidth) : (width);
		c++;
	}

	return output;
}

uint16_t truetype_getStringWidth(char _character[]){
	uint16_t length = 0;
	while(_character[length] != '\0'){
		length++;
	}
	wchar_t *wcharacter = (wchar_t *)calloc(sizeof(wchar_t), length + 1);
	for(uint16_t i = 0; i < length; i++){
		wcharacter[i] = _character[i];
	}
	uint16_t output = truetype_getStringWidthL(wcharacter);
	free(wcharacter);
	return output;
}

uint32_t calculateCheckSum(uint32_t _offset, uint32_t _length){
	uint32_t checksum = 0;

	_length = (_length + 3) / 4;
	bitmap_truetype_fs.fr = f_lseek(&bitmap_truetype_fs.File, _offset);

	while (_length-- > 0) {
	checksum += getUInt32t();
	}
	return checksum;
}

void readHeadTable(){
	for (int i = 0; i < numTables; i++) {
		if (strcmp(table[i].name, "head") == 0) {
			bitmap_truetype_fs.fr = f_lseek(&bitmap_truetype_fs.File, table[i].offset);

			headTable.version = getUInt32t();
			headTable.revision = getUInt32t();
			headTable.checkSumAdjustment = getUInt32t();
			headTable.magicNumber = getUInt32t();
			headTable.flags = getUInt16t();
			headTable.unitsPerEm = getUInt16t();
			for (int j = 0; j < 8; j++) {
				headTable.created[i] = getUInt8t();
			}
			for (int j = 0; j < 8; j++) {
				headTable.modified[i] = getUInt8t();
			}
			xMin = headTable.xMin = getInt16t();
			yMin = headTable.yMin = getInt16t();
			xMax = headTable.xMax = getInt16t();
			yMax = headTable.yMax = getInt16t();
			headTable.macStyle = getUInt16t();
			headTable.lowestRecPPEM = getUInt16t();
			headTable.fontDirectionHint = getInt16t();
			headTable.indexToLocFormat = getInt16t();
			headTable.glyphDataFormat = getInt16t();
		}
	}
}

void readCoords(char _xy, uint16_t _startPoint){
	int16_t value = 0;
	uint8_t shortFlag, sameFlag;

	if (_xy == 'x') {
		shortFlag = FLAG_XSHORT;
		sameFlag = FLAG_XSAME;
	} else {
		shortFlag = FLAG_YSHORT;
		sameFlag = FLAG_YSAME;
	}

	for (uint16_t i = _startPoint; i < glyph.numberOfPoints; i++) {
		if (glyph.points[i].flag & shortFlag) {
			if (glyph.points[i].flag & sameFlag) {
				value += getUInt8t();
			} else {
				value -= getUInt8t();
			}
		} else if (~glyph.points[i].flag & sameFlag) {
			value += getUInt16t();
		}

		if (_xy == 'x') {
			if(glyphTransformation.enableScale){
				glyph.points[i].x = value + glyphTransformation.dx;
			}else{
				glyph.points[i].x = value + glyphTransformation.dx;
			}
		} else {
			if(glyphTransformation.enableScale){
				glyph.points[i].y = value + glyphTransformation.dy;
			}else{
				glyph.points[i].y = value + glyphTransformation.dy;
			}
		}
	}
}

uint32_t seekToTable(const char *_name){
	for (uint32_t i = 0; i < numTables; i++) {
		if (strcmp(table[i].name, _name) == 0) {
			bitmap_truetype_fs.fr = f_lseek(&bitmap_truetype_fs.File, table[i].offset);
			return table[i].offset;
		}
	}
	return 0;
}

uint8_t readCmap(){
	uint16_t platformId, platformSpecificId;
	uint32_t cmapOffset, tableOffset;
	uint8_t foundMap = 0;

	if ((cmapOffset = seekToTable("cmap")) == 0) {
		return 0;
	}

	cmapIndex.version = getUInt16t();
	cmapIndex.numberSubtables = getUInt16t();

	for (uint16_t i = 0; i < cmapIndex.numberSubtables; i++) {
		platformId = getUInt16t();
		platformSpecificId = getUInt16t();
		tableOffset = getUInt32t();

		if ((platformId == 3) && (platformSpecificId == 1)) {
			cmapFormat4.offset = cmapOffset + tableOffset;
			readCmapFormat4();
			foundMap = 1;
			break;
		}
	}

	if (foundMap == 0) {
		return 0;
	}

	return 1;
}

uint8_t readCmapFormat4(){
	bitmap_truetype_fs.fr = f_lseek(&bitmap_truetype_fs.File, cmapFormat4.offset);
	if ((cmapFormat4.format = getUInt16t()) != 4) {
		return 0;
	}

	cmapFormat4.length = getUInt16t();
	cmapFormat4.language = getUInt16t();
	cmapFormat4.segCountX2 = getUInt16t();
	cmapFormat4.searchRange = getUInt16t();
	cmapFormat4.entrySelector = getUInt16t();
	cmapFormat4.rangeShift = getUInt16t();
	cmapFormat4.endCodeOffset = cmapFormat4.offset + 14;
	cmapFormat4.startCodeOffset = cmapFormat4.endCodeOffset + cmapFormat4.segCountX2 + 2;
	cmapFormat4.idDeltaOffset = cmapFormat4.startCodeOffset + cmapFormat4.segCountX2;
	cmapFormat4.idRangeOffsetOffset = cmapFormat4.idDeltaOffset + cmapFormat4.segCountX2;
	cmapFormat4.glyphIndexArrayOffset = cmapFormat4.idRangeOffsetOffset + cmapFormat4.segCountX2;

	return 1;
}

uint8_t readHMetric(){
	if (seekToTable("hmtx") == 0) {
		return 0;
	}

	hmtxTablePos = f_tell(&bitmap_truetype_fs.File);
	return 1;
}

ttHMetric_t getHMetric(uint16_t _code){
	ttHMetric_t result;
	result.advanceWidth = 0;

	bitmap_truetype_fs.fr = f_lseek(&bitmap_truetype_fs.File, hmtxTablePos + (_code * 4));
	result.advanceWidth = getUInt16t();
	result.leftSideBearing = getInt16t();

	result.advanceWidth = (result.advanceWidth * bitmap_truetype_param.characterSize) / (yMax - yMin);
	result.leftSideBearing  = (result.leftSideBearing * bitmap_truetype_param.characterSize) / (yMax - yMin);
	return result;
}

uint8_t readKern(){
	uint32_t nextTable;

	if (seekToTable("kern") == 0) {
		return 0;
	}

	kernHeader.nTables = getUInt32t();

	//only support up to 32 sub-tables
	if (kernHeader.nTables > 32){
		kernHeader.nTables = 32;
	}

	for (uint8_t i = 0; i < kernHeader.nTables; i++) {
		uint16_t format;

		kernSubtable.length = getUInt32t();
		nextTable = f_tell(&bitmap_truetype_fs.File) + kernSubtable.length;
		kernSubtable.coverage = getUInt16t();

		format = (uint16_t)(kernSubtable.coverage >> 8);

		// only support format0
		if(format != 0){
			bitmap_truetype_fs.fr = f_lseek(&bitmap_truetype_fs.File, nextTable);
			continue;
		}

		// only use horizontal kerning tables
		if ((kernSubtable.coverage & 0x0003) != 0x0001){
			bitmap_truetype_fs.fr = f_lseek(&bitmap_truetype_fs.File, nextTable);
			continue;
		}

		//format0
		kernFormat0.nPairs = getUInt16t();
		kernFormat0.searchRange = getUInt16t();
		kernFormat0.entrySelector = getUInt16t();
		kernFormat0.rangeShift = getUInt16t();
		kernTablePos = f_tell(&bitmap_truetype_fs.File);

		break;
	}

	return 1;
}

int16_t getKerning(uint16_t _left_glyph, uint16_t _right_glyph){
	//int16_t result = characterSpace;
	int16_t result = 0;
	uint32_t key0 = ((uint32_t)(_left_glyph) << 16) | (_right_glyph);

	bitmap_truetype_fs.fr = f_lseek(&bitmap_truetype_fs.File, kernTablePos);

	for(uint16_t i = 0; i < kernFormat0.nPairs; i++){
		uint32_t key1 = getUInt32t();
		if(key0 == key1){
			result = getInt16t();
			break;
		}
		bitmap_truetype_fs.fr = f_lseek(&bitmap_truetype_fs.File, f_tell(&bitmap_truetype_fs.File) + 2);
	}

	return result;
}

void generateOutline(int32_t _x, int32_t _y, uint16_t _width){
	points = NULL;
	numPoints = 0;
	numBeginPoints = 0;
	numEndPoints = 0;

	int32_t x0, y0, x1, y1;

	uint16_t j = 0;

	for (uint16_t i = 0; i < glyph.numberOfContours; i++) {
		uint8_t firstPointOfContour = j;
		uint8_t lastPointOfContour = glyph.endPtsOfContours[i];

		//Rotate to on-curve the first point
		uint16_t numberOfRotations = 0;
		while((firstPointOfContour + numberOfRotations) <= lastPointOfContour){
			if(glyph.points[(firstPointOfContour + numberOfRotations)].flag & FLAG_ONCURVE){
				break;
			}
			numberOfRotations++;
		}
		if((j + numberOfRotations) <= lastPointOfContour){
			for(uint16_t ii = 0; ii < numberOfRotations; ii++){
				ttPoint_t tmp = glyph.points[firstPointOfContour];
				for(uint16_t jj = firstPointOfContour; jj < lastPointOfContour; jj++){
					glyph.points[jj] = glyph.points[jj + 1];
				}
				glyph.points[lastPointOfContour] = tmp;
			}
		}

		while(j <= lastPointOfContour){
			ttCoordinate_t pointsOfCurve[4];

			//Examine the number of dimensions of a curve
			pointsOfCurve[0].x = glyph.points[j].x;
			pointsOfCurve[0].y = glyph.points[j].y;
			uint16_t searchPoint = (j == lastPointOfContour) ? (firstPointOfContour) : (j + 1);
			uint8_t degree = 1;
			while(searchPoint != j){
				if(degree < 4){
					pointsOfCurve[degree].x = glyph.points[searchPoint].x;
					pointsOfCurve[degree].y = glyph.points[searchPoint].y;
				}
				if(glyph.points[searchPoint].flag & FLAG_ONCURVE){
					break;
				}
				searchPoint = (searchPoint == lastPointOfContour) ? (firstPointOfContour) : (searchPoint + 1);
				degree++;
			}

			//Replace Bezier curves of 4 dimensions or more with straight lines
			if(degree >= 4){
				uint16_t tmp_j = j;
				uint16_t tmp_degree = 0;
				while(tmp_degree < degree){
					if(tmp_j > lastPointOfContour){
						tmp_j = firstPointOfContour;
					}
					glyph.points[tmp_j].flag |= FLAG_ONCURVE;
					tmp_j++;
					tmp_degree++;
				}
			}

			//Generate outline according to degree
			switch(degree){
				case 3: //third-order Bezier curve
					x0 = pointsOfCurve[0].x;
					y0 = pointsOfCurve[0].y;

					for (uint8_t t = 0; t <= 10; t += 2) {
						x1 = (int32_t)((10 - t) * (10 - t) * (10 - t) * pointsOfCurve[0].x + 3 * (10 - t) * (10 - t) * t * pointsOfCurve[1].x + 3 * (10 - t) * t * t * pointsOfCurve[2].x + t * t * t * pointsOfCurve[3].x);
						y1 = (int32_t)((10 - t) * (10 - t) * (10 - t) * pointsOfCurve[0].y + 3 * (10 - t) * (10 - t) * t * pointsOfCurve[1].y + 3 * (10 - t) * t * t * pointsOfCurve[2].y + t * t * t * pointsOfCurve[3].y);
						x1 /= 1000;
						y1 /= 1000;

						addLine(map(x0, glyph.xMin, glyph.xMax, _x, _x + _width - 1),
								map(y0, yMin, yMax, _y + bitmap_truetype_param.characterSize - 1, _y),
								map(x1, glyph.xMin, glyph.xMax, _x, _x + _width - 1),
								map(y1, yMin, yMax, _y + bitmap_truetype_param.characterSize - 1, _y));
						x0 = x1;
						y0 = y1;
					}
					break;
				case 2: //Second-order Bezier curve
					x0 = pointsOfCurve[0].x;
					y0 = pointsOfCurve[0].y;

					for (uint8_t t = 0; t <= 10; t += 2) {
						x1 = (int32_t)((10 - t) * (10 - t) * pointsOfCurve[0].x + 2 * t * (10 - t) * pointsOfCurve[1].x + t * t * pointsOfCurve[2].x);
						y1 = (int32_t)((10 - t) * (10 - t) * pointsOfCurve[0].y + 2 * t * (10 - t) * pointsOfCurve[1].y + t * t * pointsOfCurve[2].y);
						x1 /= 100;
						y1 /= 100;

						addLine(map(x0, glyph.xMin, glyph.xMax, _x, _x + _width - 1),
								map(y0, yMin, yMax, _y + bitmap_truetype_param.characterSize - 1, _y),
								map(x1, glyph.xMin, glyph.xMax, _x, _x + _width - 1),
								map(y1, yMin, yMax, _y + bitmap_truetype_param.characterSize - 1, _y));
						x0 = x1;
						y0 = y1;
					}

					break;
				default:
					degree = 1;
				case 1: //straight line
					addLine(map(pointsOfCurve[0].x, glyph.xMin, glyph.xMax, _x, _x + _width - 1),
							map(pointsOfCurve[0].y, yMin, yMax, _y + bitmap_truetype_param.characterSize - 1, _y),
							map(pointsOfCurve[1].x, glyph.xMin, glyph.xMax, _x, _x + _width - 1),
							map(pointsOfCurve[1].y, yMin, yMax, _y + bitmap_truetype_param.characterSize - 1, _y));
					break;
			}

			j += degree;
		}
		addEndPoint(numPoints - 1);
		addBeginPoint(numPoints);
	}
	return;
}

void freePointsAll(){
	freePoints();
	freeBeginPoints();
	freeEndPoints();
}

int16_t isInside(uint16_t _x, uint16_t _y){
	int16_t windingNumber = 0;
	uint16_t bpCounter = 0, epCounter = 0;
	ttCoordinate_t point = {_x, _y};
	ttCoordinate_t point1;
	ttCoordinate_t point2;

	for (uint16_t i = 0; i < numPoints; i++) {
		point1 = points[i];
		// Wrap?
		if (i == endPoints[epCounter]) {
			point2 = points[beginPoints[bpCounter]];
			epCounter++;
			bpCounter++;
		} else {
			point2 = points[i + 1];
		}

		if (point1.y <= point.y) {
			if (point2.y > point.y) {
				if (isLeft(&point1, &point2, &point) > 0) {
					windingNumber++;
				}
			}
		} else {
			// start y > point.y (no test needed)
			if (point2.y <= point.y) {
				if (isLeft(&point1, &point2, &point) < 0) {
					windingNumber--;
				}
			}
		}
	}

	return windingNumber;
}

uint8_t fillGlyph(uint16_t _x_min, uint16_t _y_min, uint16_t _width){
	for(uint16_t y = _y_min; y < (_y_min + bitmap_truetype_param.characterSize); y++){
		ttCoordinate_t point1, point2;
		ttCoordinate_t point;
		point.y = y;

		uint16_t intersectPointsNum = 0;
		uint16_t bpCounter = 0;
		uint16_t epCounter = 0;
	    uint16_t p2Num = 0;

		for (uint16_t i = 0; i < numPoints; i++) {
			point1 = points[i];
			// Wrap?
			if (i == endPoints[epCounter]) {
				p2Num = beginPoints[bpCounter];
				epCounter++;
				bpCounter++;
			} else {
				p2Num = i + 1;
			}
			point2 = points[p2Num];

			if (point1.y <= y) {
				if (point2.y > y) {
					//Have a valid up intersect
					intersectPointsNum++;
					ttWindIntersect_t *tmp = (ttWindIntersect_t *)realloc(pointsToFill, sizeof(ttWindIntersect_t) * intersectPointsNum);
					if(tmp == NULL){
						free(pointsToFill);
						return 0;
					}
					pointsToFill = tmp;
					pointsToFill[intersectPointsNum - 1].p1 = i;
					pointsToFill[intersectPointsNum - 1].p2 = p2Num;
					pointsToFill[intersectPointsNum - 1].up = 1;
				}
			} else {
				// start y > point.y (no test needed)
				if (point2.y <= y) {
					//Have a valid down intersect
					intersectPointsNum++;
					ttWindIntersect_t *tmp = (ttWindIntersect_t *)realloc(pointsToFill, sizeof(ttWindIntersect_t) * intersectPointsNum);
					if(tmp == NULL){
						free(pointsToFill);
						return 0;
					}
					pointsToFill = tmp;
					pointsToFill[intersectPointsNum - 1].p1 = i;
					pointsToFill[intersectPointsNum - 1].p2 = p2Num;
					pointsToFill[intersectPointsNum - 1].up = 0;
				}
			}
		}

		int16_t windingNumber = 0;
		for(uint16_t x = _x_min; x < (_x_min + _width); x++){
			windingNumber = 0;
			point.x = x;

			for (uint16_t i = 0; i < intersectPointsNum; i++) {
				point1 = points[pointsToFill[i].p1];
				point2 = points[pointsToFill[i].p2];

				if(pointsToFill[i].up == 1){
					if (isLeft(&point1, &point2, &point) > 0) {
						windingNumber++;
					}
				}else{
					if (isLeft(&point1, &point2, &point) < 0) {
						windingNumber--;
					}
				}
			}

			if(windingNumber != 0){
				bitmap_pixel(x, y, bitmap_truetype_param.colorInside);
			}
		}

		free(pointsToFill);
		pointsToFill = NULL;
	}
	return 1;
}

uint8_t readGlyph(uint16_t _code, uint8_t _justSize){
	uint32_t offset = getGlyphOffset(_code);
	bitmap_truetype_fs.fr = f_lseek(&bitmap_truetype_fs.File, offset);
	glyph.numberOfContours = getInt16t();
	glyph.xMin = getInt16t();
	glyph.yMin = getInt16t();
	glyph.xMax = getInt16t();
	glyph.yMax = getInt16t();

	glyphTransformation.dx = 0;
	glyphTransformation.dy = 0;
	glyphTransformation.enableScale = 0;
	glyphTransformation.scale_x = 1;
	glyphTransformation.scale_y = 1;

	if(_justSize){
		return 0;
	}

	if (glyph.numberOfContours >= 0) {
		return readSimpleGlyph(0);
	}else{
		return readCompoundGlyph();
	}
	return 0;
}

uint32_t getGlyphOffset(uint16_t _index){
	uint32_t offset = 0;

	for (int i = 0; i < numTables; i++) {
		if (strcmp(table[i].name, "loca") == 0) {
			if (headTable.indexToLocFormat == 1) {
				bitmap_truetype_fs.fr = f_lseek(&bitmap_truetype_fs.File, (table[i].offset + _index * 4));
				offset = getUInt32t();
			} else {
				bitmap_truetype_fs.fr = f_lseek(&bitmap_truetype_fs.File, (table[i].offset + _index * 2));
				offset = getUInt16t() * 2;
			}
			break;
		}
	}

	for (int i = 0; i < numTables; i++) {
		if (strcmp(table[i].name, "glyf") == 0) {
			return (offset + table[i].offset);
		}
	}

	return 0;
}

uint16_t codeToGlyphId(uint16_t _code){
	uint16_t start, end, idRangeOffset;
	int16_t idDelta;
	uint8_t found = 0;
	uint16_t offset, glyphId;

	for (int i = 0; i < cmapFormat4.segCountX2 / 2; i++) {
		bitmap_truetype_fs.fr = f_lseek(&bitmap_truetype_fs.File, (cmapFormat4.endCodeOffset + 2 * i));
		end = getUInt16t();
		if (_code <= end) {
			bitmap_truetype_fs.fr = f_lseek(&bitmap_truetype_fs.File, (cmapFormat4.startCodeOffset + 2 * i));
			start = getUInt16t();
			if (_code >= start) {
				bitmap_truetype_fs.fr = f_lseek(&bitmap_truetype_fs.File, (cmapFormat4.idDeltaOffset + 2 * i));
				idDelta = getInt16t();
				bitmap_truetype_fs.fr = f_lseek(&bitmap_truetype_fs.File, (cmapFormat4.idRangeOffsetOffset + 2 * i));
				idRangeOffset = getUInt16t();
				if (idRangeOffset == 0) {
					glyphId = (idDelta + _code) % 65536;
				} else {
					offset = (idRangeOffset / 2 + i + _code - start - cmapFormat4.segCountX2 / 2) * 2;
					bitmap_truetype_fs.fr = f_lseek(&bitmap_truetype_fs.File, (cmapFormat4.glyphIndexArrayOffset + offset));
					glyphId = getUInt16t();
				}

				found = 1;
				break;
			}
		}
	}
	if (!found) {
		return 0;
	}
	return glyphId;
}

uint8_t readSimpleGlyph(uint8_t _addGlyph){
	uint8_t repeatCount;
	uint8_t flag;
	static uint16_t counterContours;
	static uint16_t counterPoints;

	if (glyph.numberOfContours <= 0) {
		return 0;
	}

	if(!_addGlyph){
		counterContours = 0;
		counterPoints = 0;
	}

	if(_addGlyph){
		uint16_t *tmp;
		tmp = (uint16_t *)realloc(glyph.endPtsOfContours, (sizeof(uint16_t) * glyph.numberOfContours));
		if(tmp == NULL){
			free(glyph.endPtsOfContours);
			return 0;
		}
		glyph.endPtsOfContours = tmp;
	}else{
		glyph.endPtsOfContours = (uint16_t *)malloc((sizeof(uint16_t) * glyph.numberOfContours));
	}

	for (uint16_t i = counterContours; i < glyph.numberOfContours; i++) {
		glyph.endPtsOfContours[i] = counterPoints + getUInt16t();
	}

	bitmap_truetype_fs.fr = f_lseek(&bitmap_truetype_fs.File, (getUInt16t() + f_tell(&bitmap_truetype_fs.File)));

	for (uint16_t i = counterContours; i < glyph.numberOfContours; i++) {
		if (glyph.endPtsOfContours[i] > glyph.numberOfPoints) {
			glyph.numberOfPoints = glyph.endPtsOfContours[i];
		}
	}
	glyph.numberOfPoints++;

	if(_addGlyph){
		ttPoint_t *tmp;
		tmp = (ttPoint_t *)realloc(glyph.points, sizeof(ttPoint_t) * (glyph.numberOfPoints + glyph.numberOfContours));
		if(tmp == NULL){
			free(glyph.points);
			return 0;
		}
		glyph.points = tmp;
	}else{
		glyph.points = (ttPoint_t *)malloc(sizeof(ttPoint_t) * (glyph.numberOfPoints + glyph.numberOfContours));
	}

	for (uint16_t i = counterPoints; i < glyph.numberOfPoints; i++) {
		flag = getUInt8t();
		glyph.points[i].flag = flag;
		if (flag & FLAG_REPEAT) {
			repeatCount = getUInt8t();
			while (repeatCount--) {
				glyph.points[++i].flag = flag;
			}
		}
	}

	readCoords('x', counterPoints);
	readCoords('y', counterPoints);

	counterContours = glyph.numberOfContours;
	counterPoints = glyph.numberOfPoints;

	return 1;
}
uint8_t readCompoundGlyph(){
	uint16_t glyphIndex;
	uint16_t flags;
	uint8_t numberOfGlyphs = 0;
	uint32_t offset;
	int32_t arg1, arg2;

	glyph.numberOfContours = 0;

	do{
		flags = getUInt16t();
		glyphIndex = getUInt16t();

		glyphTransformation.enableScale = (flags & 0b00000001000) ? (1) : (0);

		if(flags & 0b00000000001){
			arg1 = getInt16t();
			arg2 = getInt16t();
		}else{
			arg1 = getUInt8t();
			arg2 = getUInt8t();
		}

		if(flags & 0b00000000010){
			glyphTransformation.dx = arg1;
			glyphTransformation.dy = arg2;
		}

		if(flags & 0b01000000000){
			charCode = glyphIndex;
		}

		offset = f_tell(&bitmap_truetype_fs.File);

		uint32_t glyphOffset = getGlyphOffset(glyphIndex);
		bitmap_truetype_fs.fr = f_lseek(&bitmap_truetype_fs.File, glyphOffset);
		glyph.numberOfContours += getInt16t();
		bitmap_truetype_fs.fr = f_lseek(&bitmap_truetype_fs.File, glyphOffset + 10);

		if(numberOfGlyphs == 0){
			readSimpleGlyph(0);
		}else{
			readSimpleGlyph(1);
		}
		bitmap_truetype_fs.fr = f_lseek(&bitmap_truetype_fs.File, offset);

		numberOfGlyphs++;
		glyphTransformation.dx = 0;
		glyphTransformation.dy = 0;
		glyphTransformation.enableScale = 0;
		glyphTransformation.scale_x = 1;
		glyphTransformation.scale_y = 1;
	}while(flags & 0b00000100000);

	return 1;
}

void addLine(uint16_t _x0, uint16_t _y0, uint16_t _x1, uint16_t _y1){
	if (numPoints == 0) {
		addPoint(_x0, _y0);
		addBeginPoint(0);
	}
	addPoint(_x1, _y1);

	bitmap_line(_x0, _y0, _x1, _y1, bitmap_truetype_param.colorLine);
}

void freeGlyph(){
	free(glyph.points);
	free(glyph.endPtsOfContours);
	glyph.numberOfPoints = 0;
}
uint8_t addPoint(uint16_t _x, uint16_t _y){
	numPoints++;
	ttCoordinate_t *tmp;
	tmp = (ttCoordinate_t *)realloc(points, sizeof(ttCoordinate_t) * numPoints);
	if(tmp == NULL){
		free(points);
		return 0;
	}
	points = tmp;
	points[(numPoints - 1)].x = _x;
	points[(numPoints - 1)].y = _y;
	return 1;
}
void freePoints(){
	free(points);
	points = NULL;
	numPoints = 0;
}
uint8_t addBeginPoint(uint16_t _bp){
	numBeginPoints++;
	uint16_t *tmp;
	tmp = (uint16_t *)realloc(beginPoints, sizeof(uint16_t) * numBeginPoints);
	if(tmp == NULL){
		free(beginPoints);
		return 0;
	}
	beginPoints = tmp;
	beginPoints[(numBeginPoints - 1)] = _bp;
	return 1;
}
void freeBeginPoints(){
	free(beginPoints);
	beginPoints = NULL;
	numBeginPoints = 0;
}
uint8_t addEndPoint(uint16_t _ep){
	numEndPoints++;
	uint16_t *tmp;
	tmp = (uint16_t *)realloc(endPoints, sizeof(uint16_t) * numEndPoints);
	if(tmp == NULL){
		free(endPoints);
		return 0;
	}
	endPoints = tmp;
	endPoints[(numEndPoints - 1)] = _ep;
	return 1;
}
void freeEndPoints(){
	free(endPoints);
	endPoints = NULL;
	numEndPoints = 0;
}

int32_t isLeft(ttCoordinate_t *_p0, ttCoordinate_t *_p1, ttCoordinate_t *_point) {
	return ((_p1->x - _p0->x) * (_point->y - _p0->y) - (_point->x - _p0->x) * (_p1->y - _p0->y));
}

uint8_t getUInt8t(){
	uint8_t x[1];

	f_read(&bitmap_truetype_fs.File, x, 1, (unsigned int*)&bytesread);
	return x[0];
}
int16_t getInt16t(){
	uint8_t x[2];

	f_read(&bitmap_truetype_fs.File, x, 2, (unsigned int*)&bytesread);
	return (x[0] << 8) | x[1];
}
uint16_t getUInt16t(){
	uint8_t x[2];

	f_read(&bitmap_truetype_fs.File, x, 2, (unsigned int*)&bytesread);
	return (x[0] << 8) | x[1];
}
uint32_t getUInt32t(){
	uint8_t x[4];

	f_read(&bitmap_truetype_fs.File, x, 4, (unsigned int*)&bytesread);
	return (x[0] << 24) | (x[1] << 16) | (x[2] << 8) | x[3];
}

int32_t map(int32_t _cur_val, int32_t _cur_min, int32_t _cur_max, int32_t _new_min, int32_t _new_max){
	int32_t cur_width = _cur_max - _cur_min;
	int32_t new_width = _new_max - _new_min;

	return new_width * (_cur_val - _cur_min) / cur_width + _new_min;
}
