#ifndef BLEMANAGER_H
#define BLEMANAGER_H

#include <ArduinoBLE.h> // Use ArduinoBLE for nRF52
#include <functional>
#include <string>

#define STRING_CHARACTERISTIC_SIZE 128;

class BLEManager
{
public:
  bool deviceConnected;
  bool oldDeviceConnected;
  using ModelNameCallback = std::function<void(const String &)>;
  ModelNameCallback modelNameCallback;

  BLEManager();
  void begin(const String &deviceName, ModelNameCallback callback);
  void update();

private:
  BLEService _pService;                     // BLE Service
  BLEStringCharacteristic _pCharacteristic; // BLE Characteristic

  static constexpr const char *SERVICE_UUID = "4fafc201-1fb5-459e-8fcc-c5c9c331914b";
  static constexpr const char *CHARACTERISTIC_UUID = "beb5483e-36e1-4688-b7f5-ea07361b26a8";

  // Callback for BLE events
  static void onBLEConnected(BLEDevice central);
  static void onBLEDisconnected(BLEDevice central);
  static void onBLECharacteristicWritten(BLEDevice central, BLEStringCharacteristic characteristic);

  static BLEManager *_instance;
};

#endif