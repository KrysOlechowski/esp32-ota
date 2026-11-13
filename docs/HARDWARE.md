# Sprzęt i okablowanie

## Inwentarz

| Element | Wiadomo | Do ustalenia przed podłączeniem |
|---|---|---|
| ESP32-1 | moduł ESP-WROOM-32; OTA esp32-1 | położenie i aktualne przewody na breadboardzie |
| ESP32-2 | moduł ESP-WROOM-32; OTA esp32-2 | położenie i aktualne przewody na breadboardzie |
| Zasilacz USB | 5 V, 2 A | marka/model, stan i parametry dodatkowych obciążeń |
| Kabel | cały USB Micro-B | do uploadu USB wymagana obsługa danych |
| Płytka stykowa | oznaczone rzędy | orientacja i ciągłość szyn zasilających |
| LED, rezystory, przewody | zestaw do Arduino | kolory, wartości i typy |
| Serwa / silniki | moduły z zestawu | model, zasilanie, prąd rozruchu/zatrzymania |
| Czujnik temperatury | obecny w zestawie | model, pinout, napięcie, protokół |
| Inne moduły Arduino | różne elementy | oznaczenia i dokumentacja |

Adresy IP nie są częścią publicznej dokumentacji; lokalny spis jest w .local/.
ESP-WROOM-32 oznacza moduł radiowy, nie jednoznaczny model całej płytki.
Profil nodemcu-32s jest zachowany z istniejącego projektu; nie jest dowodem modelu devboardu.
Nie potwierdzono obecnego okablowania modułów. Poniższe przykłady są planem, nie zapisem montażu.

## Mapa pinów stanowiska

Pełna [mapa 38 pinów P/L i podręczna karta ograniczeń](PINOUT.md) została dodana
na podstawie sprawdzenia użytkownika. Użytkownik potwierdził, że obie płytki są takie same;
mapa P/L obowiązuje dla ESP32-1 i ESP32-2. Adresy otworów i okablowanie ustala się osobno. Nie utożsamiaj P/L z numeracją breadboardu.
Przy pracy korzystaj także z lokalnego `.local/BREADBOARD.md`.

## Zasilanie obecnego stanowiska

```text
Zasilacz USB 5 V / 2 A ── cały kabel USB Micro-B ── gniazdo USB płytki ESP32
```

Nie przecinaj kabla. 2 A to maksymalna wydajność zasilacza, nie prąd wymuszany na płytce.
Nie zakładaj, że regulator 3V3 ani piny zasilania devboardu dostarczą tyle do modułów.
Nie łącz dwóch źródeł 5 V równolegle (np. USB komputera i zewnętrznego zasilacza)
bez potwierdzonego układu separacji na konkretnej płytce.

ESP32 używa logiki 3,3 V. Nie podawaj 5 V na GPIO ani na pin 3V3.
Moduł „Arduino 5 V” może wymagać konwertera poziomów lub dzielnika na wyjściu do ESP32.
Współpracujące układy sygnałowe wymagają wspólnej masy.

## Płytka stykowa z oznaczonymi rzędami

Typowo pięć otworów a–e w danym rzędzie jest połączonych, podobnie f–j;
środkowa szczelina rozdziela obie grupy. Boczne szyny mogą być przerwane w połowie.
To opis typowego breadboardu, nie pomiar tej konkretnej płytki.
Sprawdź ciągłość miernikiem przy odłączonym zasilaniu. Kolory i nadruk +/− same nie tworzą połączeń.

W notatce lokalnej narysuj orientację płytki, kierunek gniazda USB i zajęte pola.
Gdy prosisz GPT/Codex o montaż, podaj zdjęcie/oznaczenia oraz numerację rzędów;
agent powinien wtedy wskazywać konkretne pola, a nie zgadywać położenia płytki.

## Pierwszy eksperyment: zewnętrzna LED (propozycja)

Po potwierdzeniu pinoutu i wolnego GPIO można użyć GPIO23:

| ESP32 / element | Połączenie |
|---|---|
| GPIO23 (L-18 według mapy) | rezystor 330 Ω do 1 kΩ |
| Drugi koniec rezystora | anoda LED |
| Katoda LED | GND |

Zweryfikuj polaryzację LED; długość nóżek nie wystarczy, jeśli były przycinane.
Nie podłączaj LED bez rezystora. Konkretne pola breadboardu ustala się po poznaniu montażu.
Program sterujący LED powinien używać millis() i zostawić obsługę OTA w loop.

## Serwo / silnik

Najpierw odczytaj model i wymagany prąd. Serwa nie zasilaj z GPIO ani 3V3 ESP32.
Użyj właściwego zasilania o odpowiedniej wydajności, połącz masę z GND ESP32,
a sygnał podaj na potwierdzony wolny GPIO. Akceptacja sygnału 3,3 V zależy od modelu.
Nie zakładaj, że wspólny zasilacz 5 V / 2 A wystarczy na serwo i ESP32.
Dla silnika DC potrzebny jest odpowiedni sterownik i zabezpieczenie indukcyjne;
nie podłączaj silnika bezpośrednio do GPIO. Nie wyprowadzaj zasilania przez przecięcie USB.

## Czujnik temperatury i inne moduły

Nie rozpoznano typu: może to być np. termistor, układ analogowy, DHT lub DS18B20.
Mają różne pinouty, napięcia i biblioteki. Nie ma jednej bezpiecznej instrukcji
podłączenia „czujnika temperatury”. Najpierw model, zdjęcie opisów i dokumentacja,
potem tabela połączeń, ewentualny pull-up i kod. Pull-up linii ESP32 powinien mieć
napięcie zgodne z logiką 3,3 V.

## Dobór pinów ESP32

- GPIO6–11 są zwykle używane przez flash — nie do eksperymentów.
- GPIO34–39 są tylko wejściowe i nie mają wewnętrznych pull-up/pull-down.
- GPIO0, 2, 5, 12, 15 wpływają na start; podłączone moduły mogą zablokować boot.
- GPIO1/3 są zwykle UART-em używanym przez USB/monitor.
- ADC2 klasycznego ESP32 koliduje z działającym Wi-Fi; do pomiarów analogowych wybieraj
  dostępne piny ADC1 po weryfikacji konkretnego devboardu.

Źródło ograniczeń układu: [ESP32 datasheet](https://documentation.espressif.com/esp32_datasheet_en.html).
Oznaczenia na devboardzie zawsze sprawdź z jego pinoutem. Podłączaj i zmieniaj przewody
przy odłączonym zasilaniu, sprawdź zwarcia, a dopiero potem uruchamiaj.
