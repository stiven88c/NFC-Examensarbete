#include "nfc_communication.h"
#include "esp_log.h"
#include "lib/pn532.h"
#include "test.h"
#include "http_client.h"
#include "http_server.h"

// Definiera GPIO-pinnar för PN532
#define SCL_PIN 22
#define SDA_PIN 23
#define RESET_PIN 21
#define IRQ_PIN 12
#define PN532_I2C_PORT 0

#define TAG "NFC_COMMUNICATION"


void init_nfc() {
    esp_err_t ret = init_PN532_I2C(SDA_PIN, SCL_PIN, RESET_PIN, IRQ_PIN, PN532_I2C_PORT);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "NFC initialization failed");
        return;
    }

    uint32_t firmware_version = getPN532FirmwareVersion();
    if (firmware_version != 0) {
        ESP_LOGI(TAG, "PN532 Firmware Version: 0x%06X", (unsigned int)firmware_version);
    } else {
        ESP_LOGE(TAG, "Failed to get firmware version");
    }

    uint8_t irq_setting = 0x01;
    uint8_t timeout_setting = 1;
    if (!SAMConfig(irq_setting, timeout_setting)) {
        ESP_LOGE(TAG, "Failed to configure SAM");
        return;
    }
}

void nfc_communication() {
    uint8_t uid[7];
    uint8_t uidLength;
    uint16_t timeout_ms = 1000;

    bool success = readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, timeout_ms);

    if (success) {
        ESP_LOGI(TAG, "Found NFC tag with UID:");
        for (uint8_t i = 0; i < uidLength; i++) {
            ESP_LOGI(TAG, "UID[%d]: %d", i, uid[i]);
        }

        char uidHex[15];
        convertUidToHex(uid, uidLength, uidHex);
        ESP_LOGI(TAG, "UID in hex: %s", uidHex);

        int status_code;
        esp_err_t delete_result = block_tag_request(API_URL, "Unknown", uidHex, &status_code);

        if (status_code == HTTP_STATUS_NOT_FOUND) {
            ESP_LOGE(TAG, "Tag not found, generating short UID");
            char short_uid[9];
            generate_random_shortID(short_uid, sizeof(short_uid));
            ESP_LOGI(TAG, "Generated short UID: %s", short_uid);

            esp_err_t post_result = create_tag_request(API_URL, uidHex, short_uid);
            if (post_result != ESP_OK) {
                ESP_LOGE(TAG, "Failed to send POST request");
            } else {
                ESP_LOGI(TAG, "POST request sent successfully");
                show_message("Välkommen till IT avdelningen!", 1000); // Meddelande för ny tag
            }
        } else if (status_code == HTTP_STATUS_CONFLICT) {
            show_message("Välkommen till jobbet!", 1000); // Meddelande ut
            ESP_LOGE(TAG, "TagAlreadyBlocked, sending PATCH request");
            esp_err_t patch_result = update_tag_request(API_URL, "Unknown", uidHex);
            if (patch_result != ESP_OK) {
                ESP_LOGE(TAG, "Failed to send PATCH request");
            } else {
                ESP_LOGI(TAG, "PATCH request sent successfully");
            }
        } else {
            show_message("Tack för idag, hoppas vi syns imorgon!", 1000); // Meddelande för blockerat tag
        }

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    } else {
        ESP_LOGE(TAG, "No NFC tag found or read failed");
    }
}