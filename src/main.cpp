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
ObjModel objModel(tft); // Create an instance of the ObjModel class
TFT_eSprite img = TFT_eSprite(&tft); // Sprite to be used as frame buffer

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
    objModel.setup(200.0, 120, 120); // Scale and center the model
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
  img.fillScreen(TFT_BLACK);
  //                                   // Display the FPS on the top left of the screen
  img.setTextColor(TFT_GREEN);       // Set text color to white with a black background
  img.setTextSize(1);                // Set text size to 2

  // Calculate text width (each character is 12 pixels wide at text size 2)
  char buffer[10];
  int textWidth = img.textWidth("FPS: 00.00", 2); // Example text width at size 2

  // Set cursor to bottom center
  img.setCursor(((MAX_IMAGE_WIDTH - textWidth) / 2) - 40, MAX_IMAGE_WIDTH - 24); // X position centered, Y position near the bottom
  sprintf(buffer, "FPS: %.2f", fps);
  img.print(buffer); // Print FPS to the screen

  // Display Accelerometer data
  Serial.print("\nAccelerometer:\n");
  Serial.print(" X1 = ");
  Serial.println(myIMU.readFloatAccelX(), 4);
  Serial.print(" Y1 = ");
  Serial.println(myIMU.readFloatAccelY(), 4);
  Serial.print(" Z1 = ");
  Serial.println(myIMU.readFloatAccelZ(), 4);
  img.setTextColor(TFT_SKYBLUE);
  img.setCursor(10, 10);
  img.print("Accel X: ");
  img.println(myIMU.readFloatAccelX(), 2);
  img.print("Accel Y: ");
  img.println(myIMU.readFloatAccelY(), 2);
  img.print("Accel Z: ");
  img.println(myIMU.readFloatAccelZ(), 2);

  // Display Gyroscope data
  Serial.print("\nGyroscope:\n");
  Serial.print(" X1 = ");
  Serial.println(myIMU.readFloatGyroX(), 4);
  Serial.print(" Y1 = ");
  Serial.println(myIMU.readFloatGyroY(), 4);
  Serial.print(" Z1 = ");
  Serial.println(myIMU.readFloatGyroZ(), 4);
  img.setTextColor(TFT_GREENYELLOW);
  img.setCursor(10, 60);
  img.print("Gyro X: ");
  img.println(myIMU.readFloatGyroX(), 2);
  img.print("Gyro Y: ");
  img.println(myIMU.readFloatGyroY(), 2);
  img.print("Gyro Z: ");
  img.println(myIMU.readFloatGyroZ(), 2);

  // Display Thermometer data
  Serial.print("\nThermometer:\n");
  Serial.print(" Degrees C1 = ");
  Serial.println(myIMU.readTempC(), 4);
  Serial.print(" Degrees F1 = ");
  Serial.println(myIMU.readTempF(), 4);
  img.setTextColor(TFT_ORANGE);
  img.setCursor(10, 110);
  img.print("Temp C: ");
  img.println(myIMU.readTempC(), 2);
  img.print("Temp F: ");
  img.println(myIMU.readTempF(), 2);

  objModel.update(); // Update and render the OBJ model
  // Push the sprite to the TFT display
  img.pushSprite(40, 0);
  
  // tft.endWrite(); // End the SPI transaction
  // delay(30);

  get_touch();
}