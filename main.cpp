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
uint32_t SN = 2000;

// --- NVS calibrazione ---
bool saveCalibToNVS(const float* A, const float* B, const float* C, const float* D) {
    Preferences pref;
    if (!pref.begin("calib", false)) return false;
    bool ok = true;
    ok &= pref.putBytes("A", A, 59 * sizeof(float)) == 59 * sizeof(float);
    ok &= pref.putBytes("B", B, 59 * sizeof(float)) == 59 * sizeof(float);
    ok &= pref.putBytes("C", C, 59 * sizeof(float)) == 59 * sizeof(float);
    ok &= pref.putBytes("D", D, 59 * sizeof(float)) == 59 * sizeof(float);
    pref.end();
    return ok;
}

bool loadCalibFromNVS(float* A, float* B, float* C, float* D) {
    Preferences pref;
    if (!pref.begin("calib", true)) return false;
    bool ok = true;
    ok &= pref.getBytes("A", A, 59 * sizeof(float)) == 59 * sizeof(float);
    ok &= pref.getBytes("B", B, 59 * sizeof(float)) == 59 * sizeof(float);
    ok &= pref.getBytes("C", C, 59 * sizeof(float)) == 59 * sizeof(float);
    ok &= pref.getBytes("D", D, 59 * sizeof(float)) == 59 * sizeof(float);
    pref.end();
    return ok;
}


// Dichiarazione dei PIN
const int enablePins[4] = { 10, 48, 14, 13 };   // PIN di Enable per i MUX (attivo basso) 
const int addressPins[4] = { 45, 47, 9, 21 }; // PIN di Indirizzo per il canale del MUX
const int ADCPINs[4] = { 1, 2, 5, 4 };  // PIN di ADC
const int sdaPin = 11;
const int sclPin = 12;
const int CLEARBAT = 7;
const int CSpin = 36;    //Chip Select pin per IMU
const int SA0pin = 35;   //SA0 pin per selezione indirizzo per IMU


//Tasks principali
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
    /*
    while (sn_test != true) {
        sn_test = readBLE_initial(SN);
    }
    Serial.println("Serial Number inviato");
    */

    Serial.print("[BOOT] param: ");

    // AVVIA SEMPRE IL SERVER → l’app ora ti vede come periferica BLE
    setupBLE_Server();

    // === Recupero parametri di calibrazione ===
    bool calibOK = false;

    // prova a fetchare dal server di calibrazione
    calibOK = fetchCalibrationFromServer();
    Serial.println(calibOK ? "[CAL] Parametri letti da BLE" : "[CAL] Lettura da BLE fallita");

    // se BLE fallisce, prova da NVS
    if (!calibOK) {
        calibOK = loadCalibFromNVS(paramA, paramB, paramC, paramD);
        Serial.println(calibOK ? "[CAL] Parametri caricati da NVS" : "[CAL] Nessun parametro valido in NVS");
    }

    // se arrivano da BLE, salvali in NVS per gli avvii successivi
    if (calibOK) {
        if (saveCalibToNVS(paramA, paramB, paramC, paramD)) {
            Serial.println("[CAL] Parametri salvati in NVS");
        }
        else {
            Serial.println("[CAL] Errore salvataggio NVS");
        }
        // stampa veloce di controllo
        Serial.printf("[CAL] A[0]=%.6f B[0]=%.6f C[0]=%.6f D[0]=%.6f\n", paramA[0], paramB[0], paramC[0], paramD[0]);

        // debug per vedere se i parametri di calibrazione sono ragionevoli
        auto dbg = [](const char* name, float* v) {
            int nz = 0; float mn = 1e9f, mx = -1e9f;
            for (int i = 0; i < 59; i++) {
                float x = v[i];
                if (x != 0.0f)
                    nz++;
                if (x < mn)
                    mn = x;
                if (x > mx) mx = x;
            }
            Serial.printf("[CAL] %s: nz=%d  min=%.6f  max=%.6f  first=%.6f\n", name, nz, mn, mx, v[0]);
        };
        dbg("A", paramA); dbg("B", paramB); dbg("C", paramC); dbg("D", paramD);

    }


    // === Fine recupero parametri di calibrazione ===

    //Crea task per multicore
    xTaskCreatePinnedToCore(Task1code, "Task1", 40000, NULL, 1, &Task1, 0);
    xTaskCreatePinnedToCore(Task2code, "Task2", 40000, NULL, 1, &Task2, 1);

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



