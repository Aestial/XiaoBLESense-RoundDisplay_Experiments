#ifndef TOUCHHANDLER_H
#define TOUCHHANDLER_H

#include <TFT_eSPI.h>
#include "display_conf.h"
// #define USE_ARDUINO_GFX_LIBRARY
#define USE_TFT_ESPI_LIBRARY
#include "lv_xiao_round_screen.h"

class TouchHandler
{
public:
  TouchHandler(TFT_eSprite &sprite);
  void setup();
  void update();
  u_int16_t smoothed_delta_x;

private:
  TFT_eSprite &_sprite;        // Reference to the sprite for drawing
  lv_coord_t _touchX, _touchY; // Variables to store touch coordinates
  void displayTouchCoordinates();
};

#endif