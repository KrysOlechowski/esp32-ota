#include <ArduinoOTA.h>
#include <private_config.h>
#include "ota_service.h"

void OtaService::begin() {
  ArduinoOTA.setHostname(DEVICE_HOSTNAME);
  ArduinoOTA.setPassword(OTA_PASSWORD);
  ArduinoOTA.begin();
}

void OtaService::handle() {
  ArduinoOTA.handle();
}
