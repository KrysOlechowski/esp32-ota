# Zachowanie OTA podczas eksperymentów

OTA jest wydzielone w src/ota_service.cpp i include/ota_service.h. Setup łączy Wi-Fi,
wywołuje OtaService::begin(), a loop stale wywołuje OtaService::handle(). Eksperymenty
z LED, serwem i czujnikami dodawaj tak, by nie blokowały obsługi OTA/HTTP.

Nie usuwaj tych wywołań, nie zmieniaj bez potrzeby ArduinoOTA.setHostname/setPassword,
nie nadpisuj wrappera uploadu i nie wybieraj schematu partycji bez OTA.
Obecny default.csv ma sloty OTA; aktualizacja OTA wysyła aplikację, nie migruje tablicy partycji.
Nie zakładaj, że zmiana board_build.partitions przez OTA zmieni układ pamięci urządzenia.

Rozdzielenie plików oraz instrukcje dla agentów zmniejszają ryzyko, lecz nie blokują
usunięcia OTA. Zły kod, długie pętle, brak Wi-Fi/zasilania, zła partycja albo zbyt duży firmware
mogą uniemożliwić aktualizację. Wtedy potrzebny jest dostęp USB i odpowiedni firmware.
Nie wdrożono automatycznego rollbacku ani sprzętowego potwierdzenia działania.

Checklist po zmianie:

1. Build obu środowisk i sprawdzenie rozmiaru względem slotu aplikacji.
2. Przegląd setup/loop oraz zmian w partycjach i uploadzie.
3. Upload świadomie wybranej płytki; odczyt HTTP z jej tożsamością.
4. Kolejny upload OTA i sprawdzenie po restarcie.

Hasło użyte przez uploader musi pasować do aktualnego firmware. Zwykła zmiana lokalnego
hasła nie aktualizuje od razu urządzenia. Kompilacja zaszywa sekrety w binarnym firmware.
Wrapper ładuje espota z zainstalowanego pakietu PlatformIO; nie przekazuje auth w argv
procesu i blokuje logowanie DEBUG/INFO. Hasło pozostaje w pamięci procesu i firmware.
Nie publikuj zrzutów pamięci ani plików build. To hasłowe OTA w zaufanej sieci,
bez dodatkowego podpisywania obrazów i bez gwarancji ochrony przed każdym napastnikiem.

[PlatformIO OTA](https://docs.platformio.org/en/latest/platforms/espressif32.html#over-the-air-ota-update)
· [Przykład ArduinoOTA](https://github.com/espressif/arduino-esp32/blob/master/libraries/ArduinoOTA/examples/BasicOTA/BasicOTA.ino)
· [Partycje ESP32](https://docs.espressif.com/projects/arduino-esp32/en/latest/tutorials/partition_table.html)
