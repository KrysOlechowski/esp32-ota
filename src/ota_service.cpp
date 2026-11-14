#include <ArduinoOTA.h>
#include <private_config.h>
#include "ota_service.h"

void OtaService::begin(void (*beforeUpdate)()) {
  ArduinoOTA.setHostname(DEVICE_HOSTNAME);
  ArduinoOTA.setPassword(OTA_PASSWORD);
  if (beforeUpdate) ArduinoOTA.onStart(beforeUpdate);
  ArduinoOTA.begin();
}

void OtaService::handle() {
  ArduinoOTA.handle();
}
