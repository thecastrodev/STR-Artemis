#ifndef BMP_SENSOR_H
#define BMP_SENSOR_H

void bmp_sensor_setup();
void bmp_sensor_create_task();
void bmp_sensor_get_latest_reading(float &pressure);

#endif // BMP_SENSOR_H