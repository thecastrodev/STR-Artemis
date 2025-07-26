#include <WiFi.h>
#include "config.h"

void wifi_connect()
{
  IPAddress local_IP(STATIC_IP_ADDR);
  IPAddress gateway(GATEWAY_ADDR);
  IPAddress subnet(SUBNET_ADDR);

  Serial.print("Configurando IP estático...");
  if (!WiFi.config(local_IP, gateway, subnet))
  {
    Serial.println(" Falha!");
  }
  else
  {
    Serial.println(" Sucesso.");
  }

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD, WIFI_CHANNEL);
  Serial.print("Conectando ao Wi-Fi ");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(100);
    Serial.print(".");
  }
  Serial.println("\nConectado!");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());
}