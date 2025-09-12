#include <Arduino.h>
#include <EEPROM.h> //Libreria per scrivere/leggere su memoria FLASH
#include <Wire.h>   //Libreria per I2C
#include "Acquisition.h" // Modulo per l'acquisizione dei dati
#include "BatteryManagement.h"  //Modulo per la gestione batteria
#include "BLETransmission.h" // Modulo per la trasmissione dati tramite BLE
#include "IMU.h"    //Modulo per la gestione dati IMU
#include <Preferences.h>

/*
#include "DataCollection.h" // Modulo per la raccolta e il filtraggio dei dati
#include "Alarm.h" // Modulo per il rilevamento dell'allarme
#include "Compression.h" // Modulo per la compressione dei dati tramite LZW
*/

#define EEPROM_SIZE 10
#define led_r 16
#define led_b 15

// Variabili globali
const uint16_t sogliaAllarme = 30000; // Soglia per l'allarme (in grammi, quindi 30 kg)
uint32_t tempoUltimaTrasmissione = 0;
const uint32_t intervalloTrasmissione = 3600000; // 60 minuti in millisecondi
int id, id_r; //id dispositivo
float V_bat; //Tensione batteria
float valoriSensori[64];    //Valori sensori FSR
float IMUdata[6];   //Dati accelerometro e giroscopio
uint32_t currentTimestamp;
int BLE_dataReady=0;
TaskHandle_t Task1;
TaskHandle_t Task2;

float paramA[59], paramB[59], paramC[59], paramD[59];
uint32_t SN_test = 2000;
uint32_t param_test = 0;


// Dichiarazione dei PIN
const int enablePins[4] = { 10, 48, 14, 13 };   // PIN di Enable per i MUX (attivo basso) 
const int addressPins[4] = { 45, 47, 9, 21 }; // PIN di Indirizzo per il canale del MUX
const int ADCPINs[4] = { 1, 2, 5, 4 };  // PIN di ADC
const int sdaPin = 11;
const int sclPin = 12;
const int CLEARBAT = 7;
const int CSpin = 36;    //Chip Select pin per IMU
const int SA0pin = 35;   //SA0 pin per selezione indirizzo per IMU

Preferences prefs;

void Task1code(void* pvParam) {
    for (;;) {
        BLE_dataReady = 0;

        // 1. Acquisizione dati
        currentTimestamp = millis(); // Timestamp attuale
        acquireData(valoriSensori, enablePins, addressPins, ADCPINs); // Richiama la funzione di acquisizione dati
        V_bat = I2C_battery_level();  //Acquisizione livello batteria
        //acquireIMUdata(IMUdata);

        /*
        // 2. Raccolta e Filtraggio dati
        uint16_t valoriFiltrati[59];
        filterData(valoriSensori, valoriFiltrati);

        // 3. Rilevazione Allarme
        bool allarmeAttivo = checkThreshold(valoriFiltrati, sogliaAllarme);
        if (allarmeAttivo) {
            transmitAlarm();  // Invia un segnale di allarme immediato tramite BLE
        }

        // 4. Compressione Dati
        CompressedData pacchettoCompresso;
        compressDataLZW(valoriFiltrati, pacchettoCompresso);
        */

        // 5. Trasmissione dati periodica
        // Vedi TaskCore_BLE

        BLE_dataReady = 1;
        delay(10);
    }
}

void Task2code(void* pvParam) {
    for (;;) {
        if (BLE_dataReady == 1) {
            transmitDataPacket(currentTimestamp, valoriSensori, IMUdata);  // Trasmette i dati compressi via BLE
        }
    }
    delay(10);
}

void setup() {

	 Serial.begin(115200); // Inizializza la comunicazione seriale

    // Configura i PIN di Enable come OUTPUT e li spegne
    for (int i = 0; i < 4; i++) {
        pinMode(enablePins[i], OUTPUT);
        digitalWrite(enablePins[i], HIGH);
    }

    // Configura i PIN di Indirizzo come OUTPUT e li accende
    for (int i = 0; i < 4; i++) {
        pinMode(addressPins[i], OUTPUT);
        digitalWrite(addressPins[i], LOW);
    }

    // Configura i PIN di ADC come INPUT
    for (int i = 0; i < 4; i++) {
        pinMode(ADCPINs[i], INPUT);
    }

    //Configura i PIN per I2C 
    Wire.begin(sdaPin, sclPin);

    //Configura il pin CLEARBAT
    pinMode(CLEARBAT, OUTPUT);
    clearBattery(CLEARBAT);

    //Configura i pin per IMU e inizializza
    digitalWrite(SA0pin, LOW);  //Con pin SA0 = 0, l'indirizzo è 6Ah (106d)
    digitalWrite(CSpin, HIGH);
    setupIMU();

    //Inizializza BLE
    Serial.println("Starting BLE...");
    setupBLE_Client();
    while (param_test == 0) {
        param_test = readBLE_initial(SN_test);
    }
    Serial.print("Test terminato, valore acquisito: ");
    Serial.println(param_test);
    setupBLE_Server();

    //Crea task per multicore
    xTaskCreatePinnedToCore(Task1code, "Task1", 40000, NULL, 1, &Task1, 0);
    xTaskCreatePinnedToCore(Task2code, "Task2", 40000, NULL, 1, &Task2, 1);

    /*
    //Configura EEPROM e acquisisce/registra id device
    EEPROM.begin(EEPROM_SIZE);
    id_r = EEPROM.read(0);
    */
    //Configura partizione NVS
    
    prefs.begin("ID", false);    //legge area di memoria "0" che contiene id device
    id_r = prefs.getInt("ID");
    if (id_r == 55) {
        //Carica primo set parametri
        pinMode(led_r, OUTPUT);
        digitalWrite(led_r, HIGH);
        delay(1000);
        digitalWrite(led_r, LOW);
    }
    if (id_r == 56) {
        //Carica secondo set parametri
        pinMode(led_b, OUTPUT);
        digitalWrite(led_b, HIGH);
        delay(1000);
        digitalWrite(led_b, LOW);
    }
    //prefs.end();
    /*
    pinMode(led_r, OUTPUT);
    digitalWrite(led_r, HIGH);
    delay(1000);
    digitalWrite(led_r, LOW);
    */
}

// Loop principale del firmware
void loop() {

}



