#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#define bleServerName "ESP32_NTR_server"
#define SERVICE_UUID_SERVER "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define timeCHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define sensorCHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a9"
#define imuCHARACTERISTIC_UUID "d7395388-f82f-4b88-8f31-db216ece04f3"

BLECharacteristic timeCharacteristic(timeCHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_READ);
BLECharacteristic sensorCharacteristic(sensorCHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_READ);
BLECharacteristic imuCharacteristic(imuCHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_READ);

//BLEDescriptor timeDescriptor(BLEUUID((uint16_t)0x2902));
//BLEDescriptor sensorDescriptor(BLEUUID((uint16_t)0x2902));

void setupBLE_Server() {
    BLEDevice::init(bleServerName);
    BLEServer* pServer = BLEDevice::createServer();
    pServer->getPeerMTU(517);
    BLEService* pService = pServer->createService(SERVICE_UUID_SERVER);
    pService->addCharacteristic(&timeCharacteristic);
    pService->addCharacteristic(&sensorCharacteristic);
    //pService->addCharacteristic(&imuCharacteristic);
    //timeCharacteristic.addDescriptor(&timeDescriptor);
    //sensorCharacteristic.addDescriptor(&sensorDescriptor);
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

void transmitDataPacket(uint32_t t, float* sens, float *imu) {
    transmitTimeData(t);
    transmitSensorData(sens);
    //transmitImuData(imu);
}

//Parte client iniziale

#define bleServerName_client "ESP32_NTR_client"
static BLEUUID SERVICE_UUID_CLIENT("4fafc201-1fb5-459e-8fcc-c5c9c331915d");
static BLEUUID snCHARACTERISTIC_UUID("beb5483e-36e1-4688-b7f5-ea07361b26a7");
static BLEUUID paramCHARACTERISTIC_UUID("beb5483e-36e1-4688-b7f5-ea07361b26a1");

static boolean doConnect = false;
static boolean connected = false;
static boolean doScan = false;
static BLERemoteCharacteristic* snCharacteristic;
static BLERemoteCharacteristic* paramCharacteristic;
static BLEAdvertisedDevice* myDevice;

/*static void notifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify) {
    Serial.print("Notify callback for characteristic ");
    Serial.print(pBLERemoteCharacteristic->getUUID().toString().c_str());
    Serial.print(" of data length ");
    Serial.println(length);
    Serial.print("data: ");
    Serial.println((char*)pData);
}*/

static void notifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify) {
    Serial.print("Notify callback for characteristic ");
    float b;
    b = pBLERemoteCharacteristic->readFloat();
    Serial.println(b);
}


class MyClientCallback : public BLEClientCallbacks {
    void onConnect(BLEClient* pclient) {
    }
    void onDisconnect(BLEClient* pclient) {
        connected = false;
        Serial.println("onDisconnect");
    }
};

uint32_t connectToServer(uint32_t val_w) {
    Serial.print("Forming a connection to ");
    Serial.println(myDevice->getAddress().toString().c_str());
    BLEClient* pClient = BLEDevice::createClient();
    Serial.println(" - Created client");
    pClient->setClientCallbacks(new MyClientCallback());
    // Connect to the remove BLE Server.
    pClient->connect(myDevice);
    Serial.println(" - Connected to server");
    // Obtain a reference to the service we are after in the remote BLE server.
    BLERemoteService* pRemoteService = pClient->getService(SERVICE_UUID_CLIENT);
    if (pRemoteService == nullptr) {
        Serial.print("Failed to find our service UUID: ");
        Serial.println(SERVICE_UUID_CLIENT.toString().c_str());
        pClient->disconnect();
        //return false;
    }
    Serial.println(" - Found our service");
    // Obtain a reference to the characteristic in the service of the remote BLE server.
    snCharacteristic = pRemoteService->getCharacteristic(snCHARACTERISTIC_UUID);
    if (snCharacteristic == nullptr) {
        Serial.print("Failed to find our characteristic UUID: ");
        Serial.println(snCHARACTERISTIC_UUID.toString().c_str());
        pClient->disconnect();
        //return false;
    }
    Serial.println(" - Found our characteristic");

    uint32_t value;

    // Read the value of the characteristic.
    if (snCharacteristic->canRead()) {
        value = snCharacteristic->readUInt32();
        Serial.print("The characteristic value was: ");
        Serial.println(value);
    }

    uint32_t to_w;

    if (snCharacteristic->canWrite()) {
        to_w = val_w;
        snCharacteristic->writeValue((String)to_w);
        Serial.print("Scritto sulla caratteristica valore ");
        Serial.println(to_w);
    }

    connected = true;

    paramCharacteristic = pRemoteService->getCharacteristic(paramCHARACTERISTIC_UUID);
    if (paramCharacteristic == nullptr) {
        Serial.print("Failed to find our param characteristic UUID: ");
        Serial.println(paramCHARACTERISTIC_UUID.toString().c_str());
    }
    Serial.println(" - Found our param characteristic");
    delay(1000);
    uint32_t param;

    // Read the param value of the characteristic.
    if (paramCharacteristic->canRead()) {
        param = paramCharacteristic->readUInt32();
        Serial.print("The characteristic value was: ");
        Serial.println(param);
    }
    return param;
   
    //if (paramCharacteristic->canNotify()) {
      //  paramCharacteristic->registerForNotify(notifyCallback);
    //}

}

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


uint32_t readBLE_initial(uint32_t v) {
    if (doConnect == true) {
        if (uint32_t p = connectToServer(v)) {
            Serial.println("We are now connected to the BLE Server.");
            return(p);
        }
        else {
            Serial.println("We have failed to connect to the server; Restart your device to scan for nearby BLE server again.");
        }
        doConnect = false;
    }
}




