#include "sdkconfig.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "nvs_flash.h"

#include "wifi.h"
#include "printer.h"
#include "http_server.h"   
#include "http_client.h"   
#include "lwip/inet.h"    

static const char *TAG = "WIFI";

// Wi-Fi‐eventhandler
static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                               int32_t event_id, void* event_data)
{
    if (event_id == WIFI_EVENT_STA_START) {
        PR_WIFI("Wi-Fi startat, ansluter…");
        esp_wifi_connect();
    } else if (event_id == WIFI_EVENT_STA_DISCONNECTED) {
        PR_WIFI("Wi-Fi bortkopplad, reconnect…");
        esp_wifi_connect();
    }
}

// IP-eventhandler
static void ip_event_handler(void* arg, esp_event_base_t event_base,
                             int32_t event_id, void* event_data)
{
    if (event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* ev = (ip_event_got_ip_t*)event_data;
        char ipstr[16];
        ip4addr_ntoa_r(&ev->ip_info.ip, ipstr, sizeof(ipstr));
        PR_WIFI("Fick IP: %s", ipstr);

        // 1) Synka tid via SNTP
        http_client_init_sntp();

        // 2) Starta web-servern
        start_http_server();
    }
}

void wifi_init_sta(void)
{
    // 1) Initiera NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
        ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // 2) Initiera TCP/IP och event‐loop
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    // 3) Initiera Wi-Fi-drivrutin
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // 4) Registrera eventhandlers
    ESP_ERROR_CHECK(esp_event_handler_register(
        WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(
        IP_EVENT, IP_EVENT_STA_GOT_IP, ip_event_handler, NULL));

    // 5) Konfigurera SSID/lösen från sdkconfig
    wifi_config_t wconf = {
        .sta = {
            .ssid     = CONFIG_WIFI_SSID,
            .password = CONFIG_WIFI_PASSWORD,
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wconf));
    ESP_ERROR_CHECK(esp_wifi_start());

    PR_WIFI("Försöker ansluta till \"%s\" …", CONFIG_WIFI_SSID);
}