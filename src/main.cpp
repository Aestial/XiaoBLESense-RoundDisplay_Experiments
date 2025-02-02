#include <goblin3d.h> // Include the Goblin3D library for 3D rendering
#include <I2C_BM8563.h>
#include "LSM6DS3.h"
#include <SD.h>
#include <SPI.h>
#include "ObjModel.h"
#include "Wire.h"

// #define USE_ARDUINO_GFX_LIBRARY
#define USE_TFT_ESPI_LIBRARY
#include "lv_xiao_round_screen.h"

#define MAX_IMAGE_WIDTH 240
#define SD_CS_PIN D2
#define TFT_GREY 0xBDF7

I2C_BM8563 rtc(I2C_BM8563_DEFAULT_ADDRESS, Wire);
LSM6DS3 myIMU(I2C_MODE, 0x6A); // I2C device address 0x6A
TFT_eSprite img(&tft); // Sprite to be used as frame buffer
ObjModel objModel(img); // Create an instance of the ObjModel class, passing the sprite

unsigned long previousMillis = 0; // Store the previous time to calculate FPS
float fps = 0.0;                  // Variable to hold the FPS value

bool initial = 1;
int tftStatus = 0;

lv_coord_t touchX, touchY;

void get_touch()
{
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

  // Initialize the TFT display with black background color
  tft.init();
  tft.fillScreen(TFT_BLACK);

  Serial.print("Initializing SD card...");

  pinMode(D2, OUTPUT);
  if (!SD.begin(D2))
  {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");

  // Load and initialize the OBJ model
  if (objModel.load("monkey.obj"))
  {
    objModel.setup(150.0, 120, 140); // Scale and center the model
  }
  else
  {
    Serial.println("Failed to load OBJ model.");
    while (true)
      ; // Halt execution if loading fails
  }

  // Initialize the sprite image
  img.createSprite(MAX_IMAGE_WIDTH, MAX_IMAGE_WIDTH);

  if (myIMU.begin() != 0)
  {
    Serial.println("Device error");
  }
  else
  {
    Serial.println("Device OK!");
  }

  lv_xiao_touch_init();

  Serial.println("Setup complete.");
}

void loop()
{

  unsigned long currentMillis = millis(); // Get the current time in milliseconds

  // Calculate the time difference for this frame
  unsigned long deltaMillis = currentMillis - previousMillis;
  previousMillis = currentMillis;

  // Calculate FPS as the inverse of frame time (in seconds)
  if (deltaMillis > 0)
    fps = 1000.0 / deltaMillis;

  // tft.startWrite();
  // Clear the sprite and draw the overlay
  img.fillScreen(TFT_BLACK);

  // Draw a border around the screen for better visual separation
  img.drawRect(0, 0, MAX_IMAGE_WIDTH, MAX_IMAGE_WIDTH, TFT_WHITE);

  // Display FPS at the top center
  img.setTextColor(TFT_GREEN);
  img.setTextSize(1);
  char buffer[10];
  sprintf(buffer, "FPS: %.2f", fps);
  int fpsTextWidth = img.textWidth(buffer);
  img.setCursor((MAX_IMAGE_WIDTH - fpsTextWidth) / 2, 5); // Center the FPS text
  img.print(buffer);

  // Draw a horizontal line to separate FPS from sensor data
  img.drawFastHLine(10, 20, MAX_IMAGE_WIDTH - 20, TFT_WHITE);

  // Display Accelerometer data in the top-left section
  img.setTextColor(TFT_CYAN);
  img.setCursor(MAX_IMAGE_WIDTH / 2 - 60, 25);
  img.print("Accel:");
  img.setCursor(MAX_IMAGE_WIDTH / 2 - 60, 35);
  img.print("X: ");
  img.println(myIMU.readFloatAccelX(), 2);
  img.setCursor(MAX_IMAGE_WIDTH / 2 - 60, 45);
  img.print("Y: ");
  img.println(myIMU.readFloatAccelY(), 2);
  img.setCursor(MAX_IMAGE_WIDTH / 2 - 60, 55);
  img.print("Z: ");
  img.println(myIMU.readFloatAccelZ(), 2);

  // Draw a vertical line to separate accelerometer and gyroscope data
  img.drawFastVLine(MAX_IMAGE_WIDTH / 2, 20, 60, TFT_WHITE);

  // Display Gyroscope data in the top-right section
  img.setTextColor(TFT_GREENYELLOW);
  img.setCursor(MAX_IMAGE_WIDTH / 2 + 10, 25);
  img.print("Gyro:");
  img.setCursor(MAX_IMAGE_WIDTH / 2 + 10, 35);
  img.print("X: ");
  img.println(myIMU.readFloatGyroX(), 2);
  img.setCursor(MAX_IMAGE_WIDTH / 2 + 10, 45);
  img.print("Y: ");
  img.println(myIMU.readFloatGyroY(), 2);
  img.setCursor(MAX_IMAGE_WIDTH / 2 + 10, 55);
  img.print("Z: ");
  img.println(myIMU.readFloatGyroZ(), 2);

  // Draw a horizontal line to separate gyroscope and thermometer data
  img.drawFastHLine(10, 80, MAX_IMAGE_WIDTH - 20, TFT_WHITE);

  // Display Thermometer data at the bottom
  img.setTextColor(TFT_ORANGE);
  img.setCursor(MAX_IMAGE_WIDTH / 2 - 30, MAX_IMAGE_WIDTH - 30);
  img.print("Temp:");
  img.setCursor(MAX_IMAGE_WIDTH / 2 - 30, MAX_IMAGE_WIDTH - 20);
  img.print("C: ");
  img.println(myIMU.readTempC(), 2);
  img.setCursor(MAX_IMAGE_WIDTH / 2 - 30, MAX_IMAGE_WIDTH - 10);
  img.print("F: ");
  img.println(myIMU.readTempF(), 2);

  objModel.update();

  // Push the sprite to the TFT display
  img.pushSprite(0, 0);

  // tft.endWrite(); // End the SPI transaction
  // delay(30);

  get_touch();
}