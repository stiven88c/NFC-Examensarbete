#ifndef NFC_COMMUNICATION_H
#define NFC_COMMUNICATION_H

#include "esp_err.h" 

#define HTTP_STATUS_CONFLICT 409
#define HTTP_STATUS_NOT_FOUND 404


void init_nfc();
void nfc_communication();


#endif 