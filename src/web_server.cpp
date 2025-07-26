#include <Arduino.h>
#include <WebServer.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "config.h"
#include "dht_sensor.h"
#include "bmp_sensor.h"
#include "web_server.h"
#include "wind_sensor.h"

// Variáveis e objetos privados do módulo
static WebServer server(80);
static volatile bool g_ledState = false;

// Protótipos dos Handlers (privados)
static void handleRoot();
static void handleToggle();

// Implementação dos Handlers
static void handleRoot()
{
  float temp_local, hum_local, pressure_local;
  float wind_speed_local = 0.0;
  dht_sensor_get_latest_readings(temp_local, hum_local);
  bmp_sensor_get_latest_reading(pressure_local);
  wind_sensor_get_latest_reading(wind_speed_local);

  String html = R"(
<!DOCTYPE html>
<html lang="pt-BR">

<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <meta http-equiv='refresh' content='10'>
  <title>Artemis</title>
  <style>
    :root {
      --bg-color: #ffffff;
      --card-color: #f0f0f0;
      --text-color: #333333;
      --label-color: #666666;
      --border-radius: 8px;
    }

    body {
      font-family: sans-serif;
      background-color: var(--bg-color);
      color: var(--text-color);
      display: flex;
      justify-content: center;
      align-items: center;
      min-height: 100vh;
      margin: 0;
      padding: 1rem;
      box-sizing: border-box;
    }

    .container {
      width: 100%;
      max-width: 600px;
    }

    header {
      text-align: center;
      margin-bottom: 2rem;
    }

    header h1 {
      margin: 0;
      font-size: 2.5rem;
      font-weight: 700;
    }

    header p {
      margin: 0.25rem 0 0;
      font-size: 1rem;
      font-weight: 300;
      color: var(--label-color);
    }

    main {
      display: grid;
      grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
      gap: 1.5rem;
    }

    .card {
      background-color: var(--card-color);
      padding: 1.5rem;
      border-radius: var(--border-radius);
      text-align: center;
    }

    .label {
      display: block;
      font-size: 1rem;
      color: var(--label-color);
      margin-bottom: 0.5rem;
    }

    .value {
      font-size: 3rem;
      font-weight: 700;
      line-height: 1;
    }

    .unit {
      font-size: 1.5rem;
      font-weight: 400;
      margin-left: 0.25rem;
    }

    footer {
      margin-top: 2rem;
      text-align: center;
    }

    .status-box {
      background-color: var(--card-color);
      padding: 1rem 2rem;
      border-radius: var(--border-radius);
      display: inline-block;
      font-size: 1.25rem;
      font-weight: 400;
    }
  </style>
</head>
<body>
  <body>
  <div class="container">
    <header>
      <h1>Artemis</h1>
      <p>Sistema em Tempo Real para Monitoramento Meteorológico</p>
    </header>
    <main>
    <div class="card">
        <span class="label">Temperatura</span>
        <span class="data value">)";
  html += String(temp_local, 1);
  html += R"(<span class="unit">&deg;C</span></span>
        </div>
  )";
  html += R"(
      <div class="card">
        <span class="label">Umidade</span>
        <span class="data value">)";
  html += String(hum_local, 1);
  html += R"(<span class="unit">%</span></span>
      </div>
  )";
  html += R"(
      <div class="card">
        <span class="label">Pressão</span>
        <span class="data value">)";
  html += String(pressure_local / 100.0F, 1); // A biblioteca retorna Pa, dividimos por 100 para obter hPa (hectopascais)
  html += R"(<span class="unit">hPa</span></span>
      </div>
  )";
  html += R"(
      <div class="card">
        <span class="label">Velocidade do Vento</span>
        <span class="data value">)";
  html += String(wind_speed_local, 1);
  html += R"(<span class="unit">km/h</span></span>
      </div>
  )";
  html += R"(</a></main>
    </div>
  </body>
  </html>)";

  // <footer>
  //   <div class = "status-box"> % STATUS %
  //   </ div>
  // </ footer>

  server.send(200, "text/html", html);
}

static void handleToggle()
{
  g_ledState = !g_ledState;
  digitalWrite(LED_PIN, g_ledState);
  Serial.printf("LED state alterado para: %s\n", g_ledState ? "ON" : "OFF");

  server.sendHeader("Location", "/", true);
  server.send(302, "text/plain", "");
}

// Task do servidor web
static void vWebServerTask(void *pvParameters)
{
  server.begin();
  Serial.println("Servidor HTTP iniciado.");

  for (;;)
  {
    server.handleClient();
    vTaskDelay(pdMS_TO_TICKS(5));
  }
}

// Funções públicas
void web_server_setup()
{
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, g_ledState);

  server.on("/", HTTP_GET, handleRoot);
  server.on("/toggle", HTTP_GET, handleToggle);
}

void web_server_create_task()
{
  xTaskCreatePinnedToCore(
      vWebServerTask,
      "WebServerTask",
      WEBSERVER_TASK_STACK,
      NULL,
      WEBSERVER_TASK_PRIORITY,
      NULL,
      WEBSERVER_TASK_CORE);
}