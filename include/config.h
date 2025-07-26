#ifndef CONFIG_H
#define CONFIG_H

// --- Configurações de Rede ---
#define WIFI_SSID "Arthemis"
#define WIFI_PASSWORD "12345678"
#define WIFI_CHANNEL 1

// --- Configurações de IP Estático ---
#define STATIC_IP_ADDR {192, 168, 82, 82}
#define GATEWAY_ADDR {192, 168, 82, 70}
#define SUBNET_ADDR {255, 255, 255, 0}

// --- Configurações de Pinos e Sensor ---
#define DHT_PIN 15
#define DHT_TYPE DHT11
#define LED_PIN 2

// --- Configuração do Sensor de Vento ---
#define WIND_SENSOR_PIN 23                // Pino GPIO para o sensor de vento (ex: 23). NÃO pode ser pino only-input.
#define WIND_SENSOR_CALC_INTERVAL_MS 2000 // Intervalo em ms para calcular a velocidade (e.g., 5 segundos).

// --- Configuração das Tarefas do FreeRTOS ---
#define DHT_SENSOR_TASK_STACK 2048
#define DHT_SENSOR_TASK_PRIORITY 1
#define DHT_SENSOR_TASK_CORE 1

#define BMP_SENSOR_TASK_STACK 2048
#define BMP_SENSOR_TASK_PRIORITY 1
#define BMP_SENSOR_TASK_CORE 1

#define WIND_SENSOR_TASK_STACK 2048
#define WIND_SENSOR_TASK_PRIORITY 1
#define WIND_SENSOR_TASK_CORE 1

#define WEBSERVER_TASK_STACK 4096
#define WEBSERVER_TASK_PRIORITY 2
#define WEBSERVER_TASK_CORE 0

#endif // CONFIG_H