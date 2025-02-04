#include "TouchHandler.h"

TouchHandler::TouchHandler(TFT_eSprite &sprite)
    : _sprite(sprite), _touchX(0), _touchY(0) {}

void TouchHandler::setup()
{
  lv_xiao_touch_init();
}

void TouchHandler::update()
{
  if (chsc6x_is_pressed())
  {
    chsc6x_get_xy(&_touchX, &_touchY);
    if (_touchX > 240 || _touchY > 240)
    {
      _touchX = 0;
      _touchY = 0;
    }
    Serial.print("Touch coordinates: X = ");
    Serial.print(_touchX);
    Serial.print(", Y = ");
    Serial.println(_touchY);
    displayTouchCoordinates();

    // TODO: process following
    smoothed_delta_x = _touchX
  }
}

void TouchHandler::displayTouchCoordinates()
{
  _sprite.setTextColor(TFT_MAGENTA);
  _sprite.setTextSize(1);
  char buffer[20];
  sprintf(buffer, "%d,%d", _touchX, _touchY);
  _sprite.setCursor(10, 110); // Left middle FPS text
  _sprite.print(buffer);
}