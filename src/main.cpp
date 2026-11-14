#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h>
#include <Preferences.h>
#include <private_config.h>
#include "ota_service.h"
#include "climate_color.h"
#include "web_page.h"
#include "stepper_motion.h"
#include "servo_motion.h"

WebServer server(80);
namespace {
// Plan sygnałów. Montaż modułów wymaga potwierdzenia pinoutu egzemplarzy.
constexpr uint8_t RGB_PINS[] = {25, 26, 27};  // P-11, P-10, P-09
constexpr uint8_t DHT_PIN = 18;                // L-11
constexpr uint8_t STEPPER_PINS[] = {19, 21, 22, 23};  // IN1..IN4, L-12/14/17/18
constexpr uint8_t SERVO_PIN = 16; // L-08, osobny timer LEDC od RGB
constexpr uint8_t SERVO_CHANNEL = 6;
ServoMotion servo;
bool servoAttached = false, servoOK = true;
Preferences servoPrefs;
bool servoStorageOK = false, servoZeroKnown = false, servoHasCommand = false;
uint8_t servoZero = 90;
StepperMotion motor;
uint8_t motorMask = 0;
constexpr bool COMMON_ANODE = false;          // typowy KY-016: wspólna katoda
constexpr uint32_t SAMPLE_MS = 2500;
DHT dht(DHT_PIN, DHT11);
ClimateColor::Range range;
ClimateColor::RGB manual = {0, 100, 255}, applied = {0, 0, 0};
bool automatic = false, enabled = false, sensorOK = false, pwmOK = false;
uint8_t brightness = 60;
uint8_t manualSaturation = 100;
float temperature = NAN, humidity = NAN;
uint32_t lastAttempt = 0, lastSuccess = 0;
bool hasReading = false;

void writeMotor() {
  if (motorMask == motor.mask()) return;
  motorMask = motor.mask();
  for (uint8_t i = 0; i < 4; ++i) digitalWrite(STEPPER_PINS[i], (motorMask >> i) & 1);
}
void stopMotor() { motor.stop(); writeMotor(); }
void releaseServo() {
  servo.release();
  if (servoAttached) ledcDetach(SERVO_PIN);
  servoAttached = false;
  digitalWrite(SERVO_PIN, LOW); pinMode(SERVO_PIN, OUTPUT);
}
void stopDrives() { stopMotor(); releaseServo(); }
void handleServo() {
  if (WiFi.status() != WL_CONNECTED) { releaseServo(); return; }
  servo.tick(millis());
  if (!servo.active()) { if (servoAttached) releaseServo(); return; }
  if (!servoAttached) {
    servoAttached = ledcAttachChannel(SERVO_PIN, 50, 16, SERVO_CHANNEL);
    servoOK = servoAttached;
  }
  if (!servoAttached || !ledcWrite(SERVO_PIN, servo.duty())) {
    servoOK = false; releaseServo();
  }
}
void handleMotor() {
  if (WiFi.status() != WL_CONNECTED) motor.stop();
  motor.tick(millis());
  writeMotor();
}

String hexColor(ClimateColor::RGB c) {
  char s[8];
  snprintf(s, sizeof(s), "#%02x%02x%02x", c.r, c.g, c.b);
  return String(s);
}

void applyColor() {
  applied = {0, 0, 0};
  if (!pwmOK) return;
  if (enabled && (!automatic || sensorOK)) {
    applied = automatic ? ClimateColor::fromReading(temperature, humidity, range)
                        : ClimateColor::withSaturation(manual, manualSaturation);
  }
  const uint8_t channels[] = {applied.r, applied.g, applied.b};
  for (int i = 0; i < 3; ++i) {
    // Korekcja gamma ułatwia wybór ciemnych odcieni suwakiem.
    uint8_t duty = lroundf(powf(channels[i] / 255.0f * brightness / 100.0f, 2.2f) * 255);
    if (COMMON_ANODE) duty = 255 - duty;
    if (!ledcWrite(RGB_PINS[i], duty)) {
      pwmOK = false;
      applied = {0, 0, 0};
      for (uint8_t pin : RGB_PINS) {
        ledcDetach(pin);
        digitalWrite(pin, COMMON_ANODE ? HIGH : LOW);
        pinMode(pin, OUTPUT);
      }
      break;
    }
  }
}

void sampleSensor() {
  uint32_t now = millis();
  if (uint32_t(now - lastAttempt) < SAMPLE_MS) return;
  lastAttempt = now;
  // Biblioteka DHT11 zajmuje ok. 20–30 ms; brak długiego oczekiwania w loop.
  float h = dht.readHumidity(), t = dht.readTemperature();
  sensorOK = isfinite(t) && isfinite(h) && t >= 0 && t <= 50 && h >= 0 && h <= 100;
  if (sensorOK) {
    temperature = t; humidity = h; hasReading = true; lastSuccess = millis();
  }
  if (automatic) applyColor();  // Przy błędzie: zgaś RGB, zachowaj ostatni odczyt na stronie.
}

void sendState() {
  String json;
  json.reserve(1000);
  json = "{\"device\":\"" DEVICE_HOSTNAME "\",\"mode\":\"";
  json += automatic ? "auto" : "manual";
  json += "\",\"enabled\":"; json += enabled ? "true" : "false";
  json += ",\"sensorOK\":"; json += sensorOK ? "true" : "false";
  json += ",\"pwmOK\":"; json += pwmOK ? "true" : "false";
  json += ",\"temperature\":"; json += hasReading ? String(temperature, 1) : String("null");
  json += ",\"humidity\":"; json += hasReading ? String(humidity, 1) : String("null");
  json += ",\"ageSeconds\":"; json += hasReading ? String(uint32_t(millis() - lastSuccess) / 1000) : String("null");
  json += ",\"color\":\"" + hexColor(applied) + "\",\"manualColor\":\"" + hexColor(manual) + "\"";
  json += ",\"brightness\":" + String(brightness);
  json += ",\"manualSaturation\":" + String(manualSaturation);
  json += ",\"servoActive\":"; json += servo.active() ? "true" : "false";
  json += ",\"servoOK\":"; json += servoOK ? "true" : "false";
  json += ",\"servoAngle\":" + String(servo.angle());
  json += ",\"servoTarget\":" + String(servo.target());
  json += ",\"servoZeroKnown\":"; json += servoZeroKnown ? "true" : "false";
  json += ",\"servoZero\":" + String(servoZero);
  json += ",\"servoHasCommand\":"; json += servoHasCommand ? "true" : "false";
  json += ",\"motorRunning\":"; json += motor.running() ? "true" : "false";
  json += ",\"motorDirection\":" + String(motor.direction());
  json += ",\"motorZeroKnown\":"; json += motor.zeroKnown() ? "true" : "false";
  json += ",\"motorAngle\":" + String(motor.position() * 360.0f / StepperMotion::STEPS_PER_TURN, 1);
  json += ",\"motorDiagnostic\":"; json += motor.diagnostic() ? "true" : "false";
  json += ",\"cold\":" + String(range.cold, 1) + ",\"warm\":" + String(range.warm, 1);
  json += ",\"dry\":" + String(range.dry, 1) + ",\"wet\":" + String(range.wet, 1) + "}";
  server.sendHeader("Cache-Control", "no-store");
  server.send(200, "application/json; charset=utf-8", json);
}

bool numberArg(const char *name, float low, float high, float &out) {
  if (!server.hasArg(name)) return false;
  String value = server.arg(name);
  if (value.isEmpty() || value.length() > 8) return false;
  for (char c : value) if ((c < '0' || c > '9') && c != '.' && c != '-') return false;
  char *end;
  out = strtof(value.c_str(), &end);
  return end != value.c_str() && *end == '\0' && isfinite(out) && out >= low && out <= high;
}
void badRequest() { server.send(400, "text/plain; charset=utf-8", "Nieprawidłowe ustawienia."); }
void controlMotor() {
  const String action = server.arg("action");
  const bool move = action == "left" || action == "right";
  float degrees = 128.0f * 360 / StepperMotion::STEPS_PER_TURN;
  float speed = 100;
  const bool hasSpeed = server.hasArg("speed");
  if (hasSpeed && (!numberArg("speed", 50, 200, speed) ||
      (speed != 50 && speed != 100 && speed != 200))) { badRequest(); return; }
  if ((move && (server.args() != 1 + int(server.hasArg("degrees")) + int(hasSpeed) ||
      (server.hasArg("degrees") && !numberArg("degrees", 1, 360, degrees)))) ||
      (!move && (server.args() != 1 + int(action == "home" && hasSpeed) ||
       (action != "stop" && action != "test" && action != "zero" && action != "home")))) {
    badRequest(); return;
  }
  if (action == "stop") stopMotor();
  else if (motor.running() || servo.active()) {
    server.send(409, "text/plain; charset=utf-8", "Poczekaj na zakończenie pracy aktywnego napędu lub wyłącz jego sygnał."); return;
  } else if (action == "test") motor.startDiagnostic(millis());
  else if (action == "zero") motor.setZero();
  else if (action == "home") {
    if (!motor.zeroKnown()) { badRequest(); return; }
    motor.home(millis(), uint16_t(speed));
  } else motor.start(action == "left" ? -1 : 1, millis(),
                    uint16_t(lroundf(degrees * StepperMotion::STEPS_PER_TURN / 360)), uint16_t(speed));
  sendState();
}
void controlServo() {
  const String action = server.arg("action");
  if (action == "release" && server.args() == 1) { releaseServo(); sendState(); return; }
  if (action == "zero" && server.args() == 1) {
    if (motor.running() || servo.active() || !servoHasCommand) {
      server.send(409, "text/plain; charset=utf-8", "Najpierw ustaw kąt i poczekaj na zakończenie ruchu."); return;
    }
    const uint8_t next = servo.angle();
    if (!servoStorageOK || ((!servoZeroKnown || next != servoZero) &&
        servoPrefs.putUChar("zero", next) != sizeof(uint8_t))) {
      server.send(500, "text/plain; charset=utf-8", "Nie zapisano zera w pamięci ESP32."); return;
    }
    servoZero = next; servoZeroKnown = true; sendState(); return;
  }
  float speed = 50, angle = servoZero;
  if (server.hasArg("speed") && (!numberArg("speed", 25, 100, speed) ||
      (speed != 25 && speed != 50 && speed != 100))) { badRequest(); return; }
  if ((action == "move" && (server.args() != 2 + int(server.hasArg("speed")) ||
       !numberArg("angle", 45, 135, angle) || floorf(angle) != angle)) ||
      (action == "home" && (server.args() != 1 + int(server.hasArg("speed")) || !servoZeroKnown)) ||
      (action != "move" && action != "home")) { badRequest(); return; }
  if (motor.running()) {
    server.send(409, "text/plain; charset=utf-8", "Najpierw zatrzymaj krokowiec."); return;
  }
  servo.move(int(angle), millis(), uint16_t(speed)); servoHasCommand = true;
  handleServo(); sendState();
}
void control() {
  String mode = server.arg("mode");
  if (mode == "off" && server.args() == 1) { enabled = false; applyColor(); sendState(); return; }
  float level;
  if (!numberArg("brightness", 0, 100, level) || floorf(level) != level) { badRequest(); return; }
  ClimateColor::RGB next = manual;
  float saturation = manualSaturation;
  if (mode == "manual" && server.args() == (server.hasArg("saturation") ? 4 : 3)) {
    if (server.hasArg("saturation") &&
        (!numberArg("saturation", 0, 100, saturation) || floorf(saturation) != saturation)) {
      badRequest(); return;
    }
    String color = server.arg("color");
    if (color.length() != 7 || color[0] != '#') { badRequest(); return; }
    for (int i = 1; i < 7; ++i) if (!isxdigit(static_cast<unsigned char>(color[i]))) { badRequest(); return; }
    unsigned long value = strtoul(color.c_str() + 1, nullptr, 16);
    next = {uint8_t(value >> 16), uint8_t(value >> 8), uint8_t(value)};
  } else if (mode != "auto" || server.args() != 2) { badRequest(); return; }
  manual = next; manualSaturation = uint8_t(saturation);
  brightness = uint8_t(level); automatic = mode == "auto"; enabled = true;
  applyColor(); sendState();
}
void configureRange() {
  ClimateColor::Range next;
  if (server.args() != 4 || !numberArg("cold", 0, 49, next.cold) ||
      !numberArg("warm", 1, 50, next.warm) || !numberArg("dry", 0, 99, next.dry) ||
      !numberArg("wet", 1, 100, next.wet) || next.warm - next.cold < 1 || next.wet - next.dry < 1) {
    badRequest(); return;
  }
  range = next; applyColor(); sendState();
}
}

