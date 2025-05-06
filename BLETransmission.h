#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#define bleServerName "ESP32_NTR"
#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define timeCHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define sensorCHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a9"

BLECharacteristic timeCharacteristic(timeCHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_READ);
BLECharacteristic sensorCharacteristic(sensorCHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_READ);

void setupBLE() {
    BLEDevice::init(bleServerName);
    BLEServer* pServer = BLEDevice::createServer();
    pServer->getPeerMTU(517);
    BLEService* pService = pServer->createService(SERVICE_UUID);
    pService->addCharacteristic(&timeCharacteristic);
    pService->addCharacteristic(&sensorCharacteristic);
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

void transmitDataPacket(uint32_t t, float* sens) {
    transmitTimeData(t);
    transmitSensorData(sens);
}











/*

// packets organization
struct DataPacket {
    uint32_t timestamp;
    uint16_t sensorValues[NUM_SENSORS];
    uint8_t sensorId;
};

void createDataPacket(DataPacket &packet) {
    packet.timestamp = millis() / 1000; // Timestamp corrente
    for (int i = 0; i < NUM_SENSORS; i++) {
        packet.sensorValues[i] = filterData(sensorData[i]);
    }
    packet.sensorId = 1; // Esempio ID sensore
}


// BLE transmission
BLECharacteristic *pDataCharacteristic;
BLECharacteristic *pAlarmCharacteristic;

void setupBLE() {
    BLEDevice::init("Sensorized Insole");
    BLEServer *pServer = BLEDevice::createServer();
    BLEService *pService = pServer->createService("ServiceUUID");

    pDataCharacteristic = pService->createCharacteristic("DataUUID", BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
    pAlarmCharacteristic = pService->createCharacteristic("AlarmUUID", BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);

    pService->start();
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->start();
}

void transmitData() {
    DataPacket packet;
    createDataPacket(packet);
    pDataCharacteristic->setValue((uint8_t *)&packet, sizeof(packet));
    pDataCharacteristic->notify();
}

void transmitAlarm() {
    if (alarmTriggered) {
        uint8_t alarm = 1;
        pAlarmCharacteristic->setValue(&alarm, 1);
        pAlarmCharacteristic->notify();
        alarmTriggered = false; // Reset allarme
    }
}

void sendDataOverBLE(uint16_t* sensorData, int sensorCount) {
    if (sensorData == NULL || sensorCount <= 0) {
        Serial.println("Errore: Dati da trasmettere mancanti");
        return;
    }

    // Trasmissione BLE, assicurandosi di non inviare dati con errore
    for (int i = 0; i < sensorCount; i++) {
        if (sensorData[i] == 0xFFFF) { // 0xFFFF indica un errore nel dato acquisito
            Serial.print("Errore nel dato del sensore ");
            Serial.println(i);
            continue; // Salta il dato corrotto
        }

        // Converti e invia i dati tramite BLE
        uint16_t valueToSend = sensorData[i];
        // logica di invio dati...
    }
}


*/



