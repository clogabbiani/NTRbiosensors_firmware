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

# Branch naming conventions
 - "main" is the main approved code, production
 - "develop" is the staging branch
 - "feature-featurename" for new functionalities
 - "bugfix-bugname" for corrections

# GIT methodology
 - before starting: always execute a pull to the main branch to update the local branch
 - during work: execute frequent commit with clear messages
 - at the end: execute a push of the local branch to the main, creating a Pull Request
