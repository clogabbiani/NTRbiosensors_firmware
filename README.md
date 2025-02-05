# NTRbiosensors Firmware

## Descrizione
Progetto per il firmware delle solette sensorizzate **ORME**, che raccoglie dati dai sensori e li trasmette via Bluetooth
=======
This project handles sensor data acquisition, processing, and BLE transmission for sensorized insoles.


---

## Configurazione dell'Ambiente

### Prerequisiti
1. **Visual Studio 2022** con estensione Arduino, Visual Micro e supporto Git.
2. **Git** installato (scaricabile da [git-scm.com](https://git-scm.com/)).
3. Librerie necessarie:
   - [ESP32 BLE NimBLE](https://github.com/h2zero/NimBLE-Arduino)
   - Altre librerie specifiche.

### Setup
1. Clona il repository:
   ```bash
   git clone https://github.com/tuo-utente/tuo-repository.git
Opzionale: creare un branch in cui inserire modifiche 
   ```bash
   git checkout -b feature-nuovo-codice
2. Aggiorna main con ultime modifiche su Git
   ```bash
   git pull origin main 
3. Modifica i file
   ```bash
   git add .
4. Crea un commit parlante delle modifiche 
   ```bash
   git commit -m "Descrizione"
5. Pusha le modifiche al branch usato (main)
   ```bash
   git push origin main
6. Opzionale: se lavoriamo su branch diversi, dopo i push bisogna creare una pull request per fare il merge con il main
