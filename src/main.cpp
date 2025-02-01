/**
 *
 * To create the images as C arrays, visit:
 * https://notisrac.github.io/FileToCArray/
 *
 */
#include <SPI.h>
#include <I2C_BM8563.h>

// #define USE_ARDUINO_GFX_LIBRARY
#define USE_TFT_ESPI_LIBRARY
#include "lv_xiao_round_screen.h"

#include "LSM6DS3.h"
#include "Wire.h"

#include "bone.c"
#include "image.h"

#define MAX_IMAGE_WIDTH 240

int tftStatus = 0;

I2C_BM8563 rtc(I2C_BM8563_DEFAULT_ADDRESS, Wire);
const int chipSelect = D2;

TFT_eSprite face = TFT_eSprite(&tft);

#define CLOCK_R 260.0f
#define TFT_GREY 0xBDF7

LSM6DS3 myIMU(I2C_MODE, 0x6A); // I2C device address 0x6A

float sx = 0, sy = 1, mx = 1, my = 0, hx = -1, hy = 0;
float sdeg = 0, mdeg = 0, hdeg = 0;
uint16_t osx = 120, osy = 120, omx = 120, omy = 120, ohx = 120, ohy = 120;
uint16_t x0 = 0, x1 = 0, yy0 = 0, yy1 = 0;
uint32_t targetTime = 0;

bool initial = 1;

void draw_clock()
{
  tft.init();
  tft.setRotation(0);
  tft.fillScreen(TFT_GREY);
  tft.setTextColor(TFT_GREEN, TFT_GREY);

  tft.fillCircle(120, 120, 122, TFT_GOLD);
  tft.fillCircle(120, 120, 115, TFT_BLACK);

  for (int i = 0; i < 360; i += 30)
  {
    sx = cos((i - 90) * 0.0174532925);
    sy = sin((i - 90) * 0.0174532925);
    x0 = sx * 115 + 120;
    yy0 = sy * 115 + 120;
    x1 = sx * 105 + 120;
    yy1 = sy * 105 + 120;

    tft.drawLine(x0, yy0, x1, yy1, TFT_BLUE);
  }

  for (int i = 0; i < 360; i += 6)
  {
    sx = cos((i - 90) * 0.0174532925);
    sy = sin((i - 90) * 0.0174532925);
    x0 = sx * 110 + 120;
    yy0 = sy * 110 + 120;

    tft.drawPixel(x0, yy0, TFT_BLUE);
    if (i == 0 || i == 180)
      tft.fillCircle(x0, yy0, 1, TFT_CYAN);
    if (i == 0 || i == 180)
      tft.fillCircle(x0 + 1, yy0, 1, TFT_CYAN);
    if (i == 90 || i == 270)
      tft.fillCircle(x0, yy0, 1, TFT_CYAN);
    if (i == 90 || i == 270)
      tft.fillCircle(x0 + 1, yy0, 1, TFT_CYAN);
  }

  tft.fillCircle(120, 120, 3, TFT_RED);

  targetTime = millis() + 1000;
}

void draw_clock_time()
{
  I2C_BM8563_DateTypeDef dateStruct;
  I2C_BM8563_TimeTypeDef timeStruct;

  rtc.getDate(&dateStruct);
  rtc.getTime(&timeStruct);

  int ss = timeStruct.seconds;
  int mm = timeStruct.minutes;
  int hh = timeStruct.hours;
  int dd = dateStruct.date;
  int mmnth = dateStruct.month;

  String date = (mmnth < 10 ? "0" : "") + String(mmnth) + "-" + (dd < 10 ? "0" : "") + String(dd);

  tft.setTextSize(2);
  tft.setTextDatum(TFT_TRANSPARENT);
  tft.setTextColor(TFT_BLUE);
  tft.drawString(date, CLOCK_R / 2, CLOCK_R * 0.5 - 30);

  tft.setTextSize(2);
  tft.setTextDatum(TFT_TRANSPARENT);
  tft.setTextColor(TFT_WHITE);
  tft.drawString("TFT_test", CLOCK_R / 2, CLOCK_R * 0.5);

  if (targetTime < millis())
  {
    targetTime = millis() + 1000;

    int sdeg = ss * 6;
    int mdeg = mm * 6 + sdeg * 0.01666667;
    int hdeg = hh * 30 + mdeg * 0.0833333;

    float hx = cos((hdeg - 90) * 0.0174532925);
    float hy = sin((hdeg - 90) * 0.0174532925);
    float mx = cos((mdeg - 90) * 0.0174532925);
    float my = sin((mdeg - 90) * 0.0174532925);
    float sx = cos((sdeg - 90) * 0.0174532925);
    float sy = sin((sdeg - 90) * 0.0174532925);

    if (ss == 0 || initial)
    {
      initial = 0;
      tft.drawLine(ohx, ohy, 120, 120, TFT_BLACK);
      ohx = hx * 50 + 120;
      ohy = hy * 50 + 120;
      tft.drawLine(omx, omy, 120, 120, TFT_BLACK);
      omx = mx * 70 + 120;
      omy = my * 70 + 120;
    }

    tft.drawLine(osx, osy, 120, 120, TFT_BLACK);
    tft.drawLine(ohx, ohy, 120, 120, TFT_WHITE);
    tft.drawLine(omx, omy, 120, 120, TFT_WHITE);
    osx = sx * 85 + 120;
    osy = sy * 85 + 120;
    tft.drawLine(osx, osy, 120, 120, TFT_RED);

    tft.fillCircle(120, 120, 3, TFT_RED);
  }
  delay(100);
}

