#include <stdio.h>
#include <string.h>
#include <time.h>

#include "esp_log.h"
#include "esp_err.h"
#include "esp_http_client.h"
#include "esp_crt_bundle.h"
#include "esp_sntp.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "printer.h"

#include "http_client.h"
#include "nfc_communication.h" 

static const char *TAG = "HTTP_CLIENT";

//------------------------------------------------------
// SNTP Initialization
//------------------------------------------------------
void http_client_init_sntp(void)
{
   PR_CLIENT("Initiera SNTP");
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "pool.ntp.org");
    sntp_init();

    time_t now = 0;
    struct tm ti = {0};
    int retry = 0, max = 10;
    while (retry < max && ti.tm_year < (2020 - 1900)) {
        PR_CLIENT("Väntar på SNTP… (%d/%d)", retry+1, max);
        vTaskDelay(pdMS_TO_TICKS(1000));
        time(&now);
        localtime_r(&now, &ti);
        retry++;
    }
    if (ti.tm_year < (2020 - 1900)) {
        PR_CLIENT("SNTP ej tillgänglig efter %d försök", max);
    } else {
        PR_CLIENT("SNTP satt: %s", asctime(&ti));
    }
}

//------------------------------------------------------
// HTTP Event Handler
//------------------------------------------------------
static esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
    if (evt->event_id == HTTP_EVENT_ON_DATA && evt->data_len > 0) {
        //Om jag vill se den HTML-data Google scripten skickar till ESP32
        //printf("%.*s\n", (int)evt->data_len, (char*)evt->data);
    }
    return ESP_OK;
}

//------------------------------------------------------
// Helper to perform JSON POST
//------------------------------------------------------
static esp_err_t perform_json_post(const char *url, const char *json)
{
    esp_http_client_config_t cfg = {
        .url                    = url,
        .event_handler          = _http_event_handler,
        .crt_bundle_attach      = esp_crt_bundle_attach,
        .max_redirection_count  = 3,           
    };
    esp_http_client_handle_t client = esp_http_client_init(&cfg);
    if (!client) return ESP_FAIL;

    esp_http_client_set_method(client, HTTP_METHOD_POST);
    esp_http_client_set_header(client, "Content-Type","application/json");
    esp_http_client_set_post_field(client, json, strlen(json));

    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        PR_CLIENT("%s status=%d", __func__, esp_http_client_get_status_code(client));
    } else {
        PR_CLIENT("%s failed: %s", __func__, esp_err_to_name(err));
    }
    esp_http_client_cleanup(client);
    return err;
}

//------------------------------------------------------
// 1) Log Event
//------------------------------------------------------
esp_err_t log_event_request(const char *script_url, const char *uid)
{
    char device_id[16];
    load_device_id(device_id, sizeof(device_id));
    if (!device_id[0]) {
        ESP_LOGW(TAG, "Ingen device_id satt – logg avbryts");
        return ESP_FAIL;
    }

    time_t now;
    struct tm t;
    char ts[32];
    time(&now);
    localtime_r(&now, &t);
    strftime(ts, sizeof(ts), "%Y-%m-%dT%H:%M:%SZ", &t);

    char payload[256];
    int n = snprintf(payload, sizeof(payload),
        "{\"action\":\"logEvent\",\"uid\":\"%s\","
        "\"device_id\":\"%s\",\"timestamp\":\"%s\"}",
        uid, device_id, ts);
    if (n < 0 || n >= sizeof(payload)) {
        PR_CLIENT("JSON overflow");
        return ESP_ERR_NO_MEM;
    }
    return perform_json_post(script_url, payload);
}

//------------------------------------------------------
// 2) Register Device
//------------------------------------------------------
esp_err_t register_device_request(const char *script_url,
                                  const char *mac,
                                  const char *device_id,
                                  const char *url,
                                  const char *desc)
{
    char payload[256];
    int n = snprintf(payload, sizeof(payload),
        "{\"action\":\"registerDevice\",\"mac\":\"%s\","
        "\"device_id\":\"%s\",\"url\":\"%s\",\"desc\":\"%s\"}",
        mac, device_id, url?url:"", desc?desc:""
    );
    if (n < 0 || n >= sizeof(payload)) {
        PR_CLIENT("JSON overflow");
        return ESP_ERR_NO_MEM;
    }
    return perform_json_post(script_url, payload);
}
