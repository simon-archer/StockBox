//Inside bitmapUtensils.cpp
#include "bitmapUtils.h"

void drawBitmap(int16_t x, int16_t y, const uint16_t bitmap[], int16_t w, int16_t h, Adafruit_ST7735 &tft) {
    tft.setAddrWindow(x, y, x + w - 1, y + h - 1);

    for(int16_t j = 0; j < h; j++) {
        for(int16_t i = 0; i < w; i++) {
            uint16_t color = bitmap[j * w + i];
            tft.pushColor(color);
        }
    }
}

void animateBitmap(int16_t &x, int16_t y, const uint16_t bitmap[], int16_t w, int16_t h, Adafruit_ST7735 &tft) {
    drawBitmap(x, y, bitmap, w, h, tft);
    delay(400); // wait for some time before erasing
    tft.fillRect(x, y, w, h, ST7735_WHITE);
    x++;
}

