#ifndef DHT_SENSOR_H
#define DHT_SENSOR_H

void dht_sensor_setup();
void dht_sensor_create_task();
void dht_sensor_get_latest_readings(float &temperature, float &humidity);

#endif // DHT_SENSOR_H