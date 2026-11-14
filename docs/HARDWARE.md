# Sprzęt i okablowanie

## Inwentarz

| Element | Wiadomo | Do ustalenia przed podłączeniem |
|---|---|---|
| ESP32-1 | PCB z napisami NodeMCU / ESP-32S / V1.1; OTA esp32-1 | wydajność toru 5V pod obciążeniem serwa |
| ESP32-2 | użytkownik potwierdził identyczność płytek; OTA esp32-2 | aktualny montaż i przewody osobno od ESP32-1 |
| Zasilacz USB | 5 V, 2 A | marka/model, stan i parametry dodatkowych obciążeń |
| Kabel | cały USB Micro-B | do uploadu USB wymagana obsługa danych |
| Płytka stykowa | oznaczone rzędy | orientacja i ciągłość szyn zasilających |
| Niebieska LED | dwunóżkowa; działa na ESP32-1 przez stronę WWW; GPIO23 i rezystor 1 kΩ | dokładny typ i parametry optyczne nieustalone |
| Rezystory | 220 Ω; 1 kΩ (1000 Ω); 10 kΩ (10 000 Ω) — potwierdzone przez użytkownika | moc znamionowa i tolerancja |
| Serwo | Tower Pro TM / Microservo 9g / SG90; bez napisu 360°; trzy przewody opisane poniżej | zasilanie i adapter, rzeczywisty zakres i pobór prądu egzemplarza |
| Silnik krokowy | napis 28BYJ-48 / 5VDC / 15031801; pięć przewodów w białym gnieździe płytki ULN2003AN widoczne na zdjęciu | oporność uzwojeń/prąd i zasilanie; brak potwierdzonego połączenia z ESP |
| Płytka sterownika silnika | ULN2003AN; diody A/B/C/D; 4 wlutowane wejścia IN1–IN4; 4 dolne piny, dwa oznaczone − i +, nadruk 5--12V; IN5–IN7 to puste pola | funkcja dwóch pozostałych dolnych pinów/ewentualnej zworki i zgodność poziomów sterujących |
| Fotorezystor | użytkownik potwierdził dwie nóżki i zygzak na powierzchni; podejrzewa GL5516 | dokładny model niepotwierdzony; brak potwierdzonego montażu |
| Czujnik DHT11 | mała płytka, 3 piny; użytkownik podał S po jednej stronie i − po drugiej; środek zasilanie wg referencji KY-015 | obecność i wartość pull-up; test rzeczywistych pomiarów |
| Dioda RGB LED | 5 mm, 4-pin — potwierdzone przez użytkownika | wspólna anoda czy katoda, kolejność wyprowadzeń, parametry i rezystory dla kanałów R/G/B |
| Moduł RGB LED KY-016 | użytkownik potwierdził B G R − i trzy rezystory 151 (150 Ω); typ ze wspólną katodą | test kolorów i rzeczywistej jasności |
| Matryca LED 8×8 | użytkownik podał 1588AS, 16 nóżek, po 8 na stronę; bez MAX7219 | producent, polaryzacja i mapa wyprowadzeń egzemplarza, rozstaw nóżek, projekt ograniczenia prądu i odświeżania |
| Tranzystory | użytkownik ma prawdopodobnie 3 sztuki; napisy C3225 oraz 2 H | liczba sztuk, zgodność obudowy i wyprowadzeń z 2SC3225; nie podłączone |
| Inne moduły Arduino | różne elementy | oznaczenia i dokumentacja |

Adresy IP nie są częścią publicznej dokumentacji; lokalny spis jest w .local/.
ESP-WROOM-32 oznacza moduł radiowy, nie jednoznaczny model całej płytki.
Profil nodemcu-32s jest zachowany z istniejącego projektu; identyfikacja PCB poniżej
pochodzi z napisów przepisanych przez użytkownika, nie z samego profilu.
ESP32-1 jest na stałe w breadboardzie i jego położenie się nie zmienia.
Użytkownik wcześniej potwierdził działanie zewnętrznej LED. Teraz potwierdził
usunięcie poprzednich przewodów, LED i rezystora; zostaje samo ESP32-1.
Nowy eksperyment: [DHT11 + KY-016](CLIMATE_RGB.md), bez serwa.
Dokładne otwory i rozróżnienie planu od montażu są w `.local/BREADBOARD.md`.

