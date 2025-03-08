// Data acquisition

#define NUM_SENSORS 59
#define MUX_COUNT 4
#define THRESHOLD 30000 // Soglia in grammi
#define minThreshold 0.1
#define maxThreshold 80
#define conversionFactor 1.0 // Fattore di conversione per i valori dell'ADC

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

    delayMicroseconds(1000); // Tempo di stabilizzazione del segnale
}

// Funzione per selezionare il pin ADC in base al canale
int selectADCpin(int channel, const int* adcPins) {
    switch (channel) {
        case 0 ... 15:
            return adcPins[0];
        case 16 ... 31:
            return adcPins[1];
        case 32 ... 47:
            return adcPins[2];
        case 48 ... 63:
            return adcPins[3];
        break;
    }
}

// funzione di supporto: converte ADC value in un'unità di misura specifica (di pressione)
float convertADCValue(int adcValue) {
    float voltage = (adcValue / 4095.0) * 3.3;  // Converti a tensione, ad esempio
	// Conversione in kg o altro in base a calibrazione (conversionFactor)
    return voltage;
}

// funzione di supporto: controllare che i valori siano accettabili
void checkErrorRange(uint16_t value, int sensorIndex) {
    if (value < minThreshold || value > maxThreshold) {
		// Gestisci errore
		// Serial.print("Errore: valore fuori range per sensore ");
		// Serial.println(sensorIndex);
    }
}

// Funzione per acquisire i dati dai sensori
void acquireData(float* sensorData, const int* enPins, const int* addPins, const int* adcPins) {

	uint32_t currentTimestamp = millis(); // Timestamp attuale

    int sensorsPerMux = 16; // Numero di sensori per MUX
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
            sensorData[sensorIndex] = convertADCValue(sensorValue);

			// Controlla che i valori siano accettabili
            // checkErrorRange(sensorData[sensorIndex], sensorIndex);

        }
    }

}
