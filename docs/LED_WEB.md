# Niebieska dioda sterowana ze strony WWW

ESP32 udostępnia stronę w lokalnej sieci Wi-Fi. Przyciski „Włącz diodę” i
„Wyłącz diodę” ustawiają GPIO23. Po uruchomieniu programu dioda jest wyłączona.
Strona pokazuje ostatni stan ustawiony w programie, nie pomiar światła LED.
Po zmianie z innego telefonu/komputera użyj „Odśwież stan”.

## Plan obwodu — przed montażem

Plan dotyczy zwykłej, dwunóżkowej niebieskiej LED. Moduł LED z elektroniką,
dioda RGB lub adresowalna wymagają osobnego rozpoznania.
Przed montażem potwierdź urządzenie (ESP32-1/ESP32-2), typ LED, wolne otwory
oraz połączenia grup breadboardu zgodnie z lokalnym `.local/BREADBOARD.md`.
Poniższa tabela jest planem sygnałów, nie kompletną instrukcją pól montażowych.

| ESP32 / element | Następny element | Pole breadboardu |
|---|---|---|
| GPIO23, L-18 | rezystor 1 kΩ (1000 Ω) | wolne otwory do potwierdzenia lokalnie |
| drugi koniec rezystora | anoda (+) niebieskiej LED | do ustalenia |
| katoda (−) LED | GND, np. L-19 | wolne otwory do potwierdzenia lokalnie |

```text
GPIO23 (L-18) ── rezystor 1 kΩ ── anoda LED → katoda LED ── GND
```

Rezystor nie ma polaryzacji. W nowej, nieprzycinanej LED dłuższa nóżka zwykle
oznacza anodę, a krótsza i płaska krawędź obudowy wskazują katodę. Przy
niejednoznacznych oznaczeniach potwierdź polaryzację testem diody w mierniku.
Nóżki LED muszą być w osobnych grupach połączeń breadboardu.

GPIO pracuje z napięciem 3,3 V. Rezystor 1 kΩ ogranicza prąd do najwyżej
3,3 mA nawet przy zwarciu LED; rzeczywisty prąd zależy od jej napięcia
przewodzenia i będzie mniejszy. Jasność może być niewielka i zależy od LED.
Masa obwodu jest wspólna z ESP32. Nie podłączaj diody bez rezystora ani do 5 V.
Przewody wkładaj przy odłączonym zasilaniu; przed włączeniem sprawdź zwarcia
i oba końce każdego połączenia. Nie używaj otworów zajętych nóżkami ESP32.

## Uruchomienie po montażu

1. Zbuduj oba środowiska: `pio run -e esp32-1` i `pio run -e esp32-2`.
2. Po wyborze urządzenia i potwierdzeniu zgodności hasła OTA wgraj firmware
   tylko na tę płytkę, np. `pio run -e esp32-1 -t upload`.
3. Otwórz `http://esp32-1.local/` (lub odpowiednią nazwę drugiej płytki).
   Gdy mDNS nie działa, użyj lokalnego adresu urządzenia. Telefon/komputer
   musi mieć dostęp do ESP32 w tej samej sieci Wi-Fi.
4. Sprawdź nazwę urządzenia na stronie, włącz i wyłącz diodę.
5. Po restarcie dioda powinna być wyłączona. Sprawdź też kolejną aktualizację
   OTA, aby potwierdzić zachowanie aktualizacji bezprzewodowych.

Samo wejście na stronę nie włącza diody: służy do tego przycisk.
Nie potrzeba hostingu, aplikacji ani dostępu do Internetu.
Sterowanie HTTP nie wymaga hasła; używaj go w zaufanej sieci lokalnej,
bez przekierowywania portów routera.

## Obsługa HTTP

| Metoda i adres | Działanie |
|---|---|
| `GET /` | strona z tożsamością urządzenia i stanem LED |
| `POST /led/on` | włączenie i przekierowanie 303 do strony |
| `POST /led/off` | wyłączenie i przekierowanie 303 do strony |

`scripts/check_http.py` nadal sprawdza tożsamość urządzenia; nie potwierdza
fizycznego świecenia LED ani działania OTA.
