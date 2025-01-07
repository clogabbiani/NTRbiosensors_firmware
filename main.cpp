#include <Arduino.h>
#include "Acquisition.h"  // Modulo per la gestione dei MUX e acquisizione ADC
#include <Wire.h>   //Libreria per I2C
#include <EEPROM.h> //Libreria per scrivere/leggere su memoria FLASH

/*
#include "DataCollection.h" // Modulo per la raccolta e il filtraggio dei dati
#include "Alarm.h" // Modulo per il rilevamento dell'allarme
#include "Compression.h" // Modulo per la compressione dei dati tramite LZW
#include "BLETransmission.h" // Modulo per la trasmissione dati tramite BLE
*/

#define EEPROM_SIZE 1

// Variabili globali
const uint16_t sogliaAllarme = 30000; // Soglia per l'allarme (in grammi, quindi 30 kg)
uint32_t tempoUltimaTrasmissione = 0;
const uint32_t intervalloTrasmissione = 3600000; // 60 minuti in millisecondi
const float frequenzaTrasmissione = 0.88; // Frequenza di trasmissione in kb/s
float campionamentoFrequenza = 10.0; // Frequenza di campionamento
const int adcvalue = 12.0;   // risoluzione dell'ADC
int id; //id dispositivo
uint16_t V_bat; //Tensione batteria

// Dichiarazione dei PIN
const int enablePins[4] = { 18, 25, 22, 21 };   // PIN di Enable per i MUX (attivo basso)
const int addressPins[4] = { 26, 24, 17, 23 }; // PIN di Indirizzo per il canale del MUX
const int ADCPINs[4] = { 38, 39, 5, 4 };  // PIN di ADC
const int sdaPin = 19;
const int sclPin = 20;

void setup() {
    Serial.begin(115200);

    // Configura i PIN di Enable come OUTPUT e inizializza a HIGH (disabilitati)
    for (int i = 0; i < 4; i++) {
        pinMode(enablePins[i], OUTPUT);
        digitalWrite(enablePins[i], HIGH);
    }

    // Configura i PIN di Indirizzo come OUTPUT
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

    //Configura EEPROM e acquisisce/registra id device
    EEPROM.begin(EEPROM_SIZE);
    id = EEPROM.read(0);    //legge area di memoria "0" che contiene id device
    if (id == 0) {
        //Codice per acquisire un progressivo id tramite bluetooth
        EEPROM.write(0, 0); //cambiare il secondo parametro con il nuovo id dispositivo ottenuto
        EEPROM.commit();
    }
}

// Loop principale del firmware
void loop() {
    // 1. Acquisizione dati
    uint16_t valoriSensori[59];
    acquireData(valoriSensori, enablePins, addressPins, ADCPINs); // Richiama la funzione di acquisizione dati
    V_bat = I2C_battery_level();  //Acquisizione livello batteria

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

    // 5. Trasmissione dati periodica
    if (millis() - tempoUltimaTrasmissione >= intervalloTrasmissione) {
        transmitData(pacchettoCompresso);  // Trasmette i dati compressi via BLE
        tempoUltimaTrasmissione = millis(); // Aggiorna l'ultimo timestamp di trasmissione
    }

    */

    delay(100); // Attendere un po' prima del prossimo ciclo
}

