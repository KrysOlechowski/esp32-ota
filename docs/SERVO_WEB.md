# SG90 — dodatek do DHT11, RGB i krokowca

Status: użytkownik potwierdził działanie SG90 na ESP32-1. Dodano następnie
zapis zera, prędkość oraz ±10°; ta nowsza wersja wymaga uploadu i testu sprzętowego.

## Połączenia i zasilanie

| Sygnał ESP32 / zasilanie | Pozycja | Przewód SG90 |
|---|---|---|
| GPIO16 | L-08 | żółty/pomarańczowy — sygnał |
| +5 V z osobnej ładowarki silników | poza ESP32 | czerwony |
| minus ładowarki silników, wspólny z GND ESP32 | poza ESP32 | brązowy |

Szczegółowe proponowane otwory zapisane w lokalnej notatce breadboardu.
Nie wkładać przewodów pod płytkę ESP32 ani do otworów zajętych nóżkami.
Montaż przy obu zasilaniach odłączonych. Nie zasilać serwa z GPIO, 3V3 ani
przez tor 5V ESP32. ESP32 pozostaje na USB lampki 5 V / 1 A.

Ładowarka silników 5 V / 2 A zasila równolegle ULN2003 oraz SG90. Przewody
serwa prowadzą do wspólnych węzłów zasilania poza ESP32, nie do wyjść ULN2003.
Zachować mostek zasilania na ULN2003. Program rozdziela aktywne sterowanie obu
napędów w czasie, ale nie odłącza elektrycznie ich zasilania. Wydajność ładowarki,
przewodów i styków przy impulsach prądu serwa nie została zmierzona. Pierwszy test
bez obciążenia i mechanizmu na ramieniu. Przy resetach lub drżeniu przerwać test,
odłączyć zasilania i sprawdzić tor zasilania. Nie zwiększać napięcia.

