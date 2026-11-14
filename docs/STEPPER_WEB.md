# Krokowiec 28BYJ-48 5 V + ULN2003

Status: użytkownik potwierdził montaż, upload i obrót silnika oraz świecenie diod
ULN2003 na ESP32-1. Początkowy brak działania ustąpił po założeniu mostka
zasilania między dwoma dodatkowymi pinami. DHT11 + KY-016 pozostają w układzie.
To potwierdzenie użytkownika, bez niezależnego pomiaru napięć lub położenia osi.
SG90 oraz przekaźnik nie są częścią eksperymentu.

## Sygnały i zasilanie

| ESP32 | Pozycja P/L | ULN2003 | Rola |
|---|---|---|---|
| GPIO19 | L-12 | IN1 | faza 1 |
| GPIO21 | L-14 | IN2 | faza 2 |
| GPIO22 | L-17 | IN3 | faza 3 |
| GPIO23 | L-18 | IN4 | faza 4; stara niebieska LED usunięta |
| GND | P-06 | −, wspólne z minusem ładowarki | odniesienie sygnałów |

To wyjścia 3,3 V, bez pinów startowych, flash i wejściowych.
DHT pozostaje na GPIO18, RGB na GPIO25/26/27. Szczegółowe proponowane otwory
są w lokalnej notatce breadboardu; użytkownik potwierdził wykonanie montażu.

- USB lampki `OUT 5V/1A` → cały kabel → USB ESP32 z DHT/RGB.
- Osobna ładowarka 5 V / 2 A → posiadany kabel → oznaczone +/− ULN2003.
- Minus ładowarki, minus ULN2003 i GND ESP32 mają wspólne połączenie.
- Nie łączyć plusów źródeł. Nie prowadzić zasilania silnika przez pin 5V,
  3V3 ani GPIO ESP32. Prąd silnika wraca do ładowarki bezpośrednio z ULN2003.
- Silnik pozostaje w białym pięciopinowym gnieździe. Na rozpoznanym egzemplarzu
  dwa dodatkowe piny wymagają mostka zasilania silnika/wskaźników. Użytkownik
  oznaczył je na zdjęciu spodu dwiema zielonymi kropkami (nad czerwonym +
  i brązowym − w orientacji tego zdjęcia). Po połączeniu tej pary potwierdził
  obrót i świecenie diod. Mostek pozostaje w docelowym układzie; nie zwierać + z −.
  Nie przenosić tego ustalenia na inne warianty PCB bez sprawdzenia.
- Używać oznaczeń na PCB, nie samego położenia lub kolorów.
- Wszystkie przewody wkładać przy obu źródłach odłączonych. Połączenia zasilania
  muszą być stabilne, zaizolowane i zabezpieczone przed wyrwaniem; taśma
  przytrzymująca sondy pomiarowe nie jest gotowym złączem zasilania.

Posiadany kabel był wcześniej używany przez użytkownika do zasilania odsłoniętymi
żyłami: czerwony plus, czarny minus. Napięcie nie zostało wiarygodnie zmierzone;
DT830BUZ ma wskaźnik słabej baterii. Plan opiera się na oznaczeniu ładowarki 5 V
oraz potwierdzonej historii kabla. Nie traktować jako pomiaru ani testu zasilacza.

