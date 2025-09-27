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
// conversione usando i parametri di calibrazione secondo modello asintotico inverso ---
// kg = c * ((V - a) / (d - V))^(1/b)
extern float paramA[59], paramB[59], paramC[59], paramD[59];

static inline float clampf(float x, float lo, float hi) {
    return (x < lo) ? lo : (x > hi) ? hi : x;
}
float convertADCValue(int adcValue, int sensorIndex) {
    // 12 bit su 3.3V
    const float V = (adcValue / 4095.0f) * 3.3f;

    // Se index fuori range, ritorna tensione come fallback
    if (sensorIndex < 0 || sensorIndex >= 59) return V;

    const float a = paramA[sensorIndex];
    const float b = paramB[sensorIndex];
    const float c = paramC[sensorIndex];
    const float d = paramD[sensorIndex];

    // debug sui parametri di calibrazione
    if (sensorIndex < 5) {
        Serial.printf("Convert[%d]: ADC=%d V=%.3f params: a=%.3f b=%.3f c=%.3f d=%.3f\n",
            sensorIndex, adcValue, V, a, b, c, d);
    }

    // Protezioni:
    // - b vicino a 0 -> eviti divisione 1/b
    // - (d - V) e (V - a) devono essere > 0 per avere argomento positivo
    if (fabsf(b) < 1e-6f) return 0.0f;  // oppure V

    // clamp V dentro (a+eps, d-eps)
    const float eps = 1e-6f;
    float Vy = clampf(V, a + eps, d - eps);

    float num = Vy - a;
    float den = d - Vy;
    if (den <= 0.0f || num <= 0.0f) return 0.0f; // fuori dominio → 0

    float ratio = num / den;

    // Se per rumore ratio è negativo o zero, proteggi
    if (ratio <= 0.0f) return 0.0f;

    // kg
    float kg = c * powf(ratio, 1.0f / b);

    // opzionale: clamp kg a un range atteso fisicamente
    if (!isfinite(kg)) kg = 0.0f;
    return kg;
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
            /*if (sensorIndex < 8) {
                Serial.print("Debug first 8 sensors");
                Serial.printf("Sensor[%d]: ADC=%d ", sensorIndex, sensorValue);
            }*/

			// Converte il valore dell'ADC in un'unità di misura specifica
            // debug per la conversione in kg
            float kgValue = convertADCValue(sensorValue, sensorIndex);
            Serial.printf("S[%d]: ADC=%d V=%.3f kg=%.3f\n",sensorIndex, sensorValue,
                (sensorValue / 4095.0f) * 3.3f, kgValue);
            /*if (sensorIndex < 5 && kgValue != 0.0f) {
                Serial.printf("S[%d]: ADC=%d V=%.3f kg=%.3f\n",
                    sensorIndex, sensorValue,
                    (sensorValue / 4095.0f) * 3.3f, kgValue);
            }*/
            sensorData[sensorIndex] = kgValue;

			// Controlla che i valori siano accettabili
            // checkErrorRange(sensorData[sensorIndex], sensorIndex);

        }
    }

}
