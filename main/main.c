#include "wifi.h"
#include "http_client.h"
#include "sensor_handler.h"
#include "lib/pn532.h"
#include "test.h"
#include "nfc_communication.h"
#include "http_server.h" 

#define TAG "NFC_TEST"





void app_main() {
    
    wifi_init_sta();
    init_nfc();
    init_sensors();
    start_http_server();

    while (true) {
        nfc_communication();
        check_sensors(); 
        //vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}