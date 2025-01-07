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