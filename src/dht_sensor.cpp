#include "dht_sensor.h"
#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "DHT.h"
#include "config.h"

// Variáveis e objetos privados do módulo
static DHT dht(DHT_PIN, DHT_TYPE);
static volatile float g_temperature = -99.9;
static volatile float g_humidity = -99.9;
static portMUX_TYPE sensorMutex = portMUX_INITIALIZER_UNLOCKED;

// Task de leitura do sensor
static void vReadSensorTask(void *pvParameters)
{
  TickType_t xLastWakeTime = xTaskGetTickCount();
  const TickType_t xFrequency = pdMS_TO_TICKS(2000);

  for (;;)
  {
    vTaskDelayUntil(&xLastWakeTime, xFrequency);

    float h = dht.readHumidity();
    float t = dht.readTemperature();

    if (!isnan(h) && !isnan(t))
    {
      portENTER_CRITICAL(&sensorMutex);
      g_temperature = t;
      g_humidity = h;
      portEXIT_CRITICAL(&sensorMutex);
    }
    else
    {
      Serial.println("Falha ao ler do sensor DHT!");
    }
  }
}

// Funções públicas
void dht_sensor_setup()
{
  pinMode(DHT_PIN, INPUT_PULLUP);
  dht.begin();
  Serial.println("Sensor DHT inicializado.");
}

void dht_sensor_create_task()
{
  xTaskCreatePinnedToCore(
      vReadSensorTask,
      "SensorReadTask",
      DHT_SENSOR_TASK_STACK,
      NULL,
      DHT_SENSOR_TASK_PRIORITY,
      NULL,
      DHT_SENSOR_TASK_CORE);
}

void dht_sensor_get_latest_readings(float &temperature, float &humidity)
{
  portENTER_CRITICAL(&sensorMutex);
  temperature = g_temperature;
  humidity = g_humidity;
  portEXIT_CRITICAL(&sensorMutex);
}