Potwierdzony napis SG90 i kolory: HARDWARE.md. Dane producenta: 4,8 V,
[SG90 Analog](https://towerpro.com.tw/product/sg90-analog/). Próg sygnału wejściowego
niepodany; działanie z 3,3 V pozostaje do potwierdzenia dla egzemplarza. Nie
przenosić danych prądu cyfrowego SG90 jako pomiaru tego serwa.

## Panel i program

- Suwak 45–135°, przycisk Ustaw kąt, Środek 90° i Zwolnij serwo.
- Prędkość Wolno/Normalnie/Szybko oraz przyciski −10°/+10° (ograniczone do zakresu).
- Zapisz ostatni kąt jako zero: po zakończeniu sterowania zapisuje ostatni kąt
  impulsów w NVS ESP32 (Preferences, namespace servo, klucz zero). Wymaga
  wcześniejszego polecenia kąta od uruchomienia. Nie porusza osią i nie kalibruje
  czujnika fizycznego. Zwolnienie w trakcie ruchu zapisuje ostatni wysłany kąt,
  nie cel niedokończonego ruchu.
- Wróć do zera: wysyła zapamiętany kąt z wybraną prędkością, w zakresie 45–135°.
  Zapis przeżywa normalny restart i OTA, ale nie kasowanie pamięci NVS.
  Po restarcie brak automatycznego ruchu. Prędkość nie jest zapisywana.
- Błąd zapisu NVS daje HTTP 500 i nie zgłasza sukcesu. Ponowny zapis tego samego
  zera nie wykonuje niepotrzebnego zapisu do flash. ESP32 nie odczytuje osi SG90.
- Ograniczony zakres początkowy omija skrajne położenia. Kąt zadany, nie zmierzony.
- Impulsy 50 Hz: mapowanie nominalne 0–180° na 1000–2000 µs; w użytym zakresie
  1250–1750 µs. Kalibracja mechaniczna egzemplarza jeszcze niewykonana.
- Pierwsze włączenie sygnału może szybko ustawić oś w pobliżu 90°; dalsze zmiany
  realizowane programowo co 1°: Wolno co 40 ms, Normalnie co 20 ms,
  Szybko co 10 ms (nominalnie 25/50/100°/s), bez nadrabiania serią.
  To prędkość zmiany zadania, nie pomiar rzeczywistej szybkości ramienia.
- Sekundę po dojściu do kąta program wyłącza impulsy. Nie polegać na trzymaniu
  pozycji po wyłączeniu sygnału. Zwolnij serwo nie odcina przewodu zasilania.
- Restart: brak impulsów i automatycznego ruchu. Utrata Wi-Fi oraz rozpoczęcie
  OTA wyłączają sygnał serwa. OtaService::begin/handle pozostają zachowane.
- Podczas aktywnego sygnału serwa ruch krokowca jest odrzucany (409); podczas
  ruchu krokowca polecenie kąta serwa jest odrzucane (409). Stop krokowca oraz
  Zwolnij serwo pozostają niezależnie dostępne. RGB i DHT działają równolegle.
- PWM serwa: kanał LEDC 6, 50 Hz, 16 bitów. RGB pozostaje na osobnych kanałach
  0–2 z 5 kHz / 8 bitów. Lokalny core Arduino 3.3.12 dobiera osobny timer dla
  innej częstotliwości/rozdzielczości; nie nadpisujemy timera RGB.

API: POST /api/servo: action=move&angle=90 (całkowite 45–135), albo action=release. Rozszerzenie: move oraz home przyjmują opcjonalne
`speed=25`, `50` lub `100` (domyślnie 50). `action=zero` bez dodatkowych parametrów
zapisuje zero po zwolnieniu serwa, `action=home` wymaga istniejącego zera.
Pola stanu dodają servoZeroKnown, servoZero i servoHasCommand.
Błędne parametry: 400 bez zmiany. GET /api/state: servoActive, servoOK, servoAngle,
servoTarget. Te pola opisują program i sygnał, nie potwierdzają ruchu ani zasilania.

## Pierwszy test po montażu

1. Sprawdzić trzy końce przewodów, brak zwarć i brak obciążenia osi.
2. Przy ładowarce silników odłączonej włączyć ESP32 z USB lampki i wgrać
   wyłącznie ESP32-1: `~/.platformio/penv/bin/pio run -e esp32-1 -t upload`.
3. Po SUCCESS odświeżyć stronę, podłączyć ładowarkę silników, nie ruszać krokowca.
4. Środek 90°, następnie 80° i 100°. Nie przytrzymywać ramienia ręką.
5. Sprawdzić auto-wyłączenie sygnału, Zwolnij serwo, RGB/DHT oraz krokowiec.
   Pełna stabilność zasilania wymaga testu fizycznego; build tego nie dowodzi.

Testy host: servo_motion_test.cpp (zakres, rampowanie, impulsy, auto-wyłączenie,
zwolnienie, przepełnienie millis) oraz stepper_web_test.js (oba panele i blokady).

## Historia anulowanego eksperymentu

# SG90 — eksperyment anulowany

Aktualizacja 2026-11-14: użytkownik wrócił do planowania SG90 oraz silnika
krokowego 28BYJ-48. Poniżej historia wcześniejszego anulowania; nowa ocena
i inwentarz są w [HARDWARE.md](HARDWARE.md). Nie przywrócono jeszcze kodu napędów.

Użytkownik zrezygnował z serwa. Przygotowany kod i sterowanie SG90 na stronie
zostały usunięte; firmware z serwem nie został wgrany. Działająca niebieska LED
pozostaje w projekcie. Nie dodawaj ponownie SG90 bez nowego zlecenia.

## Zachowane ustalenia

- Etykieta użytkownika: Tower Pro TM / Microservo 9g / SG90, bez napisu 360°.
- Przewody: żółty/pomarańczowy, czerwony i brązowy; typowe funkcje opisano
  w [HARDWARE.md](HARDWARE.md), razem ze źródłami i ograniczeniami pewności.
- Nie wykonano testu ruchu ani pomiarów prądu serwa.
- Użytkownik ma ładowarkę USB-A 5 V / 2 A i nie chce dodatkowych adapterów
  na zaciski ani zasilania z Maca.
- Ostatni plan krótkiego testu przez pin 5V nie został potwierdzony jako wykonany.
  Nie traktuj go jako sprawdzonego sposobu zasilania rzeczywistej płytki.
- Jeśli przewody serwa zostały już podłączone, odłączyć je przy wyłączonym zasilaniu.

Oznaczenia płytki NodeMCU ESP-32S V1.1 i sprawdzony schemat zasilania zachowano
w [HARDWARE.md](HARDWARE.md); nie ma potrzeby ponownie pytać o te same napisy.

Weryfikacja przygotowania: testy host serwa i krokowca, symulowane API panelu
obu napędów i regresja obliczeń kolorów zakończone sukcesem. Build esp32-1
oraz esp32-2 zakończony sukcesem; obrazy mieszczą się w slocie OTA (około 86,6%).
Nie wykonano uploadu, testu HTTP na urządzeniu ani ruchu fizycznego SG90.

Użytkownik potwierdził działanie podstawowego SG90. Nowe ustawienia zera
i prędkości przeszły testy host i symulowanego API panelu; pamięć NVS
oraz powrót do zera po restarcie wymagają sprawdzenia na urządzeniu.
