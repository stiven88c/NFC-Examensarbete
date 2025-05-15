#ifndef TEST_H
#define TEST_H

#include <stdint.h>
#include <stddef.h>

// Funktion för att konvertera UID till hex-sträng
void convertUidToHex(uint8_t* uid, uint8_t length, char* hexStr);

// Funktion för att generera en slumpmässig 8-siffrig ShortID
void generate_random_shortID(char *short_uid, size_t size);

#endif // TEST_H