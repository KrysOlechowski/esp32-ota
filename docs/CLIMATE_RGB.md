# Klimat i kolor — DHT11 + KY-016

Status: użytkownik potwierdził działanie podstawowej wersji DHT11 + KY-016 po
wgraniu. Dodany później ręczny suwak nasycenia przeszedł testy obliczeń, interfejsu
z symulowanym API i kompilację obu środowisk, ale nie został jeszcze wgrany.
Nie wykonano niezależnego testu fizycznego nowego suwaka ani kolejnego OTA.

## Rozpoznane moduły

Użytkownik potwierdził usunięcie starej LED, rezystora i przewodów. ESP32-1 pozostaje
w stałych otworach opisanych lokalnie. Moduły nowego eksperymentu:

- DHT11 na małej płytce, 3 piny: `S` z jednej strony, `−` z drugiej.
  Układ odpowiada modułowi S / zasilanie / GND (typ KY-015); środkowy pin
  to zasilanie według dokumentacji tego układu. Brak potwierdzenia marki PCB.
- KY-016: użytkownik podał kolejność `B G R −` i trzy rezystory `151` (150 Ω).
  Typowy układ ze wspólną katodą. Nie utożsamiać z osobną, gołą LED RGB.
- Serwo anulowane. Fotorezystor i goła RGB nie są używane w tym eksperymencie.

## Plan sygnałów

| ESP32 | Pozycja | Element |
|---|---|---|
| GPIO25 | P-11 | KY-016 R |
| GPIO26 | P-10 | KY-016 G |
| GPIO27 | P-09 | KY-016 B |
| GND | L-19 | KY-016 − |
| GPIO18 | L-11 | DHT11 S |
| 3V3 | P-19 | DHT11 środkowy pin zasilania |
| GND | L-13 | DHT11 − |

Dokładny plan otworów i stan montażu znajdują się w `.local/BREADBOARD.md`.
Połączenia wykonuj przy odłączonym USB. Zasilanie ESP pozostaje z obecnej ładowarki
5 V / 2 A przez cały kabel Micro-B. W nowym planie z krokowcem ESP32 otrzymuje
zasilanie z USB lampki 5 V / 1 A; szczegóły: [STEPPER_WEB.md](STEPPER_WEB.md). Na GPIO oraz zasilanie DHT dajemy 3,3 V,
nie 5 V. Oba moduły mają wspólną masę ESP32.

KY-016 ma trzy potwierdzone rezystory 150 Ω; nie dodajemy rezystora do wspólnej
katody. Dla referencyjnych napięć przewodzenia z dokumentacji Joy-IT
(R: 1,8 V, G/B: 2,8 V) prąd przy 3,3 V wynosi orientacyjnie 10 mA R i po 3,3 mA
G/B przy pełnym PWM. To oszacowanie, nie pomiar użytkownika; jasności kanałów
mogą się różnić. DHT11 to małe obciążenie czujnikowe, bez prądu rozruchowego silnika.
Dla krótkich przewodów spróbuj z wbudowanym podciąganiem modułu/biblioteki;
jeśli odczyt zawodzi, sprawdź rezystor podciągający S do 3V3 (np. posiadany 10 kΩ).
Nie podciągaj sygnału do 5 V.

## Działanie strony

- Kolor ręczny: dowolny RGB, nasycenie 0–100% oraz osobna jasność 0–100%.
  Nasycenie dodaje biel/szarość do wybranego koloru: 0% daje szarość o tej samej
  wartości maksymalnego kanału, 100% zachowuje wybrany RGB. Zmiany stosuje
  przycisk „Ustaw kolor”. Auto nadal wyznacza nasycenie z wilgotności.
  Ręczne nasycenie domyślnie 100%, resetowane po restarcie.
- Auto: temperatura ustala odcień, wilgotność ustala nasycenie.
- Domyślna temperatura: 17°C niebieski, 18,25°C turkus, 19,5°C zielony,
  20,75°C żółty, 22°C czerwony. Przejścia ciągłe; wartości poza zakresem
  utrzymują kolor skrajny. Fizyczna barwa zależy od LED i prądu kanałów.
