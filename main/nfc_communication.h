#ifndef NFC_COMMUNICATION_H
#define NFC_COMMUNICATION_H

#include "esp_err.h"
#include <stddef.h>
#include "sdkconfig.h"


#define SCRIPT_URL    CONFIG_API_URL
#define API_KEY       CONFIG_API_KEY

extern char latest_device_id[16];
void load_device_id(char *out, size_t max_len);
void init_nfc(void);
void nfc_communication(void);

#endif // NFC_COMMUNICATION_H