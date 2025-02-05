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

# GIT methodology (when using only the main, no branches)
      1. git pull origin main 
      2. Modifica i file
      3. git add . 
      4. git commit -m "Descrizione"
      5. git push origin main
