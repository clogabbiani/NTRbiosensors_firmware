#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

// ---- helpers compatibili con le due versioni della BLE lib ----
inline std::string toStdString(const std::string& s) {
    return s;                                   // già std::string
}
inline std::string toStdString(const String& s) {
    return std::string(s.c_str(), s.length());  // Arduino String -> std::string
}

// ---- SERVER  ----

#define bleServerName "ESP32_NTR_server"
#define SERVICE_UUID_SERVER "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define timeCHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define sensorCHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a9"
#define imuCHARACTERISTIC_UUID "d7395388-f82f-4b88-8f31-db216ece04f3"

#define SERVICE_UUID_SERVER_CALIB "4fafc202-1fb5-459e-8fcc-c5c9c331914b"
#define CALIB_A_UUID "6b8b0003-1b2b-3b4b-5b6b-7b8b9babcb01" 
#define CALIB_B_UUID "6b8b0004-1b2b-3b4b-5b6b-7b8b9babcb01"
#define CALIB_C_UUID "6b8b0005-1b2b-3b4b-5b6b-7b8b9babcb01"
#define CALIB_D_UUID "6b8b0006-1b2b-3b4b-5b6b-7b8b9babcb01"

#define status_UUID "6b8b0001-1b2b-3b4b-5b6b-7b8b9babcb00"

BLECharacteristic timeCharacteristic(timeCHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_READ);
BLECharacteristic sensorCharacteristic(sensorCHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_READ);
BLECharacteristic imuCharacteristic(imuCHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_READ);

