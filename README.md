## STM32_UIKit & TrueType renderer  
A lightweight kit that uses a screen and touch panel with STM32.

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


## Development environment
- [STM32F407VET6 Black](https://stm32-base.org/boards/STM32F407VET6-STM32-F4VE-V2.0)
- ILI9341 FSMC Display with touch panel for STM32F407VET6 Black. [for example this](https://ebay.us/9OyquX)
