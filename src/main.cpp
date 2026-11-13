#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <private_config.h>
#include "ota_service.h"

WebServer server(80);

namespace {
constexpr uint8_t LED_PIN = 23;  // L-18; zewnętrzna LED przez rezystor do GND.
bool ledOn = false;

void sendPage() {
  String page = F(
      "<!doctype html><html lang='pl'><head><meta charset='utf-8'>"
      "<meta name='viewport' content='width=device-width,initial-scale=1'>"
      "<title>Niebieska dioda</title><style>"
      "body{font-family:system-ui,sans-serif;background:#eef4ff;color:#14233d;"
      "margin:0;padding:24px}main{max-width:420px;margin:8vh auto;background:white;"
      "padding:28px;border-radius:20px}h1{font-size:1.8rem}"
      ".led{width:48px;height:48px;border-radius:50%;background:#9ba7b8}"
      ".on{background:#1683ff;box-shadow:0 0 24px #1683ff}"
      "form{margin:12px 0}button{width:100%;padding:16px;border:0;border-radius:12px;"
      "font:inherit;font-weight:600;background:#095bc4;color:white;cursor:pointer}"
      "button.off{background:#e6ebf2;color:#14233d}small{color:#52627a}"
      "</style></head><body><main><h1>Niebieska dioda</h1><p>Urzadzenie: ");
  page += DEVICE_HOSTNAME;
  page += F("</p><div role='img' aria-label='Dioda' class='led");
  if (ledOn) page += F(" on");
  page += F("'></div><p>Stan: <strong>");
  page += ledOn ? F("WŁĄCZONA") : F("WYŁĄCZONA");
  page += F("</strong></p><form method='post' action='/led/on'>"
            "<button>Włącz diodę</button></form>"
            "<form method='post' action='/led/off'>"
            "<button class='off'>Wyłącz diodę</button></form>"
            "<p><a href='/'>Odśwież stan</a></p>"
            "<small>Po restarcie dioda jest wyłączona.</small></main></body></html>");
  server.sendHeader("Cache-Control", "no-store");
  server.send(200, "text/html; charset=utf-8", page);
}

void setLed(bool on) {
  ledOn = on;
  digitalWrite(LED_PIN, on ? HIGH : LOW);
  // Po POST wracamy przez GET: odświeżenie strony nie powtarza polecenia.
  server.sendHeader("Location", "/");
  server.sendHeader("Cache-Control", "no-store");
  server.send(303, "text/plain", "");
}
}  // namespace

void setup() {
  Serial.begin(115200);
  digitalWrite(LED_PIN, LOW);
  pinMode(LED_PIN, OUTPUT);
  WiFi.mode(WIFI_STA);
  WiFi.setHostname(DEVICE_HOSTNAME);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);  // Bez połączenia z Wi-Fi OTA jest niedostępne.
  }
  OtaService::begin();
  server.on("/", HTTP_GET, sendPage);
  server.on("/led/on", HTTP_POST, []() { setLed(true); });
  server.on("/led/off", HTTP_POST, []() { setLed(false); });
  server.begin();
}

void loop() {
  OtaService::handle();  // Nie usuwać; unikaj blokujących pętli i długiego delay().
  server.handleClient();
  delay(1);
}
