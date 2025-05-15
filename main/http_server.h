#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

void start_http_server();
void show_message(const char *msg, int duration_ms);
void task_show_message(void *pvParameters);

#endif // HTTP_SERVER_H