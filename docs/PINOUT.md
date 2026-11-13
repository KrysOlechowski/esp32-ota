# Mapa pinów i podręczna karta ESP32

## Zakres i pewność

Mapa referencyjna dla opisanej przez użytkownika płytki NodeMCU ESP-32S, 38 pinów,
z modułem ESP-WROOM-32. Użytkownik potwierdził pozycje przy swoim stanowisku.
Agent nie otrzymał zdjęć przy tej weryfikacji i nie wykonał pomiaru fizycznego.
Użytkownik potwierdził, że ESP32-1 i ESP32-2 są takimi samymi płytkami.
Wspólna mapa P/L obowiązuje dla obu egzemplarzy przy zachowaniu opisanej orientacji.
To nie oznacza jednakowego położenia na breadboardzie ani jednakowego okablowania.

Weryfikacja dokumentacyjna z 2026-11-13:

- Przypisania nazw sygnałów do GPIO są zgodne z dokumentacją klasycznego ESP32-WROOM-32.
- Kolejność P-01…P-19 odpowiada odwróconej kolejności J2, a L-01…L-19 odwróconej
  kolejności J3 w dokumentacji ESP32-DevKitC V4. To porównanie układu sygnałów,
  nie identyfikacja PCB użytkownika jako DevKitC ani dowód położenia przycisku EN.
- Użytkownik podał rzeczywiste otwory montażu; tabela jest zapisana lokalnie
  w ignorowanym `.local/BREADBOARD.md`. Połączenia szyn i wolne miejsca wymagają
  odrębnej informacji; samo położenie nóżek nie ustala zajętości całego stanowiska.
- Numer pozycji P/L nie jest numerem GPIO, numerem wyprowadzenia samego modułu
  ani nadrukowaną liczbą breadboardu.

## Stała orientacja

Patrzymy z góry: USB po lewej, antena po prawej.

- **P**: strona wskazana przez użytkownika jako strona przycisku EN, górna w jego
  oryginalnym zdjęciu. Nazwa umowna „prawy” nie oznacza prawej krawędzi ekranu.
- **L**: strona przeciwna, dolna w oryginalnym zdjęciu (umownie „lewy”).
- Pozycje **01** są przy USB; **19** są przy antenie.
- Zdjęcie spodu lub obróconej płytki może odwracać obraz. Nie zmieniaj przez to konwencji P/L.
- Dawne „lewo-1” oznacza L-01, „lewo-19” oznacza L-19; nadal nie są to adresy otworów.

```text
                          od USB → do anteny
P (górny w opisie)   P-01  P-02  …  P-18  P-19
USB                  [    płytka ESP32    ]  ANTENA
L (dolny w opisie)   L-01  L-02  …  L-18  L-19
```

## Pełna mapa

„Start” oznacza pin konfiguracyjny odczytywany przy resecie; „flash” oznacza pin
zarezerwowany dla pamięci, którego nie używamy do modułów.

| Pozycja P | Nadruk | Sygnał / ograniczenie | Pozycja L | Nadruk | Sygnał / ograniczenie |
|---|---|---|---|---|---|
| P-01 | `5V` | 5 V; nie GPIO | L-01 | `CLK` | GPIO6; flash |
| P-02 | `CMD` | GPIO11; flash | L-02 | `SD0` | GPIO7; flash |
| P-03 | `SD3` | GPIO10; flash | L-03 | `SD1` | GPIO8; flash |
| P-04 | `SD2` | GPIO9; flash | L-04 | `P15` | GPIO15; start |
| P-05 | `P13` | GPIO13 | L-05 | `P2` | GPIO2; start |
| P-06 | `GND` | masa | L-06 | `P0` | GPIO0; start |
| P-07 | `P12` | GPIO12; start | L-07 | `P4` | GPIO4 |
| P-08 | `P14` | GPIO14 | L-08 | `P16` | GPIO16 |
| P-09 | `P27` | GPIO27 | L-09 | `P17` | GPIO17 |
| P-10 | `P26` | GPIO26 | L-10 | `P5` | GPIO5; start |
| P-11 | `P25` | GPIO25 | L-11 | `P18` | GPIO18 |
| P-12 | `P33` | GPIO33 | L-12 | `P19` | GPIO19 |
| P-13 | `P32` | GPIO32 | L-13 | `GND` | masa |
| P-14 | `P35` | GPIO35; tylko wejście | L-14 | `P21` | GPIO21 |
| P-15 | `P34` | GPIO34; tylko wejście | L-15 | `RX` | GPIO3 / RX0 |
| P-16 | `SVN` | GPIO39; tylko wejście | L-16 | `TX` | GPIO1 / TX0 |
| P-17 | `SVP` | GPIO36; tylko wejście | L-17 | `P22` | GPIO22 |
| P-18 | `EN` | reset / enable; nie GPIO | L-18 | `P23` | GPIO23 |
| P-19 | `3V3` | 3,3 V; nie GPIO | L-19 | `GND` | masa |

## Ograniczenia do codziennej pracy

- **Flash: GPIO6–11** — L-01, L-02, L-03, P-02, P-03, P-04. Nie podłączaj LED,
  serw ani czujników. Nazwa CLK nie oznacza wolnego zegara dla modułu.
- **Tylko wejście:** GPIO34 (P-15), GPIO35 (P-14), GPIO36 (P-17), GPIO39 (P-16).
  Brak wewnętrznych pull-up/pull-down; w razie potrzeby dobierz zewnętrzny rezystor.
