//Inside bitmapUtensils.h
#ifndef BITMAPUTILS_H
#define BITMAPUTILS_H

#include <Adafruit_ST7735.h>

void drawBitmap(int16_t x, int16_t y, const uint16_t bitmap[], int16_t w, int16_t h, Adafruit_ST7735 &tft);

void animateBitmap(int16_t &x, int16_t y, const uint16_t bitmap[], int16_t w, int16_t h, Adafruit_ST7735 &tft);

#endif