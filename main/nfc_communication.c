#include "nfc_communication.h"
#include "esp_log.h"
#include "driver/i2c.h"
#include "lib/pn532.h"
#include "printer.h"
#include "http_client.h"
#include "nvs.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>
#include <stdio.h> 

#define SCL_PIN          22
#define SDA_PIN          23
#define RESET_PIN        21
#define IRQ_PIN          12
#define I2C_PORT         I2C_NUM_0
#define I2C_FREQ_HZ      100000

static const char *TAG = "NFC_CLIENT";

// ------------------------------------------------------
// Helper: turn a byte-array UID into a hex string (upper-case)
// ------------------------------------------------------
static void convertUidToHex(const uint8_t *uid, uint8_t uidLength, char *uidHex)
{
    for (uint8_t i = 0; i < uidLength; i++) {
        sprintf(uidHex + i*2, "%02X", uid[i]);
    }
    uidHex[uidLength*2] = '\0';
}

// Global definition
char latest_device_id[16] = "";

// Läs sparat device_id från NVS
void load_device_id(char *out, size_t max_len)
{
    nvs_handle_t h;
    if (nvs_open("storage", NVS_READONLY, &h) == ESP_OK) {
        size_t len = max_len;
        if (nvs_get_str(h, "device_id", out, &len) != ESP_OK) {
            out[0] = '\0';
        }
        nvs_close(h);
    } else {
        out[0] = '\0';
    }
}

void init_nfc(void)
{
    // 1) I²C-konfiguration
    i2c_config_t cfg = {
        .mode             = I2C_MODE_MASTER,
        .sda_io_num       = SDA_PIN,
        .scl_io_num       = SCL_PIN,
        .sda_pullup_en    = GPIO_PULLUP_ENABLE,
        .scl_pullup_en    = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_FREQ_HZ,
    };
    ESP_ERROR_CHECK(i2c_param_config(I2C_PORT, &cfg));
    ESP_ERROR_CHECK(i2c_driver_install(I2C_PORT, I2C_MODE_MASTER, 0, 0, 0));

    // 2) Initiera PN532
    if (init_PN532_I2C(SDA_PIN, SCL_PIN, RESET_PIN, IRQ_PIN, I2C_PORT) != ESP_OK) {
        PR_NFC("Init PN532 misslyckades");
        return;
    }

    // 3) Läs firmware-version och konfigurera SAM
    uint32_t fw = getPN532FirmwareVersion();
    if (fw) {
        PR_NFC("PN532 FW Version: 0x%06X", (unsigned int)fw);
        SAMConfig(0x01, 1);
    } else {
        PR_NFC("Ingen firmwareversion");
    }
}

void nfc_communication(void)
{
    uint8_t uid[7];
    uint8_t uidLength = 0;

    // Läs passiv tag
    if (!readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 1000)) {
        PR_NFC("Ingen NFC-tag hittad");
        return;
    }

    // Konvertera UID till hex-sträng
    char uidHex[16];
    convertUidToHex(uid, uidLength, uidHex);
    PR_NFC("UID: %s", uidHex);

    // Logga till Google Sheet
    if (log_event_request(SCRIPT_URL, uidHex) == ESP_OK) {
        PR_NFC("Loggning lyckades");
    } else {
        PR_NFC("Loggning misslyckades");
    }

    // Läs sparat device_id och uppdatera latest_device_id
    char dev[16];
    load_device_id(dev, sizeof(dev));
    if (dev[0]) {
        // Spara i global
        strncpy(latest_device_id, dev, sizeof(latest_device_id) - 1);
        latest_device_id[sizeof(latest_device_id) - 1] = '\0';
        PR_NFC("Device ID: %s", latest_device_id);
    }

    vTaskDelay(pdMS_TO_TICKS(500));
}