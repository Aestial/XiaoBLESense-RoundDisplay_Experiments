#include "BLEManager.h"

BLEManager::BLEManager()
    : _pService(SERVICE_UUID), _pCharacteristic(CHARACTERISTIC_UUID, BLERead | BLEWrite, 128),
      deviceConnected(false), oldDeviceConnected(false)
{
  _instance = this;
}

void BLEManager::begin(const String &deviceName, ModelNameCallback callback)
{
  modelNameCallback = callback;

  // Initialize BLE
  if (!BLE.begin())
  {
    Serial.println("Failed to initialize BLE!");
    return;
  }

  // Set the device name
  BLE.setLocalName(deviceName.c_str());

  BLEDescriptor characteristicDescriptor = BLEDescriptor(CHARACTERISTIC_UUID, "3D Model name");
  _pCharacteristic.addDescriptor(characteristicDescriptor);

  // Add the characteristic to the service
  // _pService.addCharacteristic(_pCharacteristic);

  // Add the service to BLE
  BLE.addService(_pService);

  // Set event handlers
  BLE.setEventHandler(BLEConnected, onBLEConnected);
  BLE.setEventHandler(BLEDisconnected, onBLEDisconnected);
  // _pCharacteristic.setEventHandler(BLEWritten, onBLECharacteristicWritten);

  // Start advertising
  BLE.advertise();
  Serial.println("BLE started. Waiting for connections...");
}

void BLEManager::update()
{
  // Poll for BLE events
  BLE.poll();

  // Handle disconnection
  if (!deviceConnected && oldDeviceConnected)
  {
    delay(500);      // Give the Bluetooth stack a chance to get things ready
    BLE.advertise(); // Restart advertising
    Serial.println("Device disconnected. Restarting advertising...");
    oldDeviceConnected = deviceConnected;
  }

  // Handle connection
  if (deviceConnected && !oldDeviceConnected)
  {
    oldDeviceConnected = deviceConnected;
  }
}

void BLEManager::onBLEConnected(BLEDevice central)
{
  _instance->deviceConnected = true;
  Serial.println("Device connected.");
}

void BLEManager::onBLEDisconnected(BLEDevice central)
{
  _instance->deviceConnected = false;
  Serial.println("Device disconnected.");
}

void BLEManager::onBLECharacteristicWritten(BLEDevice central, BLEStringCharacteristic characteristic)
{
  String value = characteristic.value();
  if (value.length() > 0)
  {
    Serial.printf("Received new model name: %s\n", value.c_str());
    _instance->modelNameCallback(value); // Notify the callback
  }
}