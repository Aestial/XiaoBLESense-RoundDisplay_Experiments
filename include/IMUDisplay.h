#ifndef IMUDISPLAY_H
#define IMUDISPLAY_H

#include <TFT_eSPI.h>
#include "LSM6DS3.h"
#include "display_conf.h"

class IMUDisplay
{
public:
  IMUDisplay(TFT_eSprite &sprite, LSM6DS3 &imu);
  void begin();
  void update();

private:
  TFT_eSprite &_sprite; // Reference to the sprite for drawing
  LSM6DS3 &_imu;        // Reference to the IMU sensor
  void drawAccelerometerData();
  void drawGyroscopeData();
  void drawThermometerData();
};

#endif