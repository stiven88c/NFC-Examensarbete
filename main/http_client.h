#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#include "esp_err.h"


void http_client_init_sntp(void);

esp_err_t log_event_request(const char *script_url, const char *uid);


esp_err_t register_device_request(const char *script_url,
                                  const char *mac,
                                  const char *device_id,
                                  const char *url,
                                  const char *desc);


esp_err_t name_tag_request(const char *script_url,
                           const char *uid,
                           const char *name);

#endif // HTTP_CLIENT_H
