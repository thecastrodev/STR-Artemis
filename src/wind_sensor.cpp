#include "wind_sensor.h"
#include "config.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// --- Variáveis e objetos privados do módulo ---

// Mutex para proteger o acesso às variáveis compartilhadas entre a ISR e a tarefa.
static portMUX_TYPE wind_sensor_mutex = portMUX_INITIALIZER_UNLOCKED;

// Variáveis voláteis modificadas pela ISR
static volatile uint32_t g_revolution_count = 0;
static volatile unsigned long g_last_interrupt_time = 0;

// Variável para armazenar o resultado final
static volatile float g_wind_speed_kmh = 0.0;
static volatile bool g_valid_reading = false;

// --- Rotina de Serviço de Interrupção (ISR) ---

static void IRAM_ATTR wind_sensor_isr()
{
  unsigned long current_time = millis();
  // Debounce simples: ignora interrupções muito próximas (menos de 20ms)
  if (current_time - g_last_interrupt_time > 20)
  {
    portENTER_CRITICAL_ISR(&wind_sensor_mutex);
    g_revolution_count++;
    g_last_interrupt_time = current_time;
    portEXIT_CRITICAL_ISR(&wind_sensor_mutex);
  }
}

// --- Tarefa de Leitura e Cálculo do FreeRTOS ---

static void vCalculateWindSpeedTask(void *pvParameters)
{
  const TickType_t xFrequency = pdMS_TO_TICKS(WIND_SENSOR_CALC_INTERVAL_MS);
  TickType_t xLastWakeTime = xTaskGetTickCount();
  uint32_t revolution_count_local;

  // Fator de conversão com base em anemômetros comuns onde 2.5 mph = 1 Hz
  // 2.5 mph = 4.02336 km/h. Este valor deve ser calibrado para o seu sensor.
  const float HZ_TO_KMH_FACTOR = 4.02336;

  for (;;)
  {
    vTaskDelayUntil(&xLastWakeTime, xFrequency);

    portENTER_CRITICAL(&wind_sensor_mutex);
    revolution_count_local = g_revolution_count;
    g_revolution_count = 0; // Reseta o contador para o próximo ciclo
    portEXIT_CRITICAL(&wind_sensor_mutex);

    // Calcula a frequência em Hz (revoluções por segundo)
    float frequency = (float)revolution_count_local / (WIND_SENSOR_CALC_INTERVAL_MS / 1000.0f);

    // Converte frequência para km/h
    float current_wind_speed = frequency * HZ_TO_KMH_FACTOR;

    portENTER_CRITICAL(&wind_sensor_mutex);
    g_wind_speed_kmh = current_wind_speed;
    g_valid_reading = true;
    portEXIT_CRITICAL(&wind_sensor_mutex);

#if CORE_DEBUG_LEVEL == 5
    Serial.printf("Revoluções no intervalo: %u, Velocidade do Vento: %.2f km/h\n", revolution_count_local, current_wind_speed);
#endif
  }
}

// --- Funções Públicas ---

void wind_sensor_setup()
{
  pinMode(WIND_SENSOR_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(WIND_SENSOR_PIN), wind_sensor_isr, FALLING);
  Serial.println("Sensor de Vento inicializado.");
}

void wind_sensor_create_task()
{
  xTaskCreatePinnedToCore(
      vCalculateWindSpeedTask,
      "WindSpeedCalcTask",
      WIND_SENSOR_TASK_STACK,
      NULL,
      WIND_SENSOR_TASK_PRIORITY,
      NULL,
      WIND_SENSOR_TASK_CORE);
}

bool wind_sensor_get_latest_reading(float &speed)
{
  portENTER_CRITICAL(&wind_sensor_mutex);
  speed = g_wind_speed_kmh;
  bool valid = g_valid_reading;
  portEXIT_CRITICAL(&wind_sensor_mutex);
  return valid;
}