Referencyjny 28BYJ-48 5 V / 50 Ω pobiera orientacyjnie 100–200 mA dla jednej/dwóch
faz przed uwzględnieniem spadku napięcia sterownika; rzeczywisty egzemplarz
niezmierzony. TI podaje dla ULN2003A maks. napięcie wejścia włączającego 2,4 V
przy 200 mA i VCE=2 V (25°C). Uzasadnia to próbę sterowania 3,3 V przy małym
obciążeniu faz; nie jest pomiarem ani gwarancją parametrów całej płytki.
Źródło: [TI ULN2003A, tabela 5.6](https://www.ti.com/lit/ds/symlink/uln2003a.pdf).
Oszacowanie uzwojeń i identyfikacja modułu: [HARDWARE.md](HARDWARE.md).

## Sterowanie

- Lewo/Prawo: wybór kąta 1–360° (domyślnie 90°), odstęp zależny od prędkości: 20/10/5 ms.
  Osobne przyciski pełnego obrotu w obu kierunkach. Nominalnie 4096 półkroków
  na obrót, około 82/41/20,5 s dla 50/100/200 półkroków/s. To wartość przybliżona dla przekładni nominalnie 1:64;
  dokładny obrót egzemplarza wymaga kalibracji. Nie obiecujemy dokładnego kąta.
- Ustaw tutaj zero: bieżące położenie jako umowne 0°, bez ruchu. Dostępne na postoju.
- Wróć do zera: najkrótsza droga do umownego zera w obrębie obrotu, bez odtwarzania
  liczby wcześniejszych pełnych obrotów. Pozycja pokazywana modulo 360°.
- Brak enkodera i czujnika bazowania: program liczy wysłane kroki, nie mierzy osi.
  Restart i test A–D kasują ustawienie zera. Ręczne poruszenie, luz przekładni
  lub zgubienie kroków unieważniają fizyczne odniesienie; ustawić zero ponownie.
  Przy wyłączonych cewkach nie ma elektrycznego trzymania osi.
- Test A–D: po początkowych 0,5 s włącza kolejno IN1, IN2, IN3, IN4,
  każde osobno na co najmniej 0,5 s, następnie wyłącza wszystkie wyjścia.
  Może lekko poruszyć silnikiem; to próba wyjść, nie test obrotu.
  ULN2003 nie zwraca informacji o silniku ani zasilaniu, więc program nie
  potwierdza połączenia, prądu ani ruchu. Obserwować fizyczne diody A–D.
- Stop: natychmiastowe wyłączenie czterech wyjść przy obsłużeniu żądania.
- Cewki wyłączone po ruchu, przy restarcie, utracie Wi-Fi i rozpoczęciu OTA.
  Nie ma elektrycznego podtrzymania pozycji podczas postoju.
- Nowy ruch podczas aktywnego ruchu jest odrzucany (HTTP 409).
- Ochronny limit czasu: większa z wartości 5 s i czas zadanych kroków przy wybranej prędkości + 2 s,
  sprawdzany w pętli. Nie zastępuje zabezpieczenia
  sprzętowego i nie zadziała podczas całkowitego zawieszenia procesora.
- DHT nadal odczytywany co 2,5 s; jego krótka blokada może chwilowo wydłużyć krok.
  Brak gwałtownego nadrabiania kroków po przerwie. HTTP i OTA pozostają w pętli.
- Status pokazuje stan programu, nie fizyczny ruch ani podłączenie silnika.
- Po restarcie nie znamy pozycji osi. Pierwszy test bez obciążenia, bez ruchu auto.

API: POST `/api/motor` z `action=left`/`right` i opcjonalnym `degrees=1..360`. Bez kąta zachowany
jest dawny ruch 128 półkroków dla zgodności. Pozostałe akcje (bez dodatkowych
parametrów): `stop`, `test`, `zero`, `home`. Home bez ustawionego zera daje 400.
Nieprawidłowe parametry: HTTP 400, bez zmiany ruchu.
GET `/api/state` dodaje `motorRunning`, `motorDirection` (−1, 0, 1)
i `motorDiagnostic` (aktywny test A–D), `motorZeroKnown`, `motorAngle`
(szacowany kąt modulo obrót; znaczenie względem zera tylko przy motorZeroKnown).
Strona odświeża stan co sekundę; Stop pozostaje dostępny podczas innych żądań.

## Uruchomienie po montażu

1. Sprawdzić brak zwarć i zgodność każdego końca z tabelą. Nie używać niepewnych
   połączeń, nie zakładać ciągłości bocznych szyn breadboardu.
2. Najpierw włączyć ESP32 z lampki; potem osobne 5 V silnika. Bez kliknięcia
   ruchu wyjścia faz pozostają wyłączone w nowym firmware.
3. Po potwierdzonym montażu wgrać wyłącznie ESP32-1 w systemowym Terminalu,
   z katalogu projektu: `~/.platformio/penv/bin/pio run -e esp32-1 -t upload`.
   Zachowane lokalne hasło musi odpowiadać obecnemu firmware.
4. Sprawdzić tożsamość strony, RGB i DHT. Kliknąć raz Prawo: krótki ruch i stop,
   następnie Lewo. Sprawdzić Stop podczas ruchu i brak resetu ESP32.
5. Gdy silnik tylko drży albo ESP resetuje się, wyłączyć zasilania i sprawdzić
   przewody oraz zasilanie; nie podnosić napięcia na silniku 5 V.
6. Sprawdzić kolejne OTA oraz zatrzymanie na początku aktualizacji.

Testy na komputerze:

```sh
c++ -std=c++11 -Iinclude test/host/stepper_motion_test.cpp -o /tmp/stepper-motion-test
/tmp/stepper-motion-test
node test/host/stepper_web_test.js
pio run -e esp32-1 -e esp32-2
```

Weryfikacja przygotowania: build `esp32-1` i `esp32-2` zakończony sukcesem;
oba obrazy mieszczą się w slocie OTA (około 85,9% flash aplikacji).
Test sekwencji obu kierunków, Stop, automatycznego końca ruchu, limitu czasu,
braku nadrabiania oraz przepełnienia millis(): OK. Test przycisków z symulowanym
API (w tym Stop przy zajętej obsłudze lampki i błąd sieci): OK.
Dotychczasowy test mapowania kolorów: OK. To nie test fizycznego sterownika,
HTTP na urządzeniu ani aktualizacji OTA; upload nie został wykonany.

Diagnostyka po zgłoszeniu pierwszej próby: użytkownik widzi potwierdzenie ruchu
w UI, ale diody A–D nie świecą i silnik nie wydaje dźwięków. Wskazuje to m.in.
na możliwy brak zasilania, masy lub sygnałów; nie ustalono jednej przyczyny.
Użytkownik wsunął odsłonięte żyły USB bezpośrednio do breadboardu; oględziny
nie potwierdzają kontaktu elektrycznego. Dodano test A–D do obserwacji wyjść.

Wynik uruchomienia: użytkownik potwierdził obrót silnika i świecenie diod
po połączeniu dwóch dodatkowych pinów mostkiem. Nie wykonano niezależnego
testu HTTP, dokładności ruchu, Stop ani kolejnego OTA po tej zmianie.

Stan końcowy: użytkownik potwierdził działanie sterowania, usunięcie dodatkowej
niebieskiej LED diagnostycznej z rezystorem i przewodami oraz przywrócenie
bezpośredniego GPIO19 → IN1. Ponowny test po usunięciu kontrolki: działa.
Mostek zasilania ULN2003 pozostaje. Przycisk Test A–D w programie nadal
steruje diodami modułu; nie wymaga osobnej kontrolki.

Rozszerzenie kąta i zera: przygotowane bez zmiany przewodów; upload i test
na silniku tej wersji nadal do wykonania. Testy host obejmują pełne 4096 kroków
bez przedwczesnego timeoutu, zawijanie kąta, najkrótszy powrót do zera, Stop,
wyzerowanie i unieważnienie zera w teście A–D.

Wybór prędkości: Wolno/Normalnie/Szybko = 50/100/200 półkroków/s,
domyślnie 100. API left/right/home przyjmuje opcjonalne `speed=50`, `100`
lub `200`; inne wartości odrzucane przed zmianą stanu. Stop/test/zero nie
przyjmują speed. Wybór z UI dotyczy następnego ruchu, także powrotu do zera.
Brak rampy przyspieszenia; jeżeli obciążenie powoduje gubienie kroków, wybrać
niższą prędkość i ustawić ponownie umowne zero. Nowa wersja wymaga uploadu
i sprawdzenia na silniku; okablowanie bez zmian.
