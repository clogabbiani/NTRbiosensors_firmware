#include <Wire.h>

byte ind = 0x6A;

int16_t convert_twos(uint16_t value) {
    // Se il valore è più grande o uguale a 32768 (2^15), è un numero negativo in complemento a due
    if (value >= 32768) {
        return (int16_t)(value - 65536); // 65536 è 2^16, quindi per ottenere il valore negativo, sottrai 65536
    }
    else {
        return (int16_t)value; // Se il valore è più piccolo di 32768, è già il suo valore positivo in complemento a due
    }
}

void I2C_scanner_array(byte* address_array) {
    byte error, address;
    int device_found;

    device_found = 0;

    for (address = 1; address < 128; address++) {
        Wire.beginTransmission(address);
        error = Wire.endTransmission(); //Questa funziona ritorna 0 se slave presente, altrimenti 4
        if (error == 0) {
            device_found = 1;
            Serial.print("Device found at address ");
            Serial.print(address);
            Serial.println("");
            address_array[address] = address;
        }
    }
}
float readAccX() {
    uint8_t AccX_L;
    uint8_t AccX_H;
    uint16_t AccX;
    int16_t AccX_signed;
    float AccX_conv;
    Wire.beginTransmission(ind);
    Wire.write(0x28);   //Accesso al registro OUTX_L_A
    Wire.endTransmission(false);
    Wire.requestFrom(ind, 1);
    AccX_L = Wire.read();
    Wire.endTransmission();

    Wire.beginTransmission(ind);
    Wire.write(0x29);   //Accesso al registro OUTX_H_A
    Wire.endTransmission(false);
    Wire.requestFrom(ind, 1);
    AccX_H = Wire.read();
    Wire.endTransmission();
    AccX = AccX_H << 8;
    AccX = AccX | AccX_L;
    AccX_signed = (int16_t)(AccX);
    AccX_conv = (float)AccX_signed / 16393;
    return(AccX_conv);

}
float readAccY() {
    uint8_t AccY_L;
    uint8_t AccY_H;
    uint16_t AccY;
    int16_t AccY_signed;
    float AccY_conv;
    Wire.beginTransmission(ind);
    Wire.write(0x2A);   //Acceso al registro OUTY_L_A
    Wire.endTransmission(false);
    Wire.requestFrom(ind, 1);
    AccY_L = Wire.read();
    Wire.endTransmission();

    Wire.beginTransmission(ind);
    Wire.write(0x2B);   //Acceso al registro OUTY_H_A
    Wire.endTransmission(false);
    Wire.requestFrom(ind, 1);
    AccY_H = Wire.read();
    Wire.endTransmission();
    AccY = AccY_H << 8;
    AccY = AccY | AccY_L;
    AccY_signed = (int16_t)(AccY);
    AccY_conv = (float)AccY_signed / 16393;
    return(AccY_conv);

}
float readAccZ() {
    uint8_t AccZ_L = 0;
    uint8_t AccZ_H = 0;
    uint16_t AccZ = 0;
    int16_t AccZ_signed;
    float AccZ_conv;
    Wire.beginTransmission(ind);
    Wire.write(0x2C);   //Acceso al registro OUTZ_L_A
    Wire.endTransmission(false);
    Wire.requestFrom(ind, 1);
    AccZ_L = Wire.read();
    Wire.endTransmission();

    Wire.beginTransmission(ind);
    Wire.write(0x2D);   //Acceso al registro OUTZ_H_A
    Wire.endTransmission(false);
    Wire.requestFrom(ind, 1);
    AccZ_H = Wire.read();
    Wire.endTransmission();
    AccZ = AccZ_H << 8;
    AccZ = AccZ | AccZ_L;
    AccZ_signed = (int16_t)(AccZ);
    AccZ_conv = (float)AccZ_signed / 16393;
    return(AccZ_conv);

}

