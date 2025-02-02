#include "IMUDisplay.h"

IMUDisplay::IMUDisplay(TFT_eSprite &sprite, LSM6DS3 &imu)
    : _sprite(sprite), _imu(imu) {}

void IMUDisplay::begin()
{
  if (_imu.begin() != 0)
  {
    Serial.println("IMU device error");
  }
  else
  {
    Serial.println("IMU device OK!");
  }
}

void IMUDisplay::update()
{
  // Draw Accelerometer data
  drawAccelerometerData();

  // Draw a vertical line to separate accelerometer and gyroscope data
  _sprite.drawFastVLine(MAX_IMAGE_WIDTH / 2, 20, 60, TFT_WHITE);

  // Draw Gyroscope data
  drawGyroscopeData();

  // Draw a horizontal line to separate gyroscope and thermometer data
  _sprite.drawFastHLine(10, 80, MAX_IMAGE_WIDTH - 20, TFT_WHITE);

  // Draw Thermometer data
  drawThermometerData();

  // Draw a horizontal line to separate gyroscope and thermometer data
  _sprite.drawFastHLine(20, MAX_IMAGE_WIDTH - 35, MAX_IMAGE_WIDTH - 40, TFT_WHITE);
}

void IMUDisplay::drawAccelerometerData()
{
  _sprite.setTextColor(TFT_CYAN);
  _sprite.setCursor(MAX_IMAGE_WIDTH / 2 - 60, 25);
  _sprite.print("Accel:");
  _sprite.setCursor(MAX_IMAGE_WIDTH / 2 - 60, 35);
  _sprite.print("X: ");
  _sprite.println(_imu.readFloatAccelX(), 2);
  _sprite.setCursor(MAX_IMAGE_WIDTH / 2 - 60, 45);
  _sprite.print("Y: ");
  _sprite.println(_imu.readFloatAccelY(), 2);
  _sprite.setCursor(MAX_IMAGE_WIDTH / 2 - 60, 55);
  _sprite.print("Z: ");
  _sprite.println(_imu.readFloatAccelZ(), 2);
}

void IMUDisplay::drawGyroscopeData()
{
  _sprite.setTextColor(TFT_GREENYELLOW);
  _sprite.setCursor(MAX_IMAGE_WIDTH / 2 + 10, 25);
  _sprite.print("Gyro:");
  _sprite.setCursor(MAX_IMAGE_WIDTH / 2 + 10, 35);
  _sprite.print("X: ");
  _sprite.println(_imu.readFloatGyroX(), 2);
  _sprite.setCursor(MAX_IMAGE_WIDTH / 2 + 10, 45);
  _sprite.print("Y: ");
  _sprite.println(_imu.readFloatGyroY(), 2);
  _sprite.setCursor(MAX_IMAGE_WIDTH / 2 + 10, 55);
  _sprite.print("Z: ");
  _sprite.println(_imu.readFloatGyroZ(), 2);
}

void IMUDisplay::drawThermometerData()
{
  _sprite.setTextColor(TFT_ORANGE);
  _sprite.setCursor(MAX_IMAGE_WIDTH / 2 - 25, MAX_IMAGE_WIDTH - 30);
  _sprite.print("Temp:");
  _sprite.setCursor(MAX_IMAGE_WIDTH / 2 - 25, MAX_IMAGE_WIDTH - 20);
  _sprite.print("C: ");
  _sprite.println(_imu.readTempC(), 2);
  _sprite.setCursor(MAX_IMAGE_WIDTH / 2 - 25, MAX_IMAGE_WIDTH - 10);
  _sprite.print("F: ");
  _sprite.println(_imu.readTempF(), 2);
}