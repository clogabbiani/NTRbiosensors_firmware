// Data acquisition
#include <Arduino.h>
#include <Wire.h>

#define NUM_SENSORS 59
#define MUX_COUNT 4
#define THRESHOLD 30000 // Soglia in grammi
#define minThreshold 1
#define maxThreshold 80
#define conversionFactor 1

void selectSensorMux(int muxIndex, int channel, const int* enPins, const int* addPins) {
    // Disabilita tutti i MUX
    for (int i = 0; i < 4; i++) {
        digitalWrite(enPins[i], HIGH); // HIGH disabilita il MUX
    }

    // Abilita il MUX selezionato (attivo basso)
    digitalWrite(enPins[muxIndex], LOW);

    // Imposta l'indirizzo del canale sui PIN di Indirizzo
    for (int i = 0; i < 4; i++) {
        digitalWrite(addPins[i], (channel >> i) & 1); // Estrae il bit i-esimo dell'indirizzo
    }

    delayMicroseconds(10); // Tempo di stabilizzazione del segnale
}

int selectADCpin(int channel, const int* adcPins) {
    switch (channel) {
        case 0 ... 15:
            return adcPins[0];
        case 16 ... 31:
            return adcPins[1];
        case 32 ... 46:
            return adcPins[2];
        case 47 ... 59:
            return adcPins[3];
        break;
    }
}

// funzione di supporto: converte ADC value in un'unità di misura specifica (di pressione)
uint16_t convertADCValue(int adcValue) {
    float voltage = (adcValue / 4095.0) * 3.3;  // Converti a tensione, ad esempio
    // Conversione in kg o altro in base a calibrazione
    return static_cast<uint16_t>(voltage * conversionFactor);
}

// funzione di supporto: controllare che i valori siano accettabili
void checkErrorRange(uint16_t value, int sensorIndex) {
    if (value < minThreshold || value > maxThreshold) {
        Serial.print("Valore fuori range per il sensore ");
        Serial.println(sensorIndex);
    }
}

void acquireData(uint16_t* sensorData, const int* enPins, const int* addPins, const int* adcPins) {
    int sensorsPerMux = NUM_SENSORS / 4; // Numero di sensori per MUX
    for (int mux = 0; mux < 4; mux++) {
        for (int channel = 0; channel < sensorsPerMux; channel++) {
            int sensorIndex = mux * sensorsPerMux + channel;

            // Seleziona il MUX e il canale
            selectSensorMux(mux, channel, enPins, addPins);

            //Seleziona il pin ADC da usare
            int ADC_PIN = selectADCpin(channel, adcPins);

            // Legge il valore del sensore dall'ADC
            int sensorValue = analogRead(ADC_PIN);
            sensorData[sensorIndex] = convertADCValue(sensorValue);

            Serial.print("Sensore ");
            Serial.println(sensorIndex);
            Serial.print("Raw data ");
            Serial.println(sensorValue);
            Serial.print("Converted data ");
            Serial.println(sensorData[sensorIndex]);

            // Controlla eventuali errori
            checkErrorRange(sensorData[sensorIndex], sensorIndex);

            Serial.println();
        }
    }
}

byte I2C_scanner() {
    byte error, address;
    int device_found;

    address = 1;
    device_found = 0;

    while (address < 127 && device_found == 0) {
        Wire.beginTransmission(address);
        error = Wire.endTransmission(); //Questa funziona ritorna 0 se slave presente, altrimenti 4
        if (error == 0) {
            device_found = 1;
        }
        else if (error == 4) {
            address++;
        }
    }
    return address;
}

uint16_t I2C_battery_level() {

    uint16_t V_MSB, V_LSB, Meas_V;
    byte ind;
    ind = I2C_scanner();

    Wire.beginTransmission(ind);
    Wire.write(0x01); //Indirizzo del Control Register
    Wire.write(0x7C); //Valore del Control Register da scrivere per far compiere un acquisizione all'ADC (modalità default ADC è sleep)
    Wire.endTransmission();

    Wire.beginTransmission(ind);
    Wire.write(0x08);
    Wire.endTransmission();
    Wire.requestFrom(ind, (uint8_t)1);
    V_MSB = Wire.read();

    Wire.beginTransmission(ind);
    Wire.write(0x09);
    Wire.endTransmission();
    Wire.requestFrom(ind, (uint8_t)1);
    V_LSB = Wire.read();

    V_MSB = V_MSB << 16;
    Meas_V = V_MSB | V_LSB;

    Wire.beginTransmission(ind);
    Wire.write(0x01); //Indirizzo del Control Register
    Wire.write(0x3C); //Valore del Control Register da scrivere per far rimetter configurazione default (ADC sleep)
    Wire.endTransmission();

    return Meas_V;
}
