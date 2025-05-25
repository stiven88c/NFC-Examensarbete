#include "wifi.h"
#include "nfc_communication.h"
#include "printer.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void app_main(void)
{
    wifi_init_sta();
    init_nfc();
    while (1) {
        nfc_communication();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
