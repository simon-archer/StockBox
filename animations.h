// animations.h
#include <stdint.h>
#ifndef _ANIMATIONS_H    // To make sure you don't declare the function more than once by including the header multiple times.
#define _ANIMATIONS_H

#define MOUTH_WIDTH  12   
#define MOUTH_HEIGHT 7

#define STILL_IMAGE_WIDTH  128   
#define STILL_IMAGE_HEIGHT 160

// Aftermarket Animation
extern const uint16_t mouth[MOUTH_WIDTH * MOUTH_HEIGHT];

//Aftermarket still image
extern const uint16_t aft_still_image[STILL_IMAGE_WIDTH * STILL_IMAGE_HEIGHT];

//Premarket still image
extern const uint16_t pre_still_image[STILL_IMAGE_WIDTH * STILL_IMAGE_HEIGHT];

#endif
