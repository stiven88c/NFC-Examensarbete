#ifndef SENSOR_HANDLER_H
#define SENSOR_HANDLER_H

// Definiera GPIO-pinnar för magnetsensorerna
#define SENSOR_PIN_5 2  // Supernöjd
#define SENSOR_PIN_4 3  // Nöjd
#define SENSOR_PIN_3 6  // Neutral
#define SENSOR_PIN_2 7  // Lite besviken
#define SENSOR_PIN_1 14  // Mycket besviken

void init_sensors();  
void check_sensors(); 

#endif