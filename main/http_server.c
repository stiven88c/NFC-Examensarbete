#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "esp_http_server.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_mac.h"

#include "printer.h"
#include "http_server.h"
#include "http_client.h"       
#include "nfc_communication.h" 

static const char *TAG = "HTTP_SERVER";

// — Hjälp: spara device_id —
static esp_err_t save_device_id(const char *id) {
    nvs_handle_t h;
    esp_err_t e = nvs_open("storage", NVS_READWRITE, &h);
    if (e != ESP_OK) return e;
    e = nvs_set_str(h, "device_id", id);
    if (e == ESP_OK) e = nvs_commit(h);
    nvs_close(h);
    return e;
}

// — Buffertar för startsidan —
static char page_buf[2048];
static char errbuf_dev[128];

// — Mall för huvudsidan —
static const char *MAIN_TPL =
"<!DOCTYPE html><html lang='sv'><head><meta charset='utf-8'>"
"<meta name='viewport' content='width=device-width,initial-scale=1'>"
"<title>Inställningar</title>"
"<style>body{font-family:sans-serif;padding:20px;}"
"fieldset{margin-bottom:20px;}label,input,button,p{display:block;margin-top:8px;}"  
"legend{font-weight:bold;}""</style>"
"<script>"
"window.addEventListener('DOMContentLoaded',()=>{"
"  document.getElementById('deviceForm').addEventListener('submit',()=>"
"    document.getElementById('devStatus').innerText='Väntar...');"
"});"
"</script></head><body>"
"<h1>Inställningar</h1>"
"<fieldset><legend>Namnge enhet</legend>"
"%s" /* 1: dev-fel */
"<p>Nuvarande Device ID: <strong>%s</strong></p>"
"<form id='deviceForm' method='GET' action='/set'>"
"  <label for='device'>Device ID (1–999):</label>"
"  <input id='device' type='number' name='device' min='1' max='999' required>"
"  <button>Spara enhet</button>"
"</form>"
"<p id='devStatus' style='color:blue;'></p>"
"</fieldset>"
"</body></html>";

// — Bygger huvudsidan —
static char* build_main_page(const char *err, const char *device) {
    const char *e = err && *err ? err : "";
    const char *d = device && *device ? device : "(ej satt)";
    snprintf(page_buf, sizeof(page_buf), MAIN_TPL, e, d);
    return page_buf;
}

// GET / → visa huvudsidan
static esp_err_t html_get_handler(httpd_req_t *req) {
    char device[16];
    load_device_id(device, sizeof(device));
    char *p = build_main_page(NULL, device);
    httpd_resp_set_type(req, "text/html; charset=utf-8");
    return httpd_resp_send(req, p, HTTPD_RESP_USE_STRLEN);
}

// GET /set → spara device_id + registerDevice
static esp_err_t set_get_handler(httpd_req_t *req) {
    char qs[64], device[16], err[128] = "";
    load_device_id(device, sizeof(device));
    if (httpd_req_get_url_query_str(req, qs, sizeof(qs)) == ESP_OK &&
        httpd_query_key_value(qs, "device", device, sizeof(device)) == ESP_OK) {
        int v = atoi(device);
        if (v >= 1 && v <= 999) {
            if (save_device_id(device) == ESP_OK) {
                PR_SERVER("Sparat Device ID: %s", device);
                uint8_t mac[6]; esp_read_mac(mac, ESP_MAC_WIFI_STA);
                char mac_s[18]; snprintf(mac_s, sizeof(mac_s),
                  "%02X:%02X:%02X:%02X:%02X:%02X", mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
                if (register_device_request(SCRIPT_URL, mac_s, device, "", "") != ESP_OK) {
                    strcpy(err, "Fel vid serverregistrering");
                }
            } else {
                snprintf(err, sizeof(err), "Kunde inte spara ID '%s'", device);
            }
        } else {
            snprintf(err, sizeof(err), "ID måste vara 1–999 (du angav %d)", v);
        }
    } else {
        strcpy(err, "Ogiltig eller saknad parameter");
    }
    char dev2[16]; load_device_id(dev2, sizeof(dev2));
    char *p = build_main_page(err, dev2);
    httpd_resp_set_type(req, "text/html; charset=utf-8");
    return httpd_resp_send(req, p, HTTPD_RESP_USE_STRLEN);
}

void start_http_server(void) {
    ESP_ERROR_CHECK(nvs_flash_init());
    httpd_handle_t server = NULL;
    httpd_config_t cfg = HTTPD_DEFAULT_CONFIG();
    cfg.server_port = 80;
    ESP_ERROR_CHECK(httpd_start(&server, &cfg));
    httpd_register_uri_handler(server, &(httpd_uri_t){.uri="/",    .method=HTTP_GET,  .handler=html_get_handler});
    httpd_register_uri_handler(server, &(httpd_uri_t){.uri="/set", .method=HTTP_GET,  .handler=set_get_handler});
    PR_SERVER("HTTP-servern startad på port %d", cfg.server_port);
}