void setup() {
  Serial.begin(115200);
  servoStorageOK = servoPrefs.begin("servo", false);
  if (servoStorageOK && servoPrefs.isKey("zero")) {
    servoZero = servoPrefs.getUChar("zero", 90);
    servoZeroKnown = servoZero >= 45 && servoZero <= 135;
    if (!servoZeroKnown) servoZero = 90;
  }
  releaseServo(); // po restarcie brak impulsów i automatycznego ruchu
  for (uint8_t pin : STEPPER_PINS) { digitalWrite(pin, LOW); pinMode(pin, OUTPUT); }
  pwmOK = true;
  for (uint8_t pin : RGB_PINS) {
    digitalWrite(pin, COMMON_ANODE ? HIGH : LOW); pinMode(pin, OUTPUT);
    if (!ledcAttach(pin, 5000, 8)) pwmOK = false;
  }
  if (!pwmOK) {
    for (uint8_t pin : RGB_PINS) { ledcDetach(pin); digitalWrite(pin, COMMON_ANODE ? HIGH : LOW); pinMode(pin, OUTPUT); }
  }
  applyColor();
  dht.begin(); lastAttempt = millis();
  WiFi.mode(WIFI_STA); WiFi.setHostname(DEVICE_HOSTNAME); WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) delay(100);
  OtaService::begin(stopDrives);
  server.on("/", HTTP_GET, []() {
    server.sendHeader("Cache-Control", "no-store");
    server.send_P(200, "text/html; charset=utf-8", WEB_PAGE);
  });
  server.on("/api/state", HTTP_GET, sendState);
  server.on("/api/control", HTTP_POST, control);
  server.on("/api/range", HTTP_POST, configureRange);
  server.on("/api/motor", HTTP_POST, controlMotor);
  server.on("/api/servo", HTTP_POST, controlServo);
  server.begin();
}
void loop() {
  OtaService::handle();
  server.handleClient();
  handleMotor();
  handleServo();
  sampleSensor();
  handleMotor();
  delay(1);
}
