#include "FPSDisplay.h"

FPSDisplay::FPSDisplay(TFT_eSprite &sprite)
    : _sprite(sprite) {}

void FPSDisplay::update(unsigned long deltaMillis)
{
  // Calculate FPS as the inverse of frame time (in seconds)
  if (deltaMillis > 0)
    _fps = 1000.0 / deltaMillis;

  // Draw the FPS value
  drawFPS();
}

void FPSDisplay::drawFPS()
{
  _sprite.setTextColor(TFT_GREEN);
  _sprite.setTextSize(1);
  char buffer[10];
  sprintf(buffer, "FPS: %.2f", _fps);
  int width = _sprite.textWidth(buffer);
  _sprite.setCursor((MAX_IMAGE_WIDTH - width) / 2, 5); // Center the FPS text
  _sprite.print(buffer);
}