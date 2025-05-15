#include "sensor_handler.h"
#include "driver/gpio.h"
#include "cJSON.h"
#include <stdio.h>
#include "esp_spiffs.h"
#include "freertos/FreeRTOS.h"



void init_sensors() {
    gpio_reset_pin(SENSOR_PIN_5);
    gpio_set_direction(SENSOR_PIN_5, GPIO_MODE_INPUT);
    gpio_set_pull_mode(SENSOR_PIN_5, GPIO_PULLUP_ONLY);
    
    gpio_reset_pin(SENSOR_PIN_4);
    gpio_set_direction(SENSOR_PIN_4, GPIO_MODE_INPUT);
    gpio_set_pull_mode(SENSOR_PIN_4, GPIO_PULLUP_ONLY);
    
    gpio_reset_pin(SENSOR_PIN_3);
    gpio_set_direction(SENSOR_PIN_3, GPIO_MODE_INPUT);
    gpio_set_pull_mode(SENSOR_PIN_3, GPIO_PULLUP_ONLY);
    
    gpio_reset_pin(SENSOR_PIN_2);
    gpio_set_direction(SENSOR_PIN_2, GPIO_MODE_INPUT);
    gpio_set_pull_mode(SENSOR_PIN_2, GPIO_PULLUP_ONLY);
    
    gpio_reset_pin(SENSOR_PIN_1);
    gpio_set_direction(SENSOR_PIN_1, GPIO_MODE_INPUT);
    gpio_set_pull_mode(SENSOR_PIN_1, GPIO_PULLUP_ONLY);
}


void print_feedback(int rating) {
    switch (rating) {
        case 5:
            printf("Besökaren var supernöjd (siffra 5)\n");
            break;
        case 4:
            printf("Besökaren var nöjd (siffra 4)\n");
            break;
        case 3:
            printf("Besökaren var neutral (siffra 3)\n");
            break;
        case 2:
            printf("Besökaren var lite besviken (siffra 2)\n");
            break;
        case 1:
            printf("Besökaren var mycket besviken (siffra 1)\n");
            break;
        default:
            printf("Okänd feedback\n");
    }
}


void check_sensors() {
    static int previous_sensor_status_5 = 1;
    static int previous_sensor_status_4 = 1;
    static int previous_sensor_status_3 = 1;
    static int previous_sensor_status_2 = 1;
    static int previous_sensor_status_1 = 1;

    int sensor_status_5 = gpio_get_level(SENSOR_PIN_5);
    int sensor_status_4 = gpio_get_level(SENSOR_PIN_4);
    int sensor_status_3 = gpio_get_level(SENSOR_PIN_3);
    int sensor_status_2 = gpio_get_level(SENSOR_PIN_2);
    int sensor_status_1 = gpio_get_level(SENSOR_PIN_1);

    if (sensor_status_5 == 0 && previous_sensor_status_5 == 1) {
        print_feedback(5);
    }
    if (sensor_status_4 == 0 && previous_sensor_status_4 == 1) {
        print_feedback(4);
    }
    if (sensor_status_3 == 0 && previous_sensor_status_3 == 1) {
        print_feedback(3);
    }
    if (sensor_status_2 == 0 && previous_sensor_status_2 == 1) {
        print_feedback(2);
    }
    if (sensor_status_1 == 0 && previous_sensor_status_1 == 1) {
        print_feedback(1);
    }

    previous_sensor_status_5 = sensor_status_5;
    previous_sensor_status_4 = sensor_status_4;
    previous_sensor_status_3 = sensor_status_3;
    previous_sensor_status_2 = sensor_status_2;
    previous_sensor_status_1 = sensor_status_1;
}