BLECharacteristic AcalibCharacteristic(CALIB_A_UUID, BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
BLECharacteristic BcalibCharacteristic(CALIB_B_UUID, BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
BLECharacteristic CcalibCharacteristic(CALIB_C_UUID, BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
BLECharacteristic DcalibCharacteristic(CALIB_D_UUID, BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
BLECharacteristic statusCharacteristic(status_UUID, BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);

void setupBLE_Server_Calib() {
    BLEDevice::init(bleServerName);
    BLEServer* pServer = BLEDevice::createServer();
    pServer->getPeerMTU(517);
    BLEService* pService = pServer->createService(SERVICE_UUID_SERVER_CALIB);
    //pService->addCharacteristic(&timeCharacteristic);
    //pService->addCharacteristic(&sensorCharacteristic);
    //pService->addCharacteristic(&imuCharacteristic);
    pService->addCharacteristic(&AcalibCharacteristic);
    pService->addCharacteristic(&BcalibCharacteristic);
    pService->addCharacteristic(&CcalibCharacteristic);
    pService->addCharacteristic(&DcalibCharacteristic);
    pService->addCharacteristic(&statusCharacteristic);
    pService->start();
    BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID_SERVER_CALIB);
    pAdvertising->setScanResponse(true);
    BLEDevice::startAdvertising();
}

void setupBLE_Server() {
    BLEDevice::init(bleServerName);
    BLEServer* pServer = BLEDevice::createServer();
    pServer->getPeerMTU(517);
    BLEService* pService = pServer->createService(SERVICE_UUID_SERVER);
    pService->addCharacteristic(&timeCharacteristic);
    pService->addCharacteristic(&sensorCharacteristic);
    //pService->addCharacteristic(&imuCharacteristic);
    //pService->addCharacteristic(&AcalibCharacteristic);
    //pService->addCharacteristic(&BcalibCharacteristic);
    //pService->addCharacteristic(&CcalibCharacteristic);
    //pService->addCharacteristic(&DcalibCharacteristic);
    //pService->addCharacteristic(&statusCharacteristic);
    pService->start();
    BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID_SERVER);
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

void populate_calib() {
    uint8_t array_256_zero[256] = { 0 };
    AcalibCharacteristic.setValue(array_256_zero, 256);
    BcalibCharacteristic.setValue(array_256_zero, 256);
    CcalibCharacteristic.setValue(array_256_zero, 256);
    DcalibCharacteristic.setValue(array_256_zero, 256);
    //AcalibCharacteristic.notify();
    //BcalibCharacteristic.notify();
    //CcalibCharacteristic.notify();
    //DcalibCharacteristic.notify();

}

void transmitDataPacket(uint32_t t, float* sens, float *imu) {
    transmitTimeData(t);
    transmitSensorData(sens);
    //transmitImuData(imu);
}

bool readFloats64(BLECharacteristic ble_charac, float* array_64) {
    uint8_t* array_256;
    array_256 = ble_charac.getData();
    
    /*for (int i = 0; i < 64; i++) {
        float val_s;
        ((uint8_t*)&val_s)[0] = array_256[4 * i];
        ((uint8_t*)&val_s)[1] = array_256[4 * i + 1];
        ((uint8_t*)&val_s)[2] = array_256[4 * i + 2];
        ((uint8_t*)&val_s)[3] = array_256[4 * i + 3];
        array_64[i] = val_s;
    }*/

    for (int i = 0; i < 64; i++) {
        memcpy(&array_64[i], &array_256[4 * i], sizeof(float));
    }
    return true;
}

bool acquireCalibParam() {
    populate_calib();
    String status = "0";
    while (status != "1") {
        status = statusCharacteristic.getValue();
    }

    float A64[64], B64[64], C64[64], D64[64];
    /*if (!readFloats64(CALIB_A_UUID, A64)) { return false; }
    if (!readFloats64(CALIB_B_UUID, B64)) {return false; }
    if (!readFloats64(CALIB_C_UUID, C64)) {return false; }
    if (!readFloats64(CALIB_D_UUID, D64)) {return false; }*/
    delay(1000);
    readFloats64(AcalibCharacteristic, A64);
    readFloats64(BcalibCharacteristic, B64);
    readFloats64(CcalibCharacteristic, C64);
    readFloats64(DcalibCharacteristic, D64);

    //Copia i primi 59 nelle globali (dichiarate in main.cpp)
    extern float paramA[59], paramB[59], paramC[59], paramD[59];
    for (int i = 0; i < 59; i++) {
        paramA[i] = A64[i]; paramB[i] = B64[i]; paramC[i] = C64[i]; paramD[i] = D64[i];
        Serial.println();
        Serial.print(paramA[i]);
        Serial.print(paramB[i]);
        Serial.print(paramC[i]);
        Serial.print(paramD[i]);
    }
    return true;
}
/*

// ---- CLIENT ----
#define bleServerName_client "ESP32_NTR_client"
static BLEUUID SERVICE_UUID_CLIENT("4fafc201-1fb5-459e-8fcc-c5c9c331915d");
static BLEUUID snCHARACTERISTIC_UUID("beb5483e-36e1-4688-b7f5-ea07361b26a7");

// ---- CALIBRATION CHARACTERISTICS ----
static BLEUUID CALIB_META_UUID("6b8b0002-1b2b-3b4b-5b6b-7b8b9babcb01");
static BLEUUID CALIB_A_UUID("6b8b0003-1b2b-3b4b-5b6b-7b8b9babcb01");
static BLEUUID CALIB_B_UUID("6b8b0004-1b2b-3b4b-5b6b-7b8b9babcb01");
static BLEUUID CALIB_C_UUID("6b8b0005-1b2b-3b4b-5b6b-7b8b9babcb01");
static BLEUUID CALIB_D_UUID("6b8b0006-1b2b-3b4b-5b6b-7b8b9babcb01");

static boolean doConnect = false;
static boolean connected = false;
static boolean doScan = false;
static BLERemoteCharacteristic* snCharacteristic;
static BLERemoteCharacteristic* paramCharacteristic;
static BLEAdvertisedDevice* myDevice;

class MyClientCallback : public BLEClientCallbacks {
    void onConnect(BLEClient* pclient) {
    }
    void onDisconnect(BLEClient* pclient) {
        connected = false;
        Serial.println("onDisconnect");
    }
};

// Scan for BLE servers and find the first one that advertises the service we are looking for.
class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
    // Called for each advertising BLE server.
    void onResult(BLEAdvertisedDevice advertisedDevice) {
        Serial.print("BLE Advertised Device found: ");
        Serial.println(advertisedDevice.toString().c_str());
        // We have found a device, let us now see if it contains the service we are looking for.
        if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(SERVICE_UUID_CLIENT)) {
            BLEDevice::getScan()->stop();
            myDevice = new BLEAdvertisedDevice(advertisedDevice);
            doConnect = true;
            doScan = true;
        } // Found our server
    } // onResult
}; // MyAdvertisedDeviceCallbacks

void setupBLE_Client() {
    BLEDevice::init("");
    Serial.println("BLE device initialized as client, scanning...");
    BLEScan* pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    pBLEScan->setActiveScan(true);
    pBLEScan->start(30);
}

bool sendSN_BLE(uint32_t serial_num) {
    BLEClient* pClient = BLEDevice::createClient();
    //Serial.println(" - Created client");
    pClient->setClientCallbacks(new MyClientCallback());
    // Connect to the remove BLE Server.
    pClient->connect(myDevice);
    //Serial.println(" - Connected to server");
    // Obtain a reference to the service we are after in the remote BLE server.
    BLERemoteService* pRemoteService = pClient->getService(SERVICE_UUID_CLIENT);
    if (pRemoteService == nullptr) {
        Serial.print("Failed to find our service UUID: ");
        Serial.println(SERVICE_UUID_CLIENT.toString().c_str());
        pClient->disconnect();
        return false;
    }
    //Serial.println(" - Found our service");
    // Obtain a reference to the characteristic in the service of the remote BLE server.
    snCharacteristic = pRemoteService->getCharacteristic(snCHARACTERISTIC_UUID);
    if (snCharacteristic == nullptr) {
        Serial.print("Failed to find our characteristic UUID: ");
        Serial.println(snCHARACTERISTIC_UUID.toString().c_str());
        pClient->disconnect();
        return false;
    }
    //Serial.println(" - Found our characteristic");

    uint32_t value;

    // Read the value of the characteristic.
    if (snCharacteristic->canRead()) {
        value = snCharacteristic->readUInt32();
        Serial.print("The sn characteristic value was: ");
        Serial.println(value);
    }
    else {
        return false;
    }

    uint32_t to_w;

    if (snCharacteristic->canWrite()) {
        to_w = serial_num;
        snCharacteristic->writeValue((String)to_w);
        Serial.print("Scritto sulla caratteristica sn valore ");
        Serial.println(to_w);
        return true;
    }
    else {
        return false;
    }
}

bool fetchCalibrationFromServer() {

    // 1. Controlla se il dispositivo è stato trovato dallo scan
    if (!myDevice) {
        Serial.println(" - Dispositivo non trovato dallo scan.");
        return false;
    }

    // 2. Crea un nuovo client
    BLEClient* pClient = BLEDevice::createClient();
    if (!pClient) return false;
    pClient->setClientCallbacks(new MyClientCallback());

    // 3. Tenta la connessione
    Serial.print("Forming a connection to ");
    Serial.println(myDevice->getAddress().toString().c_str());
    if (!pClient->connect(myDevice)) {
        Serial.println(" - Connect failed.");
        delete pClient; // Libera la memoria se la connessione fallisce
        return false;
    }
    Serial.println(" - Connected to server.");

    // 4. Ottieni il servizio e i caratteristici (UUID)
    BLERemoteService* svc = pClient->getService(SERVICE_UUID_CLIENT);
    if (!svc) {
        Serial.println(" - Failed to find calibration service.");
        pClient->disconnect();
        return false;
    }
    Serial.println(" - Found our service.");

    // 5. Funzione di supporto per leggere 64 float da una caratteristica
    auto readFloats64 = [&](BLEUUID uuid, float* out64) -> bool {
        BLERemoteCharacteristic* ch = svc->getCharacteristic(uuid);
        if (!ch) return false;

        // raw può essere String o std::string a seconda della lib
        auto raw = ch->readValue();
        std::string v = toStdString(raw);

        if (v.size() != 64 * sizeof(float)) return false;   // 256 byte attesi
        memcpy(out64, v.data(), 64 * sizeof(float));
        return true;
    };

    // 6. Leggi i 4 blocchi
    float A64[64], B64[64], C64[64], D64[64];
    if (!readFloats64(CALIB_A_UUID, A64)) { pClient->disconnect(); return false; }
    if (!readFloats64(CALIB_B_UUID, B64)) { pClient->disconnect(); return false; }
    if (!readFloats64(CALIB_C_UUID, C64)) { pClient->disconnect(); return false; }
    if (!readFloats64(CALIB_D_UUID, D64)) { pClient->disconnect(); return false; }

    // 7. Copia i primi 59 nelle globali (dichiarate in main.cpp)
    extern float paramA[59], paramB[59], paramC[59], paramD[59];
    for (int i = 0; i < 59; i++) {
        paramA[i] = A64[i]; paramB[i] = B64[i]; paramC[i] = C64[i]; paramD[i] = D64[i];
    }

    // 8. Disconnetti alla fine
    pClient->disconnect();
    Serial.println(" - Disconnected from server.");
    return true;
}

*/


