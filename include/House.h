#ifndef HOUSE_H
#define HOUSE_H

#include <goblin3d.h> // Include the Goblin3D library for 3D rendering
#include <TFT_eSPI.h> // Include the TFT display library

class House
{
public:
  House(TFT_eSPI &tft); // Constructor
  void setup();         // Initialize the house
  void update();        // Update and render the house

private:
  TFT_eSPI &_tft;        // Reference to the TFT display
  goblin3d_obj_t _house; // Goblin3D object for the house

  // Helper functions for rendering
  static void drawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
  static void eraseLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);

  // Static instance for callback context
  static House *_instance;
};

#endif