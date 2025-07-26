#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "wifi_manager.h"
#include "dht_sensor.h"
#include "bmp_sensor.h"
#include "wind_sensor.h"
#include "web_server.h"

void setup()
{
  Serial.begin(115200);

  // 1. Inicializa o sensor
  dht_sensor_setup();
  bmp_sensor_setup();
  wind_sensor_setup();

  // 2. Inicializa e conecta ao Wi-Fi
  wifi_connect();

  // 3. Configura o servidor web (handlers e pinos)
  web_server_setup();

  // 4. Cria as tarefas do FreeRTOS
  dht_sensor_create_task();
  bmp_sensor_create_task();
  wind_sensor_create_task();
  web_server_create_task();

  Serial.println("Sistema inicializado. Tarefas do FreeRTOS em execução.");
}

void loop()
{
  // A tarefa do loop do Arduino é deletada para economizar recursos,
  // já que o FreeRTOS gerencia a execução.
  vTaskDelete(NULL);
}