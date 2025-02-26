#include "BLEManager.h"

#ifdef __BLUEFRUIT__
// Initialize the static instance pointer
BLEManager* BLEManager::_instance = nullptr;

BLEManager::BLEManager()
    : _bleService(), _bleCharacteristic(), _deviceConnected(false), _oldDeviceConnected(false) {}

void BLEManager::begin(const std::string& deviceName, ModelNameCallback callback) {
    _modelNameCallback = callback;
    _instance = this; // Set the static instance to this object

    // Initialize Bluefruit
    Bluefruit.begin();
    Bluefruit.setName(deviceName.c_str());

    // Set the connection event handlers
    Bluefruit.Periph.setConnectCallback(onBLEConnected);
    Bluefruit.Periph.setDisconnectCallback(onBLEDisconnected);

    // Create the BLE Service
    _bleService = BLEService(SERVICE_UUID);

    // Start the service
    _bleService.begin();

    // Create the BLE Characteristic
    _bleCharacteristic = BLECharacteristic(CHARACTERISTIC_UUID);
    _bleCharacteristic.setProperties(CHR_PROPS_WRITE);
    _bleCharacteristic.setPermission(SECMODE_OPEN, SECMODE_OPEN);
    _bleCharacteristic.setFixedLen(20); // Set a fixed length for the characteristic
    _bleCharacteristic.setWriteCallback(onBLECharacteristicWritten);

    // Add the characteristic to the service
    _bleCharacteristic.begin();

    // Start advertising
    Bluefruit.Advertising.addService(_bleService);
    Bluefruit.Advertising.start();
    Serial.println("BLE started. Waiting for connections...");
}

void BLEManager::update() {
    // Handle disconnection
    if (!_deviceConnected && _oldDeviceConnected) {
        delay(500); // Give the Bluetooth stack a chance to get things ready
        Bluefruit.Advertising.start(); // Restart advertising
        Serial.println("Device disconnected. Restarting advertising...");
        _oldDeviceConnected = _deviceConnected;
    }

    // Handle connection
    if (_deviceConnected && !_oldDeviceConnected) {
        _oldDeviceConnected = _deviceConnected;
    }
}

void BLEManager::onBLEConnected(uint16_t conn_handle) {
    if (_instance) {
        _instance->_deviceConnected = true;
        Serial.println("Device connected.");
    }
}

void BLEManager::onBLEDisconnected(uint16_t conn_handle, uint8_t reason) {
    if (_instance) {
        _instance->_deviceConnected = false;
        Serial.println("Device disconnected.");
    }
}

void BLEManager::onBLECharacteristicWritten(uint16_t conn_handle, BLECharacteristic* chr, uint8_t* data, uint16_t len) {
    if (_instance && len > 0) {
        std::string value(reinterpret_cast<char*>(data), len);
        Serial.printf("Received new model name: %s\n", value.c_str());
        _instance->_modelNameCallback(value); // Notify the callback
    }
}

#endif

#ifdef __BLE_ESP__

BLEManager::BLEManager()
    : _pServer(nullptr), _pService(nullptr), _pCharacteristic(nullptr),
    _deviceConnected(false), _oldDeviceConnected(false) {}

void BLEManager::begin(const std::string &deviceName, ModelNameCallback callback)
{
    _modelNameCallback = callback;

    // Initialize BLE
    BLEDevice::init(deviceName);
    _pServer = BLEDevice::createServer();
    _pServer->setCallbacks(new MyServerCallbacks(*this));

    // Create the BLE Service
    _pService = _pServer->createService(SERVICE_UUID);

    // Create a BLE Characteristic for receiving the model name
    _pCharacteristic = _pService->createCharacteristic(
        CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_WRITE);
    _pCharacteristic->setCallbacks(new MyCharacteristicCallbacks(*this));

    // Start the service
    _pService->start();

    // Start advertising
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06); // Functions that help with iPhone connections
    pAdvertising->setMinPreferred(0x12);
    BLEDevice::startAdvertising();
    Serial.println("BLE started. Waiting for connections...");
}

void BLEManager::update()
{
    // Handle disconnection
    if (!_deviceConnected && _oldDeviceConnected)
    {
        delay(500);                   // Give the Bluetooth stack a chance to get things ready
        _pServer->startAdvertising(); // Restart advertising
        Serial.println("Device disconnected. Restarting advertising...");
        _oldDeviceConnected = _deviceConnected;
    }

    // Handle connection
    if (_deviceConnected && !_oldDeviceConnected)
    {
        _oldDeviceConnected = _deviceConnected;
    }
}

void BLEManager::MyServerCallbacks::onConnect(BLEServer *pServer)
{
    _manager._deviceConnected = true;
    Serial.println("Device connected.");
}

void BLEManager::MyServerCallbacks::onDisconnect(BLEServer *pServer)
{
    _manager._deviceConnected = false;
    Serial.println("Device disconnected.");
}

void BLEManager::MyCharacteristicCallbacks::onWrite(BLECharacteristic *pCharacteristic)
{
    std::string value = pCharacteristic->getValue();
    if (!value.empty())
    {
        Serial.printf("Received new model name: %s\n", value.c_str());
        _manager._modelNameCallback(value); // Notify the callback
    }
}

#endif