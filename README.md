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

## Structure  
![structure](https://user-images.githubusercontent.com/26690530/149136722-b78f821b-959c-45d4-a238-acdd53963ac4.png)

## STM32_UIKit Components  
### Progress Bar   
`void stm32uikit_rectProgress(uint16_t x0, uint16_t y0, uint16_t width, uint16_t val1000)`
![fig1](https://user-images.githubusercontent.com/26690530/149614485-237fb8c0-1da2-45e5-992d-009d128f96f8.png)
`void stm32uikit_roundProgress(uint16_t x0, uint16_t y0, uint16_t width, uint16_t val1000)`
![fig2](https://user-images.githubusercontent.com/26690530/149614486-467eb09d-780e-4531-b1a0-9d49c0ea4398.png)

### Analog Meter  
`void stm32uikit_analogMeter(uint16_t x0, uint16_t y0, uint16_t val1000)`
![fig3](https://user-images.githubusercontent.com/26690530/149614503-be96e9c1-db25-4a47-80f4-dc3c8686e9a5.png)

### Status  
`void stm32uikit_status(uint16_t x0, uint16_t y0, uint16_t status)`
![fig4](https://user-images.githubusercontent.com/26690530/149614531-82f94cef-75b9-4a07-99c2-e5914dd12fd0.png)

### Slide Bar  
`void stm32uikit_sllideBar(Coordinate_t touch, uint16_t x0, uint16_t y0, uint16_t width, uint16_t *val1000)`  
![fig5](https://user-images.githubusercontent.com/26690530/149614569-66519ddd-27da-4bae-a5ca-64e6a31ea8ed.png)

### Button  
`void stm32uikit_roundButton(Coordinate_t touch, uint16_t x0, uint16_t y0, uint16_t width, uint8_t *val)`
![fig6](https://user-images.githubusercontent.com/26690530/149614597-0a0ec7d4-7f33-4fef-a5b8-0b72003b6fce.png)

### Switch  
`void stm32uikit_switch(Coordinate_t touch, uint16_t x0, uint16_t y0, uint8_t *val)`
![fig7](https://user-images.githubusercontent.com/26690530/149614646-81487eb7-7633-4655-8701-57bb8466bde8.png)

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

## Development environment
- [STM32F407VET6 Black](https://stm32-base.org/boards/STM32F407VET6-STM32-F4VE-V2.0)
- ILI9341 FSMC Display with touch panel for STM32F407VET6 Black. [for example this](https://ebay.us/9OyquX)