float readGirX() {
    uint8_t GirX_L;
    uint8_t GirX_H;
    uint16_t GirX;
    int16_t GirX_signed;
    float GirX_conv;
    Wire.beginTransmission(ind);
    Wire.write(0x22);   //Accesso al registro OUTX_L_G
    Wire.endTransmission(false);
    Wire.requestFrom(ind, 1);
    GirX_L = Wire.read();
    Wire.endTransmission();

    Wire.beginTransmission(ind);
    Wire.write(0x23);   //Accesso al registro OUTX_H_G
    Wire.endTransmission(false);
    Wire.requestFrom(ind, 1);
    GirX_H = Wire.read();
    Wire.endTransmission();
    GirX = GirX_H << 8;
    GirX = GirX | GirX_L;
    GirX_signed = (int16_t)(GirX);
    GirX_conv = (float)GirX_signed / 114.285;
    return(GirX_conv);

}
float readGirY() {
    uint8_t GirY_L;
    uint8_t GirY_H;
    uint16_t GirY;
    int16_t GirY_signed;
    float GirY_conv;
    Wire.beginTransmission(ind);
    Wire.write(0x24);   //Accesso al registro OUTX_L_G
    Wire.endTransmission(false);
    Wire.requestFrom(ind, 1);
    GirY_L = Wire.read();
    Wire.endTransmission();

    Wire.beginTransmission(ind);
    Wire.write(0x25);   //Accesso al registro OUTX_H_G
    Wire.endTransmission(false);
    Wire.requestFrom(ind, 1);
    GirY_H = Wire.read();
    Wire.endTransmission();
    GirY = GirY_H << 8;
    GirY = GirY | GirY_L;
    GirY_signed = (int16_t)(GirY);
    GirY_conv = (float)GirY_signed / 114.285;
    return(GirY_conv);

}
float readGirZ() {
    uint8_t GirZ_L;
    uint8_t GirZ_H;
    uint16_t GirZ;
    int16_t GirZ_signed;
    float GirZ_conv;
    Wire.beginTransmission(ind);
    Wire.write(0x26);   //Accesso al registro OUTX_L_G
    Wire.endTransmission(false);
    Wire.requestFrom(ind, 1);
    GirZ_L = Wire.read();
    Wire.endTransmission();

    Wire.beginTransmission(ind);
    Wire.write(0x27);   //Accesso al registro OUTX_H_G
    Wire.endTransmission(false);
    Wire.requestFrom(ind, 1);
    GirZ_H = Wire.read();
    Wire.endTransmission();
    GirZ = GirZ_H << 8;
    GirZ = GirZ | GirZ_L;
    GirZ_signed = (int16_t)(GirZ);
    GirZ_conv = (float)GirZ_signed / 114.285;
    return(GirZ_conv);

}

float readTemp() {
    uint8_t Temp_L;
    uint8_t Temp_H;
    uint16_t Temp;
    int16_t Temp_signed;
    float Temp_conv;
    Wire.beginTransmission(ind);
    Wire.write(0x20);   //Acceso al registro OUT_TEMP_L
    Wire.endTransmission(false);
    Wire.requestFrom(ind, 1);
    Temp_L = Wire.read();
    Wire.endTransmission();

    delayMicroseconds(100);

    Wire.beginTransmission(ind);
    Wire.write(0x21);   //Acceso al registro OUT_TEMP_H
    Wire.endTransmission(false);
    Wire.requestFrom(ind, 1);
    Temp_H = Wire.read();
    Wire.endTransmission();
    Temp = Temp_H << 8;
    Temp = Temp | Temp_L;
    Temp_signed = (int16_t)(Temp);
    Temp_conv = ((float)Temp_signed / 256) + 25;
    return(Temp_conv);
}

void setupIMU() {
    Wire.beginTransmission(ind);
    Wire.write(0x10);   //Accesso al registro di controllo dell'accelerometro CTRL1_XL
    Wire.write(0x60);   //Selezionata modalità High Performance ODR = 416Hz, FS = +-1g
    Wire.endTransmission();

    Wire.beginTransmission(ind);
    Wire.write(0x11);   //Accesso al registro di controllo del giroscopio CTRL2_G
    Wire.write(0x60);   //Selezionata modalità High Performance ODR = 416Hz, FS = +-250dps
    Wire.endTransmission();
}

void acquireIMUdata(float* IMUarray) {
    IMUarray[0] = readAccX();
    IMUarray[1] = readAccY();
    IMUarray[2] = readAccZ();
    IMUarray[3] = readGirX();
    IMUarray[4] = readGirY();
    IMUarray[5] = readGirZ();
}