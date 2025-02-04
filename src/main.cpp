#include <goblin3d.h> // Goblin3D library for 3D rendering
#include <I2C_BM8563.h>
#include "LSM6DS3.h"
#include <SD.h>
#include <SPI.h>
#include "display_conf.h"
#include "FPSDisplay.h"
#include "IMUDisplay.h"
#include "ObjModel.h"
#include "TouchHandler.h"
#include "Wire.h"

#define SD_CS_PIN D2
#define TFT_GREY 0xBDF7

#define TOUCH_SENSITIVITY -0.35

I2C_BM8563 rtc(I2C_BM8563_DEFAULT_ADDRESS, Wire);
LSM6DS3 myIMU(I2C_MODE, 0x6A); // I2C device address 0x6A

TFT_eSprite img(&tft); // Sprite to be used as frame buffer
FPSDisplay fpsDisplay(img); // Create an instance of the FPSDisplay class
IMUDisplay imuDisplay(img, myIMU); // Create an instance of the IMUDisplay class
ObjModel objModel(img); // Create an instance of the ObjModel class, passing the sprite
TouchHandler touchHandler(img); // Create an instance of the TouchHandler class

unsigned long previousMillis = 0; // Store the previous time to calculate FPS
float fps = 0.0;                  // Variable to hold the FPS value

lv_coord_t touchX, touchY;

void setup()
{
  Serial.begin(115200);

  // Initialize the TFT display with black background color
  tft.init();
  tft.fillScreen(TFT_BLACK);
  
  // Initialize SD card after Round Screen initialization
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

  touchHandler.setup();

  Serial.println("Setup complete.");
}

void loop()
{
  unsigned long currentMillis = millis(); // Get the current time in milliseconds
  unsigned long deltaMillis = currentMillis - previousMillis; // Calculate the time difference for this frame
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
  // Update and handle touch input
  touchHandler.update();
  // Update and display 3D model
  objModel.rotateYaw(touchHandler.smoothed_delta_x * TOUCH_SENSITIVITY);
  objModel.update();
  // Push the sprite to the TFT display
  img.pushSprite(0, 0);

}