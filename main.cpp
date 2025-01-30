#include <Arduino.h>

#include <EEPROM.h> //Libreria per scrivere/leggere su memoria FLASH
#include <Wire.h>   //Libreria per I2C

#include "Acquisition.h" // Modulo per l'acquisizione dei dati

// per compressione LZW
#include <map>
#include <vector>
#include <string>

/*
#include "DataCollection.h" // Modulo per la raccolta e il filtraggio dei dati
#include "Alarm.h" // Modulo per il rilevamento dell'allarme
#include "Compression.h" // Modulo per la compressione dei dati tramite LZW
#include "BLETransmission.h" // Modulo per la trasmissione dati tramite BLE
*/

#define EEPROM_SIZE 1
#define led 16 
#define DICT_MAX_SIZE 4096  // Limite per il dizionario (12-bit)    

// Variabili globali
const uint16_t sogliaAllarme = 30000; // Soglia per l'allarme (in grammi, quindi 30 kg)
uint32_t tempoUltimaTrasmissione = 0;
const uint32_t intervalloTrasmissione = 3600000; // 60 minuti in millisecondi
const float frequenzaTrasmissione = 0.88; // Frequenza di trasmissione in kb/s
float campionamentoFrequenza = 10.0; // Frequenza di campionamento
const int adcvalue = 12.0;   // risoluzione dell'ADC
int id, id_r; //id dispositivo
float V_bat; //Tensione batteria
float valoriSensori[59];
uint32_t currentTimestamp;

// Dichiarazione dei PIN
const int enablePins[4] = { 10, 48, 14, 13 };   // PIN di Enable per i MUX (attivo basso) 
const int addressPins[4] = { 45, 47, 9, 21 }; // PIN di Indirizzo per il canale del MUX
const int ADCPINs[4] = { 1, 2, 5, 4 };  // PIN di ADC
const int sdaPin = 11;
const int sclPin = 12;

void setup() {

    pinMode(led, OUTPUT);
    digitalWrite(led, HIGH);
    delay(1000);
    digitalWrite(led, LOW);

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

    //Configura EEPROM e acquisisce/registra id device
    /*EEPROM.begin(EEPROM_SIZE);
    id_r = EEPROM.read(0);    //legge area di memoria "0" che contiene id device
    if (id_r == 0) {
        //Da inserire: Codice per acquisire un progressivo id tramite bluetooth
        EEPROM.write(0, id); 
        EEPROM.commit();
    }
    else {
        id = id_r;
    }*/

    
}

// Loop principale del firmware
void loop() {
    // 1. Acquisizione dati
    currentTimestamp = millis(); // Timestamp attuale
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

    digitalWrite(led, LOW);
    delay(1000); // Attendere un po' prima del prossimo ciclo
    digitalWrite(led, HIGH);
    delay(1000);
}

