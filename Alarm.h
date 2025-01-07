
void checkThreshold() {
    uint32_t totalLoad = 0;
    for (int i = 0; i < NUM_SENSORS; i++) {
        totalLoad += sensorData[i];
    }
    if (totalLoad > THRESHOLD) {
        alarmTriggered = true;
        transmitAlarm(); // Trasmette allarme in tempo reale
    }
}