## Identyfikacja sprzętu — zachowaj dla kolejnych agentów

Ustalenia z 2026-11-13. Nie pytaj ponownie o te same napisy ani nie szukaj
ponownie schematu bez nowej sprzeczności lub konkretnej brakującej informacji.

### Płytka ESP32-1

- Spód, zapis użytkownika: `node mcu`, `esp-32s`, `v1.1`, `101010`.
- Góra: przycisk opisany przez użytkownika jako `Ioo` / BOOT (IO0) oraz `EN` / reset.
- Na metalowej osłonie modułu użytkownik odczytał m.in. `211-161007`.
  Samo to oznaczenie nie ustala wydajności zasilania płytki.
- Napisy odpowiadają rodzinie NodeMCU-32S V1.1. Nie potwierdzają producenta
  rzeczywistego egzemplarza ani zgodności wszystkich elementów ze schematem.
- Mapa P/L obu płytek jest potwierdzona przez użytkownika: [PINOUT.md](PINOUT.md).
  Nowe oznaczenia nie zmieniają tej mapy ani zapisanego położenia breadboardu.

Sprawdzono wizualnie sekcję POWER w **schemacie Ai-Thinker NodeMCU-32S V1.1,
2016/10/10** (strona 1):

```text
USB (VDDUSB) → dioda D1 → VDD5V → pin 5V oraz wejście stabilizatora AMS1117-3.3
```

D1 jest opisana `1N5819/SS14/S4(SOD_323)`. Zatem w schemacie referencyjnym
pin 5V jest zasilany z USB przez diodę; nie jest wyjściem stabilizatora 3,3 V.
Nie oznacza to zatwierdzonej wydajności całego toru USB, diody i ścieżek
rzeczywistego egzemplarza dla prądu serwa. Nie zmierzono napięcia pod obciążeniem.

