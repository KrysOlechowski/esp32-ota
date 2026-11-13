#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <private_config.h>
#include "ota_service.h"

WebServer server(80);

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.setHostname(DEVICE_HOSTNAME);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);  // Bez połączenia z Wi-Fi OTA jest niedostępne.
  }
  OtaService::begin();
  server.on("/", HTTP_GET, []() {
    server.send(200, "text/plain; charset=utf-8",
                "SUKCES! Program wgrany z MacBooka przez OTA!\nUrzadzenie: " DEVICE_HOSTNAME);
  });
  server.begin();
}

void loop() {
  OtaService::handle();  // Nie usuwać; unikaj blokujących pętli i długiego delay().
  server.handleClient();
  delay(1);
}
