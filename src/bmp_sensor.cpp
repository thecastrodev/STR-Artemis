
#include "bmp_sensor.h"
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_BMP085.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "config.h"

// Variáveis e objetos privados do módulo
static Adafruit_BMP085 bmp;
static volatile float g_pressure = -999.0;
static portMUX_TYPE bmpMutex = portMUX_INITIALIZER_UNLOCKED;

// Task de leitura do sensor BMP180
static void vReadBmpTask(void *pvParameters)
{
  TickType_t xLastWakeTime = xTaskGetTickCount();
  // Período de leitura (ex: 5 segundos)
  const TickType_t xFrequency = pdMS_TO_TICKS(2000);

  for (;;)
  {
    vTaskDelayUntil(&xLastWakeTime, xFrequency);

    float p = bmp.readPressure();

    // A biblioteca retorna 0 em caso de falha de leitura
    if (p > 0)
    {
      portENTER_CRITICAL(&bmpMutex);
      g_pressure = p;
      portEXIT_CRITICAL(&bmpMutex);
    }
    else
    {
      Serial.println("Falha ao ler do sensor BMP180!");
    }
  }
}

// Funções públicas
void bmp_sensor_setup()
{
  // O BMP180 usa I2C. A biblioteca Adafruit_BMP085 chama Wire.begin() internamente.
  if (!bmp.begin())
  {
    Serial.println("Não foi possível encontrar um sensor BMP180 válido, verifique a fiação!");
    // Trava a execução se o sensor for crítico e não for encontrado.
    while (1)
    {
      vTaskDelay(1);
    }
  }
  else
  {
    Serial.println("Sensor BMP180 inicializado.");
  }
}

void bmp_sensor_create_task()
{
  xTaskCreatePinnedToCore(
      vReadBmpTask,
      "BmpReadTask",
      BMP_SENSOR_TASK_STACK,
      NULL,
      BMP_SENSOR_TASK_PRIORITY,
      NULL,
      BMP_SENSOR_TASK_CORE);
}

void bmp_sensor_get_latest_reading(float &pressure)
{
  portENTER_CRITICAL(&bmpMutex);
  pressure = g_pressure;
  portEXIT_CRITICAL(&bmpMutex);
}