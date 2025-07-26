#ifndef WIND_SENSOR_H
#define WIND_SENSOR_H

#include <Arduino.h>

void wind_sensor_setup();
void wind_sensor_create_task();
bool wind_sensor_get_latest_reading(float &speed);

#endif // WIND_SENSOR_H