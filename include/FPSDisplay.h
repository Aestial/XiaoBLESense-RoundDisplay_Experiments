#ifndef FPSDISPLAY_H
#define FPSDISPLAY_H

#include <TFT_eSPI.h>
#include "display_conf.h"

class FPSDisplay
{
public:
  FPSDisplay(TFT_eSprite &sprite);
  void update(unsigned long deltaMillis);

private:
  TFT_eSprite &_sprite; // Reference to the sprite for drawing
  float _fps = 0.0;     // Variable to hold the FPS value
  void drawFPS();
};

#endif