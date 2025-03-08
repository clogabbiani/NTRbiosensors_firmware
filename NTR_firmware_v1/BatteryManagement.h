#include <Wire.h>


void clearBattery(int clearbat) {   //restart MAX835
    digitalWrite(clearbat, LOW);
    digitalWrite(clearbat, HIGH);
    digitalWrite(clearbat, LOW);
}


// Funzione per la scansione I2C
byte I2C_scanner() {
    byte error, address, address_return;
    int device_found;

    device_found = 0;

    // Scansione degli indirizzi I2C
    for (address = 1; address < 128; address++) {
        Wire.beginTransmission(address);
        error = Wire.endTransmission(); //Questa funziona ritorna 0 se errore presente, altrimenti 1
        if (error == 0) {
            device_found = 1;
            address_return = address;
        }
    }
    return address_return;
}

// Funzione per acquisire il livello della batteria tramite I2C
float I2C_battery_level() {

    uint16_t V_MSB, V_LSB, Meas_V;
    float Meas_V_conv;
    byte ind;
    ind = I2C_scanner();

    Wire.beginTransmission(ind);
    Wire.write(0x01); //Indirizzo del Control Register
    Wire.write(0x7C); //Valore del Control Register da scrivere per far compiere un acquisizione all'ADC (modalità default ADC è sleep)
    Wire.endTransmission();

    Wire.beginTransmission(ind);
    Wire.write(0x08);
    Wire.endTransmission(false);
    Wire.requestFrom(ind, 1);
    V_MSB = Wire.read();
    Wire.endTransmission();

    Wire.beginTransmission(ind);
    Wire.write(0x09);
    Wire.endTransmission(false);
    Wire.requestFrom(ind, 1);
    V_LSB = Wire.read();
    Wire.endTransmission();

    V_MSB = V_MSB << 8;
    Meas_V = V_MSB | V_LSB;

    Meas_V_conv = 23.6 * Meas_V / 65535;

    Wire.beginTransmission(ind);
    Wire.write(0x01); //Indirizzo del Control Register
    Wire.write(0x3C); //Valore del Control Register da scrivere per far rimetter configurazione default (ADC sleep)
    Wire.endTransmission();

    return Meas_V_conv;

}

