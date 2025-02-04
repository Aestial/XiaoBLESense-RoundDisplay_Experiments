#include <goblin3d.h> // Goblin3D library for 3D rendering
#include <I2C_BM8563.h>
#include "LSM6DS3.h"
#include <SD.h>
#include <SPI.h>
#include "display_conf.h"
#include "FPSDisplay.h"
#include "IMUDisplay.h"
#include "ObjModel.h"
#include "Wire.h"

#include "BLEManager.h"

#define SD_CS_PIN D2
#define TFT_GREY 0xBDF7

// #define USE_ARDUINO_GFX_LIBRARY
#define USE_TFT_ESPI_LIBRARY
#include "lv_xiao_round_screen.h"

#define TOUCH_SENSITIVITY -0.35

I2C_BM8563 rtc(I2C_BM8563_DEFAULT_ADDRESS, Wire);
LSM6DS3 myIMU(I2C_MODE, 0x6A); // I2C device address 0x6A

TFT_eSprite img(&tft); // Sprite to be used as frame buffer
FPSDisplay fpsDisplay(img); // Create an instance of the FPSDisplay class
IMUDisplay imuDisplay(img, myIMU); // Create an instance of the IMUDisplay class
ObjModel objModel(img, "cupcake.obj"); // Create an instance of the ObjModel class, passing the sprite
BLEManager bleManager;  // Create an instance of the BLEManager class

// Store the previous time to calculate FPS
unsigned long previousMillis = 0;

// Variables to store touch coordinates
int16_t touchX_prev = 0;
int16_t touchY_prev = 0;
int16_t previous_delta_x = 0;
int16_t smoothed_delta_x = 0;
bool touched = false;
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

    if (touched) {
      int16_t delta_x = touchX - touchX_prev;

      smoothed_delta_x = (delta_x + previous_delta_x) / 2;
      previous_delta_x = smoothed_delta_x;

      // Print delta X for debugging
      Serial.print("Smoothed delta X: ");
      Serial.println(smoothed_delta_x);

      // Determine yaw rotation based on delta X
      if (smoothed_delta_x > 0)
      {
        Serial.println("Rotating right");
      }
      else if (smoothed_delta_x < 0)
      {
        Serial.println("Rotating left");
      }
    }

    // Update previous coordinates
    touchX_prev = touchX;
    touchY_prev = touchY;
    touched = true; // Mark that touch has been detected

    img.setTextColor(TFT_MAGENTA);
    img.setTextSize(1);
    char buffer1[20];
    sprintf(buffer1, "%d,%d", touchX, touchY);
    img.drawString(buffer1, 10, 100);
  }

  else
  {
    // No touch detected
    touched = false;
    smoothed_delta_x = 0.0;
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

  // Initialize the sprite image
  img.createSprite(MAX_IMAGE_WIDTH, MAX_IMAGE_WIDTH);
  // Initialize the IMU display
  imuDisplay.begin();

  objModel.setup(150.0, 120, 140); // Scale and center the model

  // Initialize BLE and set the callback for model name updates
  bleManager.begin("3D-Model-Updater", [](const std::string &modelName)
  {
    objModel.setModelName(modelName); // Update the model name dynamically
  });

  lv_xiao_touch_init();

  Serial.println("Setup complete.");
}

void loop()
{
  unsigned long currentMillis = millis(); // Get the current time in milliseconds
  // Calculate the time difference for this frame
  unsigned long deltaMillis = currentMillis - previousMillis;
  previousMillis = currentMillis;

  // Clear the sprite and draw the overlay
  img.fillScreen(TFT_BLACK);

  // TODO: Draw circular border

  // Update and display FPS
  fpsDisplay.update(deltaMillis);
  // Draw a horizontal line to separate FPS from sensor data
  img.drawFastHLine(10, 20, MAX_IMAGE_WIDTH - 20, TFT_WHITE);

  // Update and display IMU data
  imuDisplay.update();
  // Update and display 3D model
  objModel.rotateYaw(smoothed_delta_x * TOUCH_SENSITIVITY);
  objModel.update();

  get_touch();

  // Push the sprite to the TFT display
  img.pushSprite(0, 0);

  // Update BLE
  bleManager.update();

  delay(25);
}