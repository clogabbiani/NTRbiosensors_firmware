// Filtro semplice per ridurre il rumore
uint16_t filterData(uint16_t rawData) {
    static uint16_t prevData = 0;
    uint16_t filteredData = (prevData + rawData) / 2; // Filtro passa-basso
    prevData = filteredData;
    return filteredData;
}

