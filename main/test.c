#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void convertUidToHex(uint8_t* uid, uint8_t length, char* hexStr) {
    for (uint8_t i = 0; i < length; i++) {
        sprintf(&hexStr[i * 2], "%02X", uid[i]); // %02X för att få två hex-siffror
    }
    hexStr[length * 2] = '\0'; // Nullterminera strängen
}

void generate_random_shortID(char *short_id, size_t size) {
    // Generera ett 7-siffrigt heltal
    srand(time(NULL)); // Initiera slumpgeneratorn
    int random_number = rand() % 10000000; // Generera ett tal mellan 0 och 9 999 999

    // Formatera talet som en 7-siffrig sträng
    snprintf(short_id, size, "%07d", random_number); // 7 siffror + nullterminator
}