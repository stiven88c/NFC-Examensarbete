#include "http_client.h"
#include "esp_log.h"
#include "esp_http_client.h"
#include "esp_crt_bundle.h"
#include "esp_err.h"
#include <string.h>
#include <stdio.h>


static const char *TAG = "HTTP_CLIENT";

esp_err_t _http_event_handler(esp_http_client_event_t *evt) {
    switch (evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGI(TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            if (evt->data_len) {
                printf("%.*s\n", evt->data_len, (char *)evt->data);
            }
            break;
        default:
            break;
    }
    return ESP_OK;
}

esp_err_t create_tag_request(const char *base_url, const char *uid, const char *short_id) {
    char post_url[256];
    snprintf(post_url, sizeof(post_url), "%s/v1/tag", base_url); 

    esp_http_client_config_t config = {
        .url = post_url,
        .event_handler = _http_event_handler,
        .crt_bundle_attach = esp_crt_bundle_attach,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config); 

    esp_http_client_set_header(client, API_KEY_HEADER, API_KEY_VALUE); 
    esp_http_client_set_header(client, "Content-Type", "application/json"); 

    
   
    char post_data[256];
    snprintf(post_data, sizeof(post_data), "{\"tagTechnologyFamily\": \"Unknown\", \"uid\": \"%s\", \"shortId\": \"%s\"}", uid, short_id);

    esp_http_client_set_method(client, HTTP_METHOD_POST); 
    esp_http_client_set_post_field(client, post_data, strlen(post_data)); 

    esp_err_t err = esp_http_client_perform(client); 
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "HTTP POST Status = %d", esp_http_client_get_status_code(client)); 
    } else {
        ESP_LOGE(TAG, "HTTP POST request failed: %s", esp_err_to_name(err)); 
    }

    esp_http_client_cleanup(client); 
    return err; 
        ESP_LOGE(TAG, "HTTP POST request failed: %s", esp_err_to_name(err));
    }


esp_err_t block_tag_request(const char *base_url, const char *tag_technology_family, const char *uid, int *status_code) {
    char delete_url[256];
    snprintf(delete_url, sizeof(delete_url), "%s/v1/tag/%s/%s", base_url, tag_technology_family, uid);

    esp_http_client_config_t config = {
        .url = delete_url,
        .event_handler = _http_event_handler,
        .crt_bundle_attach = esp_crt_bundle_attach,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);

    esp_http_client_set_header(client, API_KEY_HEADER, API_KEY_VALUE);
    esp_http_client_set_header(client, "Content-Type", "application/json");

    
    char delete_data[256];
    snprintf(delete_data, sizeof(delete_data), "{\"reason\": \"GateExitScan\"}"); 

    esp_http_client_set_method(client, HTTP_METHOD_DELETE);
    esp_http_client_set_post_field(client, delete_data, strlen(delete_data));

    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        *status_code = esp_http_client_get_status_code(client); 
        ESP_LOGI(TAG, "HTTP DELETE Status = %d", *status_code);
    } else {
        ESP_LOGE(TAG, "HTTP DELETE request failed: %s", esp_err_to_name(err));
    }

    esp_http_client_cleanup(client);
    return err;
}

esp_err_t update_tag_request(const char *base_url, const char *tag_technology_family, const char *uid) {
    char patch_url[256];
    snprintf(patch_url, sizeof(patch_url), "%s/v1/tag/%s/%s", base_url, tag_technology_family, uid); 

    esp_http_client_config_t config = {
        .url = patch_url,
        .event_handler = _http_event_handler,
        .crt_bundle_attach = esp_crt_bundle_attach,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);

    esp_http_client_set_header(client, API_KEY_HEADER, API_KEY_VALUE);
    esp_http_client_set_header(client, "Content-Type", "application/json");

    char patch_data[256];
    snprintf(patch_data, sizeof(patch_data), "{\"action\": \"unblock\"}"); 

    esp_http_client_set_method(client, HTTP_METHOD_PATCH);
    esp_http_client_set_post_field(client, patch_data, strlen(patch_data));

    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "HTTP PATCH Status = %d", esp_http_client_get_status_code(client));
    } else {
        ESP_LOGE(TAG, "HTTP PATCH request failed: %s", esp_err_to_name(err));
    }

    esp_http_client_cleanup(client);
    return err;
}