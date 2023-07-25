// Inside premarket.cpp file
#include <Adafruit_ST7735.h>
#include "animations.h"
#include "bitmapUtils.h"

void preScreen(Adafruit_ST7735 &tft) {
    tft.setRotation(2); // Adjust the orientation of the display
    tft.fillScreen(ST7735_BLACK);
    drawBitmap(0, 0, pre_still_image, STILL_IMAGE_WIDTH, STILL_IMAGE_HEIGHT, tft);

    tft.setRotation(3); // Rotate text 90 degrees
    tft.setTextSize(2); // Set label size to 2
    tft.setTextColor(ST7735_WHITE);
    tft.setTextWrap(false); // Disable text wrapping
    tft.setCursor(2,90);
    tft.println("The Market Is");
    tft.setCursor(10, 110);
    tft.println("Not Open Yet");
    tft.setRotation(2); // Rotate text 90 degrees

}