Źródło: [schemat producenta Ai-Thinker V1.1, kopia PDF](https://hk.botsheet.com/wp-content/uploads/2021/04/nodemcu_32s_schematic_v1.1.pdf).
Schemat wystarcza do ustalenia kierunku zasilania w projekcie referencyjnym;
nie trzeba ponownie prosić użytkownika o numery elementów, by ustalić ten fakt.

### Serwo użytkownika

- Etykieta: `Tower Pro TM`, `Microservo 9g`, `SG90`.
- Użytkownik potwierdził brak `360°` i `continuous rotation`.
  Przygotowany program dotyczy wersji pozycyjnej; brak nadruku nie jest pomiarem zakresu.
- Przewody: żółty/pomarańczowy (typowo sygnał), czerwony (zasilanie), brązowy (masa).
- Użytkownik nie ma osobnego zasilania serwa. Posiadanie adaptera lub modułu
  zasilania breadboardu nie zostało potwierdzone; nie zakładaj ich obecności.
- SG90 nie jest jeszcze potwierdzone jako podłączone ani przetestowane.
- Dane producenta, nie pomiar egzemplarza: zasilanie 4,8 V; w odpowiedzi
  producenta dotyczącej wersji cyfrowej 4,8–6 V i prąd około 0,5–2 A.
  Nie stosuj tej wartości jako zmierzonego prądu rozruchu lub zatrzymania użytkownika.
- Dokumentacja producenta nie podaje jednoznacznego progu wejścia sygnałowego;
  działanie z sygnałem ESP32 3,3 V pozostaje do potwierdzenia dla egzemplarza.

Źródła: [Tower Pro SG90 Analog](https://towerpro.com.tw/product/sg90-analog/),
[SG90 Digital i odpowiedzi producenta](https://towerpro.com.tw/product/sg90-7/).
Użytkownik wcześniej zrezygnował z serwa; przygotowany kod usunięto bez uploadu.
2026-11-14 wrócił do tematu: prosi o ocenę uruchomienia SG90 razem z 28BYJ-48,
z możliwością wyboru tylko jednego silnika. Na razie planowanie, bez nowego kodu,
montażu i testu ruchu. Zachowano [historię eksperymentu](SERVO_WEB.md).

### Silnik krokowy 28BYJ-48 (2026-11-14)

- Dokładny napis przepisany przez użytkownika: `step motor 28byj-48 5vdc 15031801`.
- To silnik krokowy, nie serwo pozycyjne. Nie przypisuj numerowi 15031801
  znaczenia daty/wersji bez dokumentacji producenta egzemplarza.
- Typowe zestawy mają silnik unipolarny z przekładnią oraz sterownik ULN2003.
  Użytkownik potwierdził posiadanie płytki z czterema diodami i układem
  `19cxy8k e4 uln2003an` (zapis użytkownika). ULN2003AN identyfikuje układ;
  pozostałym napisom nie przypisano znaczenia bez dodatkowej dokumentacji.
  Nie pytaj ponownie o napis na tym układzie.
  Użytkownik potwierdził oznaczenia diod A/B/C/D, pinów zasilania − i + oraz
  napis pod nimi `5vv12v`. Zapis przypomina zakres 5–12 V płytki; nie potwierdza
  obecności stabilizatora. Silnik ma wyraźne 5VDC — nie zasilać go napięciem 12 V.
  Na zdjęciu użytkownika z 2026-11-14 widać:
  - cztery wlutowane piny IN1, IN2, IN3, IN4;
  - IN5, IN6, IN7 jako nieobsadzone pola lutownicze, nie wystające piny;
  - białe gniazdo z pięcioprzewodową wtyczką silnika;
  - diody A/B/C/D oraz dolne złącze czteropinowe; pod pierwszymi dwoma
    (od strony oznaczeń IN) nadrukowano − i +; poniżej 5--12V;
  - zdjęcie nie wystarcza do potwierdzenia funkcji pozostałych dwóch dolnych
    pinów i połączeń ewentualnej zworki. Nie każ ich zwierać ani zdejmować
    zworki bez sprawdzenia obwodu. Potwierdzone − i + są punktami zasilania.
  Nie pytaj ponownie o liczbę wejść IN ani liczbę przewodów w widocznym wtyku.
- Nie podłączać uzwojeń do GPIO ani 3V3. Potrzebny jest odpowiedni stopień
  sterujący i zasilanie silnika 5 V ze wspólną masą ESP32.
- Z typowym modułem IN1–IN4: cztery GPIO, SG90 wymaga dodatkowo jednego.
  Nie przydzielono jeszcze konkretnych pinów; nie zmieniać działającego DHT11/RGB.
- Sterowanie programowe musi być nieblokujące, by zachować HTTP/OTA i odczyty.
  DHT11 krótko blokuje pętlę podczas odczytu; uwzględnić to w regularności kroków.
- Pozycja krokowca wynika z liczby kroków. Po restarcie nie zna bezwzględnego
  położenia; mechanizm wymagający powtarzalnego zera potrzebuje bazowania/czujnika.
- Zasilanie dwóch napędów naraz nie zostało zweryfikowane. Sama obecność
  ładowarki 5 V / 2 A nie potwierdza wydajności toru USB/pinu 5V płytki.
- Porównanie zasilania (2026-11-14): referencyjny 28BYJ-48 5 V ma uzwojenia
  50 Ω ±7%, co daje orientacyjnie 100 mA na fazę przed uwzględnieniem spadku
  napięcia w ULN2003. Przy jednej/dwóch aktywnych fazach orientacyjnie 100–200 mA,
  nie licząc drobnego poboru płytki. To obliczenie dla wersji referencyjnej,
  nie pomiar użytkownika — warianty uzwojeń mogą się różnić.
  Krokowiec jest z tego powodu lepszym kandydatem do pierwszego testu zasilania
  niż SG90 z silnie zależnym od obciążenia prądem. Nie zatwierdza to automatycznie
  zasilania silnika przez płytkę ESP; przy postoju aktywne cewki nadal pobierają prąd.
  [Referencyjna karta 28BYJ-48 5 V / 50 Ω](https://s3.amazonaws.com/snapeda/datasheet/STEP_MOTOR_5V_%2828BYJ-48-5V%29_mikroElektronika.pdf).

Źródła referencyjnych zestawów, nie identyfikacja producenta egzemplarza:
[Olimex 28BYJ-48 + ULN2003](https://www.olimex.com/Products/Robot-CNC-Parts/StepperMotors/SM-5VDC-DRV/),
[SunFounder 5 V, 5 przewodów / 4 fazy](https://www.sunfounder.com/products/dc-5v-stepper-motor-28byj-48-uln2003-drive-module-5-line-4-phase),
[TI ULN2003A — sterownik obciążeń indukcyjnych](https://www.ti.com/product/ULN2003A).

## Mapa pinów stanowiska

### Nowe elementy: 1588AS i C3225 (2026-11-13)

Matryca nie jest jeszcze podłączona. Nie traktuj nadruku 1588AS jako dowodu
zgodności pinoutu z dowolnym znalezionym schematem. Znaleziono referencyjną
[kartę TOPLITE TOP-CC-1588AS](https://www.ledtoplite.com/uploadfile/2017/1588/TOP-CC-1588AS.pdf),
ale producent egzemplarza użytkownika pozostaje nieznany. Układ do pełnego
sterowania wymaga projektu prądowego; nie podłączać 16 pinów bezpośrednio
do ESP z pominięciem rezystorów i oceny prądu wspólnych linii.

W [karcie Toshiba 2SC3225](https://media.digikey.com/pdf/Data%20Sheets/Toshiba%20PDFs/2SC3225_2009-12-21.pdf)
oznaczenie obudowy to C3225. To mocna przesłanka identyfikacji, nie sprawdzenie
egzemplarzy użytkownika. Referencja: NPN, zastosowania przełączające, obudowa
TO-92MOD; wartości absolutnie maksymalne 40 V, 2 A, 900 mW nie są zalecanym
punktem pracy. Nie podawano instrukcji podłączania wyprowadzeń tranzystorów.
Trzy sztuki nie tworzą typowego ośmiokanałowego stopnia sterującego wierszami.

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

Na ESP32-1 użytkownik wykonał ten eksperyment i potwierdził działanie strony oraz
LED. Używa rezystora 1 kΩ; poniższy schemat pozostaje ogólnym odniesieniem.
Przy 3,3 V i 1 kΩ górna granica prądu wynosi 3,3 mA, a rzeczywisty prąd LED
jest mniejszy ze względu na jej napięcie przewodzenia. Usunięcie tej LED nie
rozwiązuje kwestii wydajności zasilania serwa. Ten montaż został później usunięty
przez użytkownika przed eksperymentem DHT11 + KY-016; opis poniżej jest historyczny.

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

Użytkownik podał model czujnika: **DHT11**, na małej płytce z trzema pinami S / środek / −.
Nie pytaj ponownie o model ani postać modułu. Układ środkowego zasilania
zweryfikowano dla referencyjnego KY-015; obecność pull-up na egzemplarzu pozostaje nieustalona. Potem sprawdź dokumentację zasilania i poziomu sygnału,
przygotuj tabelę połączeń i kod. Pull-up linii ESP32 powinien mieć napięcie
zgodne z logiką 3,3 V.

Użytkownik ma też dwa osobne elementy RGB: **diodę LED 5 mm, 4-pin** oraz
**moduł KY-016**. Nie utożsamiaj ich pinoutu ani nie zakładaj obecności rezystorów
w module bez sprawdzenia. Dodanie do inwentarza nie oznacza wykonanego montażu.

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
