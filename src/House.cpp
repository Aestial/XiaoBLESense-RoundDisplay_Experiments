#include "House.h"

// Initialize the static instance pointer
House *House::_instance = nullptr;

// Constructor
House::House(TFT_eSPI &tft) : _tft(tft)
{
  _instance = this; // Set the static instance to this object
}

// Initialize the house
void House::setup()
{
  // Define the 3D coordinates of the house's vertices
  float house_points[9][3] = {
      {-1.0, -1.0, 1.0}, // Base corners
      {1.0, -1.0, 1.0},
      {1.0, 1.0, 1.0},
      {-1.0, 1.0, 1.0},
      {-1.0, -1.0, -1.0},
      {1.0, -1.0, -1.0},
      {1.0, 1.0, -1.0},
      {-1.0, 1.0, -1.0},
      {0.0, 3.0, 0.0} // Apex of the roof
  };

  // Define the edges of the house, connecting pairs of vertices
  uint32_t house_edges[16][2] = {
      {0, 1}, {1, 2}, {2, 3}, {3, 0}, // Base edges
      {4, 5},
      {5, 6},
      {6, 7},
      {7, 4}, // Top edges
      {0, 4},
      {1, 5},
      {2, 6},
      {3, 7}, // Vertical edges
      {2, 8},
      {3, 8},
      {6, 8},
      {7, 8} // Roof edges
  };

  // Initialize the Goblin3D object (house) with 9 points and 16 edges
  if (!goblin3d_init(&_house, 9, 16))
  {
    Serial.println("Failed to initialize house.");
    while (true)
      ; // Halt execution if initialization fails
  }

  // Set the scaling factor for the 3D object
  _house.scale_size = 120.0;
  // Set the initial rotation angle around the X-axis
  _house.x_angle_deg = 20.0;
  // Set the initial X and Y offset to center the object on the display
  _house.x_offset = _tft.width() / 2;
  _house.y_offset = _tft.height() / 2;

  // Copy the predefined house points to the Goblin3D object's original points array
  for (uint32_t i = 0; i < 9; i++)
    for (uint32_t j = 0; j < 3; j++)
      _house.orig_points[i][j] = house_points[i][j];

  // Copy the predefined house edges to the Goblin3D object's edges array
  for (uint32_t i = 0; i < 16; i++)
    for (uint32_t j = 0; j < 2; j++)
      _house.edges[i][j] = house_edges[i][j];
}

// Update and render the house
void House::update()
{
  // Render the house with black lines to erase the previous frame
  goblin3d_render(&_house, eraseLine);

  // Update the rotation angles for a rotating effect
  _house.x_angle_deg = fmod(_house.x_angle_deg + 1.0, 360.0); // Increment X rotation
  _house.y_angle_deg = fmod(_house.y_angle_deg + 1.0, 360.0); // Increment Y rotation
  _house.z_angle_deg = fmod(_house.z_angle_deg + 1.0, 360.0); // Increment Z rotation

  // Perform rendition pre-calculations
  goblin3d_precalculate(&_house);

  // Render the house with blue lines to draw the new frame
  goblin3d_render(&_house, drawLine);
}

// Static function to draw a line on the TFT display
void House::drawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
  if (_instance)
  {
    _instance->_tft.drawLine(x1, y1, x2, y2, TFT_SKYBLUE);
  }
}

// Static function to erase a line on the TFT display
void House::eraseLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
  if (_instance)
  {
    _instance->_tft.drawLine(x1, y1, x2, y2, TFT_BLACK);
  }
}