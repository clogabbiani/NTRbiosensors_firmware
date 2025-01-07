#include <main.cpp>

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



