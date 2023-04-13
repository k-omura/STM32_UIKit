## STM32_UIKit & TrueType renderer  
A lightweight kit that uses a screen and touch panel with STM32.  
Manipulate the 8bit/1pixel frame buffer.  

## Contents
- Display ILI9341 conotrol(FSMC)
- Touch Panel XPT2046
- Bitmap Control
  - Line
  - Rect
  - Character
    - Bitmap
    - TrueType (Porting [my Arduino library](https://github.com/k-omura/truetype_Arduino))
- UIKit that integrates the above

## Demos
- [STM32_UIkit](https://youtu.be/HiLC_AcfrQw)  
- [TrueType](https://youtu.be/KPYnb5hfLiA)  

## Structure  
![structure](https://user-images.githubusercontent.com/26690530/149136722-b78f821b-959c-45d4-a238-acdd53963ac4.png)

## STM32_UIKit Quick start  
The following is the basic idea. See [example](https://github.com/k-omura/STM32_UIKit/tree/main/examples/STM32F407VET6/STM32_UIkit) for full code.  
```
#include <FSMC_ILI9341.h>
#include <bitmap.h>
#include <touch_2046.h>
#include <stm32uikit.h>

#define COLOR_BACKGROUND 0x00

uint8_t frameBuffer[ILI9341_PIXEL_COUNT] = {0};

int main(void){
  //touch panel init
  Coordinate_t touch_s3uikit;

  //display init
  ILI9341_init();
  ILI9341_setRotation(1);

  //bitmap init
  char string[30];
  bitmap_setparam(DISPLAY_WIDTH, DISPLAY_HEIGHT, COLOR_BACKGROUND, frameBuffer); //Set framebuffer size, background color, framebuffer pointer
  bitmap_clear();

  //loop
  while (1) {
    //get touched point
    touch_s3uikit = xpt2046_read(&hspi2, touch_cal);

    //set components. for example,,,
    stm32uikit_sllideBar(touch_s3uikit, 10, 100, 190, &slide_val);

    ILI9341_printBitmap(frameBuffer); //flush framebuffer to display
    bitmap_clear(); //clear bitmap
  }
}
```

## STM32_UIKit Components  
### Progress Bar   
`void stm32uikit_rectProgress(uint16_t x0, uint16_t y0, uint16_t width, uint16_t val1000)`
![fig1](https://user-images.githubusercontent.com/26690530/149614485-237fb8c0-1da2-45e5-992d-009d128f96f8.png)
`void stm32uikit_roundProgress(uint16_t x0, uint16_t y0, uint16_t width, uint16_t val1000)`
![fig2](https://user-images.githubusercontent.com/26690530/149614486-467eb09d-780e-4531-b1a0-9d49c0ea4398.png)

### Analog Meter  
`void stm32uikit_analogMeter(uint16_t x0, uint16_t y0, uint16_t val1000)`
![fig3](https://user-images.githubusercontent.com/26690530/149614503-be96e9c1-db25-4a47-80f4-dc3c8686e9a5.png)

### Circle Meter  
`void stm32uikit_circleMeter(uint16_t x0, uint16_t y0, uint16_t val1000, uint16_t thickness)`
![fig4](https://user-images.githubusercontent.com/26690530/231768426-46fd6487-e749-4b4f-af33-4a5843ae4e47.png)

### Status  
`void stm32uikit_status(uint16_t x0, uint16_t y0, uint16_t status)`
![fig5](https://user-images.githubusercontent.com/26690530/149614531-82f94cef-75b9-4a07-99c2-e5914dd12fd0.png)

### Slide Bar  
`void stm32uikit_sllideBar(Coordinate_t touch, uint16_t x0, uint16_t y0, uint16_t width, uint16_t *val1000)`  
![fig6](https://user-images.githubusercontent.com/26690530/149614569-66519ddd-27da-4bae-a5ca-64e6a31ea8ed.png)

### Button  
`void stm32uikit_roundButton(Coordinate_t touch, uint16_t x0, uint16_t y0, uint16_t width, uint8_t *val)`
![fig7](https://user-images.githubusercontent.com/26690530/149648569-fff986e2-02de-40b8-8fa9-3389936388a4.png)

### Switch  
`void stm32uikit_switch(Coordinate_t touch, uint16_t x0, uint16_t y0, uint8_t *val)`
![fig8](https://user-images.githubusercontent.com/26690530/149614646-81487eb7-7633-4655-8701-57bb8466bde8.png)

#### Input structure of a component with input  
Touched point.  
```
typedef struct {
   uint16_t x;
   uint16_t y;
   uint16_t z;
} Coordinate_t;
```
If there is no z-axis information, increase the z value to more than 300 when touching.  

## TrueType Quick start  
The following is the basic idea. See [example](https://github.com/k-omura/STM32_UIKit/tree/main/examples/STM32F407VET6/truetype) for full code.  
```
#include <FSMC_ILI9341.h>
#include <bitmap.h>

#define COLOR_BACKGROUND 0x00

uint8_t frameBuffer[ILI9341_PIXEL_COUNT] = {0};

int main(void){
  //display init
  ILI9341_init();
  ILI9341_setRotation(1);

  //bitmap init
  char string[30];
  bitmap_setparam(DISPLAY_WIDTH, DISPLAY_HEIGHT, COLOR_BACKGROUND, frameBuffer); //Set framebuffer size, background color, framebuffer pointer
  bitmap_clear();

  //SD FatFs init
  if(f_mount(&bitmap_truetype_fs.FatFs, "", 1) != FR_OK){
    while(1);
  }
  int total_sect = (bitmap_truetype_fs.FatFs.n_fatent - 2) * bitmap_truetype_fs.FatFs.csize / 2048;
  sprintf(string, " ->Total: %dMB", total_sect);

  bitmap_terminal("SD Mount: Successfully", 0, 0xff, TERMINAL_LINE_MAX);
  bitmap_terminal(string, 0, 0xff, TERMINAL_LINE_MAX);
  ILI9341_printBitmap(frameBuffer);

  //open ttf file
  if(f_open(&bitmap_truetype_fs.File, "/fonts/font.ttf", FA_OPEN_EXISTING | FA_READ) != FR_OK){
    while(1);
  }
  bitmap_terminal("File open: Successfully", 0, 0xff, TERMINAL_LINE_MAX);
  ILI9341_printBitmap(frameBuffer);
  HAL_Delay(1000);

  //Truetype init
  uint8_t res = truetype_setTtfFile(0);
  sprintf(string, "setTtfFile: %d", res);
  bitmap_terminal(string, 0, 0xff, TERMINAL_LINE_MAX);
  ILI9341_printBitmap(frameBuffer);

  truetype_setCharacterSize(40); //set height
  truetype_setCharacterSpacing(0, 1); //set Character Spacing. (characterSpace, kerningOn)
  truetype_setTextBoundary(0, 280, 320); //set line feed position. (start_x, end_x, end_y)
  truetype_setTextColor(0xff, 0xff, 1); //set text color. (in, out, fillingOn)
  truetype_setTextRotation(0); //set text rotation

  //loop
  while (1) {
    //Truetype countup test
    for(uint16_t i = 1; i <= 1000; i++){
      sprintf(string, "%04d", i);
      truetype_textDraw(80, 5, string);

      ILI9341_printBitmap(frameBuffer);
      bitmap_clear();
    }
  }
}
```

## Known issues  
- Nothing now.  

## Notes  
- Set "SDIOCLK Clock divide factor" appropriately. If the clock is too fast, it will fail to access SD and stop working.  

## Development environment
- [STM32F407VET6 Black](https://stm32-base.org/boards/STM32F407VET6-STM32-F4VE-V2.0)
- ILI9341 FSMC Display with touch panel for STM32F407VET6 Black. [for example this](https://www.ebay.com/itm/322979958874)
