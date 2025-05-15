#include "esp_log.h"
#include "esp_http_server.h"
#include "http_server.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "HTTP_SERVER";
static char message[50] = "Hello, ESP32!";
static bool message_active = false;
static char last_message[50] = ""; // För att spara det senaste meddelandet

const char *html_content = "<!DOCTYPE html>"
                            "<html lang='sv'>"
                            "<head>"
                            "<meta charset='UTF-8'>"
                            "<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
                            "<title>ESP32 Meddelande</title>"
                            "<script>"
                            "function fetchMessage() {"
                            "   fetch('/message').then(response => response.json()).then(data => {"
                            "       const messageElement = document.getElementById('message');"
                            "       if (data.message !== messageElement.innerText) {"
                            "           messageElement.innerText = data.message;"
                            "       }"
                            "   }).catch(error => console.error('Error fetching message:', error));"
                            "} "
                            "setInterval(fetchMessage, 1000);"
                            "</script>"
                            "</head>"
                            "<body>"
                            "<h1>Loftet, där vissioner blir till verklighet</h1>"
                            "<p id='message'>Laddar...</p>"
                            "</body>"
                            "</html>";

// HTTP GET handler
esp_err_t get_handler(httpd_req_t *req) {
    // Sätt Content-Type header för JSON
    httpd_resp_set_type(req, "application/json");

    // Skapa ett JSON-svar
    char json_response[100];
    snprintf(json_response, sizeof(json_response), "{\"message\": \"%s\"}", last_message);

    // Skicka svaret
    esp_err_t ret = httpd_resp_send(req, json_response, HTTPD_RESP_USE_STRLEN);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to send response");
    }

    return ret; // Returnera statusen av svaret
}

// HTTP GET handler för HTML-sidan
esp_err_t html_get_handler(httpd_req_t *req) {
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, html_content, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

// Visa meddelande
void show_message(const char *msg, int duration_ms) {
    if (message_active) {
        return; // Om ett meddelande visas, gör inget
    }

    strncpy(message, msg, sizeof(message) - 1);
    message[sizeof(message) - 1] = '\0'; // Säkerställ nullterminering

    // Spara det senaste meddelandet
    strncpy(last_message, message, sizeof(last_message) - 1);
    last_message[sizeof(last_message) - 1] = '\0'; // Säkerställ nullterminering

    message_active = true;

    xTaskCreate((void (*)(void *)) task_show_message, "ShowMessage", 2048, (void *)duration_ms, 5, NULL);
}

void task_show_message(void *pvParameters) {
    int duration_ms = (int)pvParameters;
    ESP_LOGI(TAG, "%s", message);

    vTaskDelay(duration_ms / portTICK_PERIOD_MS);
    message_active = false; // Meddelandet är inte längre aktivt
    ESP_LOGI(TAG, "Message cleared");

    vTaskDelete(NULL);
}

// Starta HTTP-servern
void start_http_server() {
    httpd_handle_t server = NULL;
    
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.server_port = 80; // Standard HTTP port

    // Start server
    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_uri_t uri_html = {
            .uri = "/",
            .method = HTTP_GET,
            .handler = html_get_handler,
            .user_ctx = NULL
        };

        httpd_uri_t uri_message = {
            .uri = "/message",
            .method = HTTP_GET,
            .handler = get_handler,
            .user_ctx = NULL
        };

        // Registrera HTML URI och meddelande URI
        httpd_register_uri_handler(server, &uri_html);
        httpd_register_uri_handler(server, &uri_message);
    }
}