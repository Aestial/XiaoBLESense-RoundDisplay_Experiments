#ifndef OBJMODEL_H
#define OBJMODEL_H

#include <goblin3d.h> // Include the Goblin3D library for 3D rendering
#include <TFT_eSPI.h> // Include the TFT display library

class ObjModel
{
public:
  ObjModel(TFT_eSprite &img);                              // Constructor (accepts a TFT_eSprite reference)
  bool load(const char *filename);                         // Load an OBJ file
  void rotateEffect();                                     // Automatic rotate effect (demo)
  void rotateYaw(float delta);          // Rotate Yaw coordinates
  void setup(float scale, float x_offset, float y_offset); // Initialize the model
  void update();                                           // Update and render the model

private:
  TFT_eSprite &_img;   // Reference to the TFT_eSprite for rendering
  goblin3d_obj_t _obj; // Goblin3D object for the 3D model

  // Helper functions for rendering
  static void drawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
  static void eraseLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);

  // Static instance for callback context
  static ObjModel *_instance;
};

#endif