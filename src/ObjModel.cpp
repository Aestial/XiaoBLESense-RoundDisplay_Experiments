#include "ObjModel.h"

// Initialize the static instance pointer
ObjModel *ObjModel::_instance = nullptr;

// Constructors
ObjModel::ObjModel(TFT_eSprite &img, const char *filename) : _img(img), _modelName(filename)
{
  _instance = this; // Set the static instance to this object
}

ObjModel::ObjModel(TFT_eSprite &img) : _img(img), _modelName("monkey.obj")
{
  _instance = this; 
}

// Load an OBJ file
bool ObjModel::load(const char *filename)
{
  if (!goblin3d_parse_obj_file(filename, &_obj))
  {
    Serial.println("Failed to load OBJ file.");
    return false;
  }
  return true;
}

// Set the model name dynamically
void ObjModel::setModelName(const std::string &modelName)
{
  _modelName = modelName;
  load(_modelName.c_str()); // Reload the new model
}

// Initialize the model
void ObjModel::setup(float scale, float x_offset, float y_offset)
{
  // Set the scaling factor for the 3D object
  _obj.scale_size = scale;
  // Set the initial X and Y offset to center the object on the display
  _obj.x_offset = x_offset;
  _obj.y_offset = y_offset;

  // Load the initial model
  load(_modelName.c_str());
}

// Update and render the model
void ObjModel::update()
{  
  // Render the model with black lines to erase the old frame
  // goblin3d_render(&_obj, eraseLine);
  // Perform rendition pre-calculations
  goblin3d_precalculate(&_obj);
  // Render the model with blue lines to draw the new frame
  goblin3d_render(&_obj, drawLine);
}

void ObjModel::rotateEffect()
{
  // Update the rotation angles for a rotating effect
  _obj.x_angle_deg = fmod(_obj.x_angle_deg + 1.0, 360.0); // Increment X rotation
  _obj.y_angle_deg = fmod(_obj.y_angle_deg + 1.0, 360.0); // Increment Y rotation
  _obj.z_angle_deg = fmod(_obj.z_angle_deg + 1.0, 360.0); // Increment Z rotation
}

void ObjModel::rotateYaw(float delta)
{
  _obj.y_angle_deg += delta; // Increment Y rotation
}

// Static function to draw a line on the sprite
void ObjModel::drawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
  if (_instance)
  {
    _instance->_img.drawLine(x1, y1, x2, y2, TFT_SKYBLUE);
  }
}

// Static function to erase a line on the sprite
void ObjModel::eraseLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
  if (_instance)
  {
    _instance->_img.drawLine(x1, y1, x2, y2, TFT_BLACK);
  }
}