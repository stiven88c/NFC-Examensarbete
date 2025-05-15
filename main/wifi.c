#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"  
#include "wifi.h"

static const char *TAG = "WIFI";

void wifi_init_sta(void) {
   
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret); 

   
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();
    
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
    
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,             
            .password = WIFI_PASSWORD,     
        },
    };
    
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config); 
    esp_wifi_start();

    int wifi_retry_count = 0;
    while (wifi_retry_count < MAX_WIFI_RETRIES && esp_wifi_connect() != ESP_OK) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        wifi_retry_count++;
        ESP_LOGI(TAG, "Wi-Fi retry count: %d", wifi_retry_count);
    }

    if (wifi_retry_count == MAX_WIFI_RETRIES) {
        ESP_LOGE(TAG, "Failed to connect to Wi-Fi after %d attempts", MAX_WIFI_RETRIES);
    } else {
        ESP_LOGI(TAG, "Connected to Wi-Fi");
    }
}