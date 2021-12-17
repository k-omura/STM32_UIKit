/*
 * stm32uikit.h
 *
 *  Created on: Dec 17, 2021
 *      Author: k-omura
 */


#ifndef DEF_STM32UIKIT_H
#define DEF_STM32UIKIT_H

#include "stdio.h"
#include "string.h"
#include "bitmap.h"
#include "bitmap_truetype.h"
#include "touch_2046.h"

#define SET_STRING 0
#define SET_PROGRESS 1
#define SET_BUTTON 2
#define SET_PIN 3

void stm32uikit_setAnchor(uint16_t, uint16_t);
void stm32uikit_componentSetgtings(uint8_t, uint8_t, uint8_t, uint16_t, uint8_t);

void stm32uikit_roundProgress(uint16_t, uint16_t);
void stm32uikit_rectProgress(uint16_t, uint16_t);
void stm32uikit_sllideBar(Coordinate_t, uint16_t, uint16_t *_val1000);
uint8_t stm32uikit_roundButton(Coordinate_t, uint16_t, uint8_t);

#endif