- Wilgotność: 30% RH daje nasycenie 55%, 70% RH — 100%.
  Sucho oznacza pastelowe barwy, wilgotno — mocne, czyste kolory.
- Zakresy temperatury i wilgotności można zmienić na stronie. To ustawienia
  mapowania koloru, nie kalibracja czujnika. Nie zapisują się po restarcie.
- Restart: tryb ręczny, lampka wyłączona, jasność 60%, kolor domyślny #0064ff.
- DHT odczytywany co 2,5 s. Odczyt biblioteki zajmuje krótko ok. 20–30 ms,
  pozostały czas pętla obsługuje HTTP i OTA. Nie jest to całkowicie asynchroniczny driver.
- Błąd pomiaru gasi RGB w auto; ręczne sterowanie nadal działa. Ostatni poprawny
  odczyt jest pokazany wraz z komunikatem błędu i wiekiem pomiaru. Brak pomiaru
  daje null w API. Nie zastępuj błędu temperaturą 0°C.
- DHT11 ma typowo dokładność ±2°C i ±5% RH, starsze warianty rozdzielczość 1°C.
  W zakresie 17–22°C efekt będzie wyraźny, ale rzeczywiste przejścia mogą być skokowe.

## Status lampki na stronie

Sekcja Lampka na górze strony pokazuje osobno tryb sterowania, ustawioną jasność,
kolor HEX i RGB, odcień HSV w stopniach oraz wynikowe nasycenie HSV.
Nasycenie jest obliczane z aktualnego RGB: (max − min) / max × 100%.
Nie jest to wilgotność ani wartość suwaka jasności. Dla #FF2525 wynosi około 85,5%.
Ręczny suwak nasycenia skaluje nasycenie koloru z selektora, dlatego 100% suwaka
nie zawsze oznacza 100% wynikowego nasycenia. W auto źródłem jest wilgotność.
HEX/RGB to kolor przed jasnością i gamma, nie pomiar optyczny. Przy wyłączeniu,
błędzie PWM lub braku odczytu w auto szczegóły koloru pokazują kreski.

## API i sprawdzenie

- GET `/api/state`: tożsamość, tryb, pomiary i ich wiek, stan wyjść, kolory, zakresy.
- POST `/api/control`: `mode=manual&color=%23ff0000&brightness=60&saturation=55`,
  `mode=auto&brightness=60` lub `mode=off`.
- POST `/api/range`: `cold=17&warm=22&dry=30&wet=70`.
- Walidacja przed zmianą stanu: błędne parametry dają HTTP 400, bez zastosowania zmian.
- Jasność ma korekcję gamma 2,2, PWM: 5 kHz / 8 bitów.
- Interfejs nie korzysta z zewnętrznych skryptów, fontów ani Internetu.

Test gradientu na komputerze:

```sh
c++ -std=c++11 -Iinclude test/host/climate_color_test.cpp -o /tmp/climate-color-test
/tmp/climate-color-test
```

Po montażu zbuduj oba środowiska i wgraj tylko ESP32-1. Sprawdź najpierw ręcznie
czerwony, zielony i niebieski, potem odczyt DHT i auto. Testuj błędne zakresy oraz
odłączenie sensora (przy wyłączonym zasilaniu), a następnie kolejne OTA.
`check_http.py` obsługuje tożsamość zarówno starej strony LED, jak i nowego API.

## Źródła (2026-11-13)

- [Joy-IT KY-016: wspólna katoda i parametry LED](https://sensorkit.joy-it.net/en/sensors/ky-016)
- [Keyestudio DHT11/KY-015: układ S / środek zasilanie / minus](https://docs.keyestudio.com/projects/KT0193F/en/latest/docs/30.html)
- [Joy-IT DHT11: zasilanie 3,3 V i parametry pomiaru](https://www.sensorkit.joy-it.net/en/sensors/ky-015)
- [Adafruit DHT: sterownik i odstęp odczytu](https://github.com/adafruit/DHT-sensor-library)
