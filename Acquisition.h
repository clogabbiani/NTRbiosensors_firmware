// Data acquisition
#include <Arduino.h>
#include <Wire.h>

#define NUM_SENSORS 59
#define MUX_COUNT 4
#define THRESHOLD 30000 // Soglia in grammi
#define minThreshold 0.1
#define maxThreshold 80
#define conversionFactor 1.0 // Fattore di conversione per i valori dell'ADC
#define BATCH_SIZE 100  // Numero di righe per batch

// Struttura per i dati del sensore
struct SensorData {
    uint32_t timestamp;       // Timestamp in millisecondi
    float values[NUM_SENSORS]; // Valori acquisiti dai sensori
};

// aggiunta di un buffer per i dati
SensorData dataBuffer[BATCH_SIZE];
int bufferIndex = 0;

// Funzione per selezionare il MUX e il canale
void selectSensorMux(int muxIndex, int channel, const int* enPins, const int* addPins) {

    // Disabilita tutti i MUX mettendoli su HIGH
    for (int i = 0; i < 4; i++) {
        digitalWrite(enPins[i], HIGH);
    }

    // Abilita il MUX selezionato (attivo=low, spento=HIGH)
    digitalWrite(enPins[muxIndex], LOW);

    // Imposta l'indirizzo del canale sui PIN di Indirizzo
    for (int i = 0; i < 4; i++) {
		// Imposta il bit i-esimo dell'indirizzo
        digitalWrite(addPins[i], (channel >> i) & 1);
    }

    delayMicroseconds(100); // Tempo di stabilizzazione del segnale
}

// Funzione per selezionare il pin ADC in base al canale
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
float convertADCValue(int adcValue) {
    float voltage = (adcValue / 4095.0) * 3.3;  // Converti a tensione, ad esempio
	// Conversione in kg o altro in base a calibrazione (conversionFactor)
    return static_cast<float>(voltage * conversionFactor);
}

// funzione di supporto: controllare che i valori siano accettabili
void checkErrorRange(uint16_t value, int sensorIndex) {
    if (value < minThreshold || value > maxThreshold) {
		// Gestisci errore
		// Serial.print("Errore: valore fuori range per sensore ");
		// Serial.println(sensorIndex);
    }
}

void compressAndSendData() {
    // Placeholder per la compressione e l'invio dei dati
    // Puoi aggiungere qui la logica per comprimere i dati e inviarli
    // Serial.println("Compressing and sending data...");
}

// Funzione per comprimere e inviare i dati
void addDataToBuffer(uint32_t timestamp, float* values) {
    if (bufferIndex < BATCH_SIZE) {
        dataBuffer[bufferIndex].timestamp = timestamp;
		// Copia i valori nel buffer
        memcpy(dataBuffer[bufferIndex].values, values, sizeof(float) * NUM_SENSORS);
        bufferIndex++;
    }
    else {
        // Gestisci il caso in cui il buffer è pieno
        compressAndSendData();
        bufferIndex = 0;
    }
}

// Funzione per acquisire i dati dai sensori
void acquireData(float* sensorData, const int* enPins, const int* addPins, const int* adcPins) {

	uint32_t currentTimestamp = millis(); // Timestamp attuale

    int sensorsPerMux = 15; // Numero di sensori per MUX
	// Per ogni MUX
    for (int mux = 0; mux < 4; mux++) {
		// Per ogni canale del MUX
        for (int channel = 0; channel < sensorsPerMux; channel++) {

			// Calcola l'indice del sensore
            int sensorIndex = mux * sensorsPerMux + channel;
         
            // Seleziona il MUX e il canale
            selectSensorMux(mux, channel, enPins, addPins);

            //Seleziona il pin ADC da usare
            int ADC_PIN = selectADCpin(sensorIndex, adcPins);

            // Legge il valore del sensore dall'ADC
            int sensorValue = analogRead(ADC_PIN);

			// Converte il valore dell'ADC in un'unità di misura specifica
            sensorData[sensorIndex] = sensorValue; // convertADCValue(sensorValue);

			// Controlla che i valori siano accettabili
            // checkErrorRange(sensorData[sensorIndex], sensorIndex);

        }
    }

    // Aggiungi dati al buffer
    addDataToBuffer(currentTimestamp, sensorData);
}