- **Start:** GPIO0 (L-06), GPIO2 (L-05), GPIO5 (L-10), GPIO12 (P-07), GPIO15 (L-04).
  Obciążenie może zmienić warunki rozruchu; pomijaj je w pierwszych eksperymentach.
- **UART0:** RX / GPIO3 (L-15), TX / GPIO1 (L-16) służą zwykle do logów/programowania.
- **GPIO16/17:** L-08/L-09 są do użycia na potwierdzonym WROOM-32; nie przenoś
  tej zasady na moduły WROVER, gdzie mogą obsługiwać PSRAM.
- **JTAG:** GPIO12–15 mogą być zajęte podczas debugowania sprzętowego.
- **ADC przy Wi-Fi/OTA:** do analogowych pomiarów wybieraj ADC1: GPIO32 (P-13),
  33 (P-12), 34 (P-15), 35 (P-14), 36 (P-17), 39 (P-16).
  ADC2 koliduje z Wi-Fi; nie oznacza to zakazu cyfrowego używania tych GPIO.
- **Logika 3,3 V:** żadnego 5 V na GPIO ani 3V3. EN (P-18) to enable/reset,
  nie pin do sterowania modułem. Oznaczenie 5V (P-01) nie ustala kierunku zasilania,
  wydajności ani zabezpieczenia przed zasilaniem zwrotnym — to zależy od konkretnego PCB.
- **Masa:** GND jest na P-06, L-13 i L-19. Moduły współpracujące sygnałowo mają
  wspólną masę; można użyć różnych potwierdzonych GND, nie muszą to być ten sam otwór.
- Serw i silników nie zasilaj z GPIO/3V3. Zachowaj cały kabel USB i sprawdź budżet
  zasilania oraz sterownik zgodnie z [instrukcją sprzętu](HARDWARE.md).

## Wygodne GPIO do pierwszych projektów

Po potwierdzeniu wolnych połączeń, napięć i funkcji modułu:

| Zastosowanie / kandydat | Pozycja | Uwaga |
|---|---|---|
| LED, sygnał cyfrowy: GPIO23 | L-18 | LED wymaga rezystora; to propozycja, nie zajęty pin |
| I2C SDA: GPIO21 | L-14 | ustaw zgodnie z kodem i wymaganiami modułu |
| I2C SCL: GPIO22 | L-17 | pull-up do napięcia zgodnego z 3,3 V |
| Inne cyfrowe: GPIO18 / GPIO19 | L-11 / L-12 | sprawdź, czy nie są używane przez SPI |
| Analogowe: GPIO32 / GPIO33 | P-13 / P-12 | ADC1, zasilanie/napięcie wejścia do sprawdzenia |

GPIO w ESP32 mogą pełnić różne funkcje. Ta tabela nie rezerwuje pinów i nie zastępuje
wymagań konkretnego czujnika czy biblioteki. Nie zakładaj modelu czujnika ani serwa.

## Jak wskazywać przewód

Każde połączenie ma zawierać: **sygnał/GPIO + pozycję P/L + potwierdzony wolny otwór
połączony z tym pinem + dokładny drugi koniec + rolę przewodu**.

Format odpowiedzi agenta:

| Krok | Rola / proponowany kolor | Koniec przy ESP32 | Drugi koniec |
|---|---|---|---|
| 1 | masa / czarny | GND, L-19; otwór: do ustalenia | GND modułu; otwór: do ustalenia |
| 2 | SDA / zielony | GPIO21, L-14; otwór: do ustalenia | SDA rozpoznanego modułu; otwór: do ustalenia |

To przykład formatu, nie gotowy montaż konkretnego czujnika. Kolor jest pomocą,
nie potwierdzeniem funkcji przewodu. Nie wymyślaj adresów typu a19 ani nie traktuj
P/L jako kolumn breadboardu. Otwór zajęty przez nóżkę ESP32 nie jest wolnym miejscem
na przewód: wskaż wolny otwór w tej samej, potwierdzonej grupie połączeń.

## Źródła i utrzymanie karty

- Mapa P/L i orientacja: dane użytkownika, potwierdzone przez niego na stanowisku.
- [Espressif: ESP32-WROOM-32, definicje pinów](https://documentation.espressif.com/esp32-wroom-32_datasheet_en.html)
  — znaczenie nadruków, pamięć flash, EN i zasilanie modułu.
- [Espressif: ESP32-DevKitC V4, Header Block J2/J3](https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32/esp32-devkitc/user_guide.html#header-block)
  — wyłącznie porównanie kolejności złączy; inny produkt niż deklarowana płytka użytkownika.
- [Espressif: GPIO & RTC GPIO](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/peripherals/gpio.html)
  — piny startowe, wejściowe, UART/JTAG i ograniczenia ADC2.
- [ESP32 datasheet](https://documentation.espressif.com/esp32_datasheet_en.html)
  — parametry elektryczne i rozruch.

Przy zwykłym użyciu opisanych pinów korzystaj z tej karty bez ponownego przeszukiwania
Internetu. Sięgaj do dokumentacji przy nowym module, innym wariancie ESP32/PCB,
nieopisanej funkcji, szczegółowych parametrach elektrycznych lub sprzecznych informacjach.
Wynik nowej weryfikacji dopisz tutaj ze źródłem i datą; nie utrwalaj przypuszczeń jako faktów.
