bool validateParameters(int threshold, int sensorCount) {
    // Controllo parametri validi
    if (threshold < 0 || threshold > 30000) { // Supponendo un range massimo di 30kg
        Serial.println("Errore: Soglia fuori dal range accettabile");
        return false;
    }

    if (sensorCount <= 0 || sensorCount > 59) {
        Serial.println("Errore: Numero di sensori non valido");
        return false;
    }
    return true;
}

void setup() {
    Serial.begin(115200);
    
    // Verifica dei parametri prima di procedere
    int threshold = 30000; // Esempio di soglia
    int sensorCount = 59;

    if (!validateParameters(threshold, sensorCount)) {
        Serial.println("Errore: Configurazione non valida, rivedere i parametri.");
        while (1); // Blocca il firmware in caso di errore grave
    }
    
    // Altre configurazioni di inizializzazione
}
