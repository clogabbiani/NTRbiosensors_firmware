#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#define bleServerName "ESP32_NTR"
#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define timeCHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define sensorCHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a9"
#define imuCHARACTERISTIC_UUID "d7395388-f82f-4b88-8f31-db216ece04f3"

BLECharacteristic timeCharacteristic(timeCHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_READ);
BLECharacteristic sensorCharacteristic(sensorCHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_READ);
BLECharacteristic imuCharacteristic(imuCHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_READ);

void setupBLE() {
    BLEDevice::init(bleServerName);
    BLEServer* pServer = BLEDevice::createServer();
    pServer->getPeerMTU(517);
    BLEService* pService = pServer->createService(SERVICE_UUID);
    pService->addCharacteristic(&timeCharacteristic);
    pService->addCharacteristic(&sensorCharacteristic);
    //pService->addCharacteristic(&imuCharacteristic);
    pService->start();
    BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    BLEDevice::startAdvertising();
}

void transmitTimeData(uint32_t t) {
    timeCharacteristic.setValue(t);
    timeCharacteristic.notify();
}

void transmitSensorData(float* sens) {
    uint8_t array_256[256] = {};
    uint8_t array_4[4] = {};

    float val_s;

    for (int i = 0; i < 64; i++) {

        val_s = sens[i];

        array_4[0] = ((uint8_t*)&val_s)[0];
        array_4[1] = ((uint8_t*)&val_s)[1];
        array_4[2] = ((uint8_t*)&val_s)[2];
        array_4[3] = ((uint8_t*)&val_s)[3];

        array_256[4 * i] = array_4[0];
        array_256[4 * i + 1] = array_4[1];
        array_256[4 * i + 2] = array_4[2];
        array_256[4 * i + 3] = array_4[3];
    }

    sensorCharacteristic.setValue(array_256, 256);
    sensorCharacteristic.notify();
}

/*void transmitImuData(float* Imusens) {
    uint8_t array_24[24] = {};
    uint8_t array_4[4] = {};

    float val_s;

    for (int i = 0; i < 6; i++) {

        val_s = Imusens[i];

        array_4[0] = ((uint8_t*)&val_s)[0];
        array_4[1] = ((uint8_t*)&val_s)[1];
        array_4[2] = ((uint8_t*)&val_s)[2];
        array_4[3] = ((uint8_t*)&val_s)[3];

        array_24[4 * i] = array_4[0];
        array_24[4 * i + 1] = array_4[1];
        array_24[4 * i + 2] = array_4[2];
        array_24[4 * i + 3] = array_4[3];
    }

    imuCharacteristic.setValue(array_24, 24);
    imuCharacteristic.notify();
}*/

void transmitDataPacket(uint32_t t, float* sens, float *imu) {
    transmitTimeData(t);
    transmitSensorData(sens);
    //transmitImuData(imu);
}



