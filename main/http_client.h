#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#include "esp_err.h"

#define API_URL "https://prs-cdp-dev-shopservice-webapp.azurewebsites.net"
#define API_KEY_HEADER "x-apikey"
#define API_KEY_VALUE "dpytEp8LkUgqu0r5ZPpQpo9iO4h8csty"



esp_err_t create_tag_request(const char *base_url, const char *uid, const char *short_id);
esp_err_t block_tag_request(const char *base_url, const char *tag_technology_family, const char *uid, int *status_code);
esp_err_t update_tag_request(const char *base_url, const char *tag_technology_family, const char *uid);


#endif 