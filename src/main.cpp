#include <goblin3d.h> // Goblin3D library for 3D rendering
#include <I2C_BM8563.h>
#include "LSM6DS3.h"
#include <SD.h>
#include <SPI.h>
#include "display_conf.h"
#include "IMUDisplay.h"
#include "ObjModel.h"
#include "Wire.h"

// #define USE_ARDUINO_GFX_LIBRARY
#define USE_TFT_ESPI_LIBRARY
#include "lv_xiao_round_screen.h"

#define SD_CS_PIN D2
#define TFT_GREY 0xBDF7

I2C_BM8563 rtc(I2C_BM8563_DEFAULT_ADDRESS, Wire);
LSM6DS3 myIMU(I2C_MODE, 0x6A); // I2C device address 0x6A
TFT_eSprite img(&tft); // Sprite to be used as frame buffer
ObjModel objModel(img); // Create an instance of the ObjModel class, passing the sprite
IMUDisplay imuDisplay(img, myIMU); // Create an instance of the IMUDisplay class

unsigned long previousMillis = 0; // Store the previous time to calculate FPS
float fps = 0.0;                  // Variable to hold the FPS value

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

  // Initialize the IMU display
  imuDisplay.begin();
  
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

  // Update and display IMU data
  imuDisplay.update();
  // Update and display 3D model
  objModel.update();

  // Push the sprite to the TFT display
  img.pushSprite(0, 0);

  get_touch();
}