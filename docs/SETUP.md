# Uruchomienie i migracja

## Co zachowano

Profil nodemcu-32s, Arduino, Wi-Fi STA, hasłowe ArduinoOTA i prosty HTTP pochodzą
z działającego projektu. Platformę espressif32 przypięto do zastanej wersji 8.0.0.
Wspólne źródła obsługują dwa środowiska; nie ma potrzeby dwóch folderów projektów.
Nazwy OTA to esp32-1 i esp32-2; adresy i dane dostępowe pozostają w konfiguracji lokalnej.
Firmware korzysta z DHCP: upload_host określa cel wysyłania, nie ustawia statycznego IP płytki.
Dla trwałych IP ustaw rezerwacje DHCP w routerze dopiero w ramach osobnego zlecenia.

## Na tym stanowisku

Migracja zachowuje dane w secrets.local.json. Nie nadpisuj tego pliku szablonem.
Prywatny opis stanowiska jest w .local/STANOWISKO.md. Poprzednie pliki
z danymi dostępowymi są w .private-backup/before-cleanup/, poza notatkami i poza Gitem.
W razie wycofania przywróć z tej kopii platformio.ini, src/main.cpp i .gitignore;
po przywróceniu ponownie zabezpiecz sekrety przed jakimkolwiek git add.
Powrót plików na dysku nie cofa firmware w urządzeniu.

## Na nowym komputerze

1. Zainstaluj VS Code i rozszerzenie PlatformIO IDE, otwórz katalog repo.
2. Skopiuj secrets.example.json jako secrets.local.json (tylko jeśli jeszcze nie istnieje).
3. Uzupełnij SSID, hasło Wi-Fi oraz upload_host i ota_password obu płytek.
4. Zachowaj hostname zgodny z wybraną płytką. Dla drugiej sieci można dopisać
   wifi_ssid i wifi_password wewnątrz wpisu konkretnego urządzenia.
5. Uruchom build obu środowisk, następnie wgraj na świadomie wybraną płytkę.
6. Po restarcie sprawdź HTTP i jeszcze jedno OTA, aby potwierdzić zachowanie obsługi OTA.

Przykładowe komendy z katalogu projektu:

```sh
# Build obu płytek
pio run -e esp32-1
pio run -e esp32-2

# Upload ESP32-1
pio run -e esp32-1 -t upload

# Upload ESP32-2
pio run -e esp32-2 -t upload

# Weryfikacja HTTP pod adresem z lokalnej konfiguracji
python3 scripts/check_http.py esp32-1
python3 scripts/check_http.py esp32-2
```

Na macOS, gdy pio nie jest w PATH albo używasz sprawdzonego Terminala:

```sh
~/.platformio/penv/bin/pio run -e esp32-1
~/.platformio/penv/bin/pio run -e esp32-2
~/.platformio/penv/bin/pio run -e esp32-1 -t upload
~/.platformio/penv/bin/pio run -e esp32-2 -t upload
# Zgodność ze starą komendą — tylko ESP32-1
~/.platformio/penv/bin/pio run -e nodemcu-32s -t upload
```

Sama zmiana upload_host przekierowuje wgrywanie. Nie zmienia hostname ani sekretów
zapisanych w firmware. Wybór środowiska ustawia komplet danych dla właściwej płytki.
Zmiana hasła OTA wymaga migracji: najpierw wyślij firmware z nowym hasłem,
autoryzując wysyłkę starym hasłem; domyślna konfiguracja używa jednej wartości do obu
celów, więc taka operacja wymaga osobnego, świadomego przygotowania lub USB.
Nie zmieniaj haseł w ramach rutynowej podmiany IP.

## VS Code

Zainstaluj rekomendowane rozszerzenie. W PlatformIO → Project Tasks wybierz
środowisko i Build/Upload, albo Terminal → Run Task → „Build: esp32-1” /
„Upload OTA: esp32-1” (analogicznie dla 2). Zadania wymagają pio w PATH terminala;
jeśli go brak, użyj widoku PlatformIO lub komendy macOS powyżej.
Generowane c_cpp_properties.json i launch.json są ignorowane, bo zawierają ścieżki komputera.
Nie uruchamiaj Upload bez wyboru środowiska — możesz objąć obie płytki.

## macOS: Terminal działa, VS Code nie wysyła OTA

Stan zgłoszony przez użytkownika: OTA i HTTP obu płytek wcześniej działały;
OTA z systemowego Terminala działało, a terminal VS Code miał problem z UDP/dostępem do LAN.
To nie uzasadnia zmiany pinów, haseł ani wyłączania OTA.

1. Ustawienia systemowe → Prywatność i ochrona → Sieć lokalna: sprawdź dostęp dla
   Visual Studio Code i Terminala, jeśli są na liście. Po zmianie uruchom aplikację ponownie.
2. Użyj tej samej komendy i tego samego środowiska w systemowym Terminalu.
   Pełna ścieżka do pio w terminalu VS Code nie omija uprawnień tej aplikacji.
3. Sprawdź, czy komputer i płytka są w tej samej osiągalnej sieci; sieć gościnna,
   izolacja klientów, VPN lub firewall mogą blokować ruch.
4. ArduinoOTA używa UDP do zaproszenia i połączenia TCP zwrotnego do komputera.
   Działające HTTP nie dowodzi, że te połączenia są dozwolone.
5. Nie wyłączaj globalnie ochrony systemu. Do czasu rozwiązania problemu używaj
   systemowego Terminala. Instrukcja nie oznacza, że uprawnienia zostały teraz naprawione.

[Apple: dostęp aplikacji do sieci lokalnej](https://support.apple.com/en-gb/guide/mac-help/mchla4f49138/mac)

## Pierwsze wgranie lub utrata OTA: USB

OTA wymaga już działającego firmware z ArduinoOTA. Nową płytkę lub firmware bez OTA
odzyskuje się przez USB z przewodem obsługującym dane. Zasilacz ścienny nie przesyła danych.
Nie uruchamiaj erase i nie zmieniaj partycji tylko po to, by „naprawić” błąd LAN.

Do świadomego odzyskania dodaj lokalnie środowisko USB (plik jest ignorowany):

```ini
; platformio.local.ini
[env:esp32-1-usb]
extends = env:esp32-1
upload_protocol = esptool
upload_command =

[env:esp32-2-usb]
extends = env:esp32-2
upload_protocol = esptool
upload_command =
```

W sekcji [platformio] dopisz `extra_configs = platformio.local.ini`.
Wybierz lokalny port z `pio device list`, następnie:

```sh
pio run -e esp32-1-usb -t upload --upload-port PORT_USB
pio run -e esp32-2-usb -t upload --upload-port PORT_USB
```

PORT_USB zastąp rzeczywistym portem. Lokalne aliasy USB są obsługiwane przez loader
konfiguracji, ale nie są domyślną drogą uploadu. Przy nietypowej płytce może być potrzebne
BOOT/EN według jej dokumentacji. Przed tą operacją sprawdź fizyczną płytkę i jej port.