void get_touch()
{
  lv_coord_t touchX, touchY;

  if (chsc6x_is_pressed())
  {
    chsc6x_get_xy(&touchX, &touchY);
    if (touchX > 240 || touchY > 240)
    {
      touchX = 0;
      touchY = 0;
    }
    Serial.print("Touch coordinates: X = ");
    Serial.print(touchX);
    Serial.print(", Y = ");
    Serial.println(touchY);
    tft.fillRect(10, 100, 90, 18, TFT_BLACK);
    char buffer1[20];
    sprintf(buffer1, "%d,%d", touchX, touchY);
    tft.drawString(buffer1, 10, 100);
  }
}

void setup()
{
  Serial.begin(115200);
  Serial.println("\n\nUsing the PNGdec library with touch interaction");

  if (myIMU.begin() != 0)
  {
    Serial.println("Device error");
  }
  else
  {
    Serial.println("Device OK!");
  }

  lv_xiao_touch_init();

  draw_clock();

  Serial.println("Setup complete.");
}

void loop()
{
  // Accelerometer
  Serial.print("\nAccelerometer:\n");
  Serial.print(" X1 = ");
  Serial.println(myIMU.readFloatAccelX(), 4);
  Serial.print(" Y1 = ");
  Serial.println(myIMU.readFloatAccelY(), 4);
  Serial.print(" Z1 = ");
  Serial.println(myIMU.readFloatAccelZ(), 4);

  tft.setTextSize(1);
  tft.setTextDatum(TFT_TRANSPARENT);
  tft.setTextColor(TFT_SKYBLUE);
  tft.setCursor(MAX_IMAGE_WIDTH/2, 10);
  tft.print("\nAccelerometer:\n");
  tft.print(" X1 = ");
  tft.println(myIMU.readFloatAccelX(), 4);
  tft.print(" Y1 = ");
  tft.println(myIMU.readFloatAccelY(), 4);
  tft.print(" Z1 = ");
  tft.println(myIMU.readFloatAccelZ(), 4);

  // Gyroscope
  Serial.print("\nGyroscope:\n");
  Serial.print(" X1 = ");
  Serial.println(myIMU.readFloatGyroX(), 4);
  Serial.print(" Y1 = ");
  Serial.println(myIMU.readFloatGyroY(), 4);
  Serial.print(" Z1 = ");
  Serial.println(myIMU.readFloatGyroZ(), 4);

  tft.setTextSize(1);
  tft.setTextDatum(TFT_TRANSPARENT);
  tft.setTextColor(TFT_GREENYELLOW);
  tft.setCursor(MAX_IMAGE_WIDTH/2, 60);
  tft.print("\nGyroscope:\n");
  tft.print(" X1 = ");
  tft.println(myIMU.readFloatGyroX(), 4);
  tft.print(" Y1 = ");
  tft.println(myIMU.readFloatGyroY(), 4);
  tft.print(" Z1 = ");
  tft.println(myIMU.readFloatGyroZ(), 4);

  // Thermometer
  Serial.print("\nThermometer:\n");
  Serial.print(" Degrees C1 = ");
  Serial.println(myIMU.readTempC(), 4);
  Serial.print(" Degrees F1 = ");
  Serial.println(myIMU.readTempF(), 4);

  tft.setTextSize(1);
  tft.setTextDatum(TFT_TRANSPARENT);
  tft.setTextColor(TFT_ORANGE);
  tft.setCursor(MAX_IMAGE_WIDTH/2, 130);
  tft.print("\nThermometer:\n");
  tft.print(" Degrees C1 = ");
  tft.println(myIMU.readTempC(), 4);
  tft.print(" Degrees F1 = ");
  tft.println(myIMU.readTempF(), 4);

  delay(50);

  draw_clock_time();
  get_touch();
}