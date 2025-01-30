// data compression using LZW
std::vector<uint16_t> compressDataLZW(const uint16_t *data, size_t length) {
    std::unordered_map<std::vector<uint16_t>, uint16_t> dictionary;
    std::vector<uint16_t> output, sequence;
    uint16_t dictSize = 256; // Inizializzazione base

    for (int i = 0; i < length; i++) {
        sequence.push_back(data[i]);
        if (dictionary.find(sequence) == dictionary.end()) {
            dictionary[sequence] = dictSize++;
            sequence.pop_back();
            output.push_back(dictionary[sequence]);
            sequence = { data[i] };
        }
    }
    return output;
}

std::vector<int> LZW_compress(const std::vector<float>& inputData) {
    // Dizionario iniziale
    std::map<std::string, int> dictionary;
    for (int i = 0; i < 256; i++) {
        dictionary[std::string(1, i)] = i; // Aggiungi tutti i singoli caratteri
    }

    std::string currentString = "";
    std::vector<int> compressedData;
    int dictSize = 256;

    for (float value : inputData) {
        // Converte il valore in stringa
        std::string nextString = currentString + String(value, 2).c_str();

        if (dictionary.find(nextString) != dictionary.end()) {
            currentString = nextString;
        }
        else {
            compressedData.push_back(dictionary[currentString]);

            // Aggiungi la nuova stringa al dizionario
            if (dictSize < DICT_MAX_SIZE) {
                dictionary[nextString] = dictSize++;
            }

            currentString = String(value, 2).c_str();  // Ripristina il carattere corrente
        }
    }

    // Output finale
    if (!currentString.empty()) {
        compressedData.push_back(dictionary[currentString]);
    }

    return compressedData;
}

void compressAndSendData(float* sensorData, int numSensors) {
    // Prepara i dati in formato vettore
    std::vector<float> inputData(sensorData, sensorData + numSensors);

    // Comprimi i dati
    std::vector<int> compressedData = LZW_compress(inputData);

    // Trasmissione dei dati compressi via Bluetooth (esempio)
    for (int code : compressedData) {
        Serial.print(code);
        Serial.print(" ");
    }
    Serial.println();
}