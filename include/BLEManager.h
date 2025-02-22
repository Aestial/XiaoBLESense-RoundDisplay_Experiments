#ifndef BLEMANAGER_H
#define BLEMANAGER_H

#include <bluefruit.h> // Use Adafruit Bluefruit Library
#include <functional>
#include <string>

class BLEManager {
public:
    using ModelNameCallback = std::function<void(const std::string&)>;

    BLEManager();
    void begin(const std::string& deviceName, ModelNameCallback callback);
    void update();

private:
    BLEService _bleService; // BLE Service
    BLECharacteristic _bleCharacteristic; // BLE Characteristic
    bool _deviceConnected;
    bool _oldDeviceConnected;
    ModelNameCallback _modelNameCallback;

    static constexpr const char* SERVICE_UUID = "4fafc201-1fb5-459e-8fcc-c5c9c331914b";
    static constexpr const char* CHARACTERISTIC_UUID = "beb5483e-36e1-4688-b7f5-ea07361b26a8";

    // Callback for BLE events
    static void onBLEConnected(uint16_t conn_handle);
    static void onBLEDisconnected(uint16_t conn_handle, uint8_t reason);
    static void onBLECharacteristicWritten(uint16_t conn_handle, BLECharacteristic* chr, uint8_t* data, uint16_t len);

    // Static instance for callback context
    static BLEManager* _instance;
};

#endif