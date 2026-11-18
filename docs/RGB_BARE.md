# Druga RGB — goła dioda 4-pin

Status: użytkownik rozpoznał kanały próbą świecenia z 3,3 V przez 1 kΩ.
Pierwsze próby przy nieprawidłowym umieszczeniu diody nie dawały światła;
po poprawieniu montażu potwierdzono poniższy pinout. Model/producent nieznany.
Po docelowym montażu i przekazaniu instrukcji uploadu użytkownik potwierdził
działanie. Nie wykonano niezależnego testu HTTP/OTA ani pomiaru prądów.

## Rozpoznanie i plan sygnałów

Numery 1–4 oznaczają kolejność nóżek przekazaną przez użytkownika, nie uniwersalny
widok z góry/spodu ani od płaskiej strony obudowy. Nie odwracać tej kolejności.

| Nóżka / długość | Rozpoznana funkcja | Docelowy sygnał |
|---|---|---|
| 1 — najkrótsza | B, świeci niebiesko | GPIO33 (P-12), przez własny 1 kΩ |
| 2 — długa | G, świeci zielono | GPIO32 (P-13), przez własny 1 kΩ |
| 3 — najdłuższa | wspólna katoda | GND wspólne z ESP32 |
| 4 — długa | R, świeci czerwono | GPIO17 (L-09), przez własny 1 kΩ |

Każda nóżka w osobnej grupie styków. Każdy kanał ma osobny rezystor 1 kΩ,
nie rezystor we wspólnej katodzie. Brak połączenia nowej RGB do +5 V.
GPIO są wyjściami logiki 3,3 V. Przy 1 kΩ prąd kanału jest ograniczony poniżej
3,3 mA nawet bez uwzględnienia spadku na LED; faktyczne prądy i jasności
niezmierzone. Dioda może być znacznie ciemniejsza od KY-016 i mieć inne proporcje
barw. Nie jest to błąd synchronizacji ustawień.

Montaż przy obu źródłach odłączonych. Przed docelowym połączeniem koniecznie
usunąć przewód testowy z węzła 3V3 DHT do wejścia rezystora. Nie łączyć tego
węzła równocześnie z wyjściem GPIO. Dokładne otwory i plan pojedynczych przewodów
są w lokalnej notatce breadboardu; działające DHT/RGB/napędy pozostają bez zmian.

## Zachowanie panelu

- Synchronizuj z lampką: kopiuje jej wynikowy RGB po nasyceniu/mapowaniu DHT,
  jasność, wyłączenie i wygaszenie podczas błędu DHT w auto.
- Ustaw niezależnie: własny kolor i jasność 0–100%, także przy wyłączonej
  pierwszej RGB albo błędzie DHT. Nie zmienia ustawień pierwszej lampki.
- Wyłącz drugą diodę: gasi wyłącznie nową RGB. Zachowuje własny kolor/jasność
  w RAM do ponownego zastosowania przyciskiem.
- Restart: synchronizacja, podczas startu obie lampki wygaszone. Ustawienia
  nowej diody nie są zapisywane w NVS.
- Gamma 2,2 i PWM 5 kHz / 8 bitów jak w pierwszej RGB. Kanały LEDC 3/4/5
  dla R/G/B, poza kanałami 0/1/2 pierwszej RGB i kanałem 6 serwa 50 Hz.
- Błąd PWM drugiej RGB gasi jej trzy wyjścia i pokazuje błąd na stronie,
  nie wyłącza pozostałych modułów. Stan API opisuje program, nie pomiar światła.

POST `/api/second`: `mode=sync`, `mode=off` albo
`mode=manual&color=%23ff0000&brightness=60`.
Walidacja pełnego żądania przed zmianą stanu; błędne parametry: HTTP 400.
GET `/api/state` dodaje secondMode, secondColor, secondManualColor,
secondBrightness (wyjściowa), secondManualBrightness (zapamiętana ręczna),
secondPwmOK. Stare pola API pozostają zachowane.

## Pierwszy test po podłączeniu

Przy ładowarce silników odłączonej włączyć ESP32 z lampki i wgrać wyłącznie
ESP32-1: `~/.platformio/penv/bin/pio run -e esp32-1 -t upload`.
Po SUCCESS odświeżyć stronę. W drugiej RGB ustawić niezależnie kolejno czerwony,
zielony i niebieski, potem sprawdzić jasność i Wyłącz drugą diodę. Sprawdzić
synchronizację z ręcznym kolorem i auto pierwszej lampki oraz jej wyłączenie.
Potwierdzić, że niezależny kolor nie zmienia KY-016. Sprawdzić też RGB przy
sterowaniu serwem (różne częstotliwości PWM) i kolejne OTA.
To plan testu fizycznego, nie zgłoszenie wykonania.

Weryfikacja przygotowania: testy UI z symulowanym API (sync, niezależny kolor,
wyłączenie, brak zmiany pierwszej lampki i błąd sieci) oraz istniejące testy
kolorów: OK. Build esp32-1 i esp32-2: SUCCESS, około 87,6% slotu aplikacji.
Upload i test fizycznej synchronizacji/kolorów po nowym montażu niewykonane.

Aktualizacja 2026-11-15: użytkownik potwierdził działanie po docelowym
podłączeniu i instrukcji uploadu. Wcześniejsze wzmianki o niewykonanym
teście opisują etap przygotowania; nie zgłoszono osobno każdego scenariusza UI.

## Uporządkowany panel i dopasowanie barw (2026-11-15)

Użytkownik zgłosił inną fizyczną barwę RGB 2 podczas auto RGB 1. Odczyt API
z agenta niedostępny (URLError), więc nie potwierdzono stanu na urządzeniu.
Kod sync kopiuje wynikowy RGB i jasność pierwszej diody także w auto.
Obie diody korzystają ze wspólnego LedOutput::duty (gamma 2,2); przy korekcji
100/100/100 wartości PWM tych samych kanałów są identyczne. Różnica barwy może
wynikać z charakterystyk diod/rezystorów albo montażu — przyczyna fizyczna
nieustalona. Przed korekcją sprawdzić czyste R/G/B: jeśli kolor jest zamieniony
lub wszystkie świecą przy zadaniu jednej barwy, sprawdzić połączenia.

Dodano rozwijane Dopasuj barwy RGB 2: R/G/B 0–100%, domyślnie 100% każdy.
Skaluje PWM po gamma tylko drugiej diody, zarówno sync, jak i manual.
Nie zwiększa prądu ponad bazowy PWM. Ustawienia sesyjne, reset po restarcie.
Przycisk Przywróć 100% usuwa korekcję. API POST /api/second-balance:
r=100&g=100&b=100. Walidacja trzech całkowitych 0–100 przed zmianą stanu.
GET stanu dodaje secondGainR/G/B. Podgląd HEX opisuje zadanie przed korekcją.

Domyślna jasność obu diod zmieniona z 60 na 100%, domyślny ręczny kolor RGB 2
jak RGB 1 (#0064ff). RGB 2 startuje w sync; obie pozostają wygaszone do
polecenia włączenia pierwszej lub ustawienia niezależnego koloru.
Układ strony: temperatura/wilgotność, grupa Diody RGB z osobną kartą dla każdej,
grupa Silniki z osobną kartą dla krokowca i serwa. Ręczne/auto ustawienia
pierwszej lampki zebrane w jej karcie. Desktop dwie kolumny, mobile jedna.

Weryfikacja: testy UI z symulowanym API i domyślnych 100%, korekcji i resetu OK;
test wspólnej konwersji PWM i ograniczania kanałów OK. Przegląd w przeglądarce
1280 px i 390 px: prawidłowa kolejność sekcji, brak poziomego przepełnienia
oraz błędów JavaScript. Oba środowiska firmware zbudowane poprawnie (87,9%
slotu aplikacji). Nie wgrano tej wersji; fizyczne barwy nie zostały skalibrowane.

Dodano jednorazowe efekty ruchu oraz efekty obu diod: [EFFECTS.md](EFFECTS.md).
Nowa wersja przygotowana, upload i test efektów na sprzęcie jeszcze niewykonane.

## Niebieski zamiast różu/fioletu — diagnostyka 2026-11-17

Użytkownik zgłosił, że RGB 2 pokazuje niebieski tam, gdzie RGB 1 daje róż/fiolet.
Przegląd kodu: kolejność R/G/B odpowiada potwierdzonemu pinoutowi, ten sam
LedOutput::duty liczy PWM obu diod. Kanały RGB 2 to 3/4/5, serwo używa 6.
Kod zainstalowanej biblioteki LEDC dobiera timer o pasującej częstotliwości
lub wolny timer; nie znaleziono programowego konfliktu częstotliwości.
Odczyt istniejącego firmware przez HTTP potwierdził pwmOK i secondPwmOK=true
oraz secondGainR/G/B=100/100/100. Odczyt wykonano podczas efektu Ogień,
nie podczas testu magenty. Te pola nie potwierdzają przepływu prądu przez LED.
Nie ustalono fizycznej przyczyny różnicy barwy i nie zmieniono domyślnej kalibracji.

Nowy panel „Sprawdź kolory obu diod” w karcie RGB 2 pozwala porównać:

| Przycisk | Kolor | Cel |
|---|---|---|
| Czerwony | #ff0000 | Sprawdzenie czerwonego toru |
| Zielony | #00ff00 | Sprawdzenie zielonego toru |
| Niebieski | #0000ff | Sprawdzenie niebieskiego toru |
| Magenta · R+B | #ff00ff | Jednoczesne pełne R i B, G wyłączony |
| Róż · więcej czerwieni | #ff0064 | Pełne R i osłabiony B |

Każdy przycisk kończy efekt, włącza RGB 1 w trybie ręcznym przy jasności
100% i nasyceniu 100%, ustawia RGB 2 w sync oraz usuwa jej korekcję
(przywraca 100/100/100). To trwała zmiana stanu bieżącej sesji do kolejnego
polecenia, nie chwilowy podgląd z automatycznym powrotem. Własny kolor
i jasność RGB 2 pozostają zapamiętane. Silniki nie dostają poleceń.

POST /api/light-test: color=0..4 według kolejności tabeli; błędne lub dodatkowe
parametry dają 400 przed zmianą stanu. GET /api/state dodaje firstDuty
oraz secondDuty: tablice R/G/B wartości 0–255 po jasności/gamma i korekcji,
zaakceptowanych przez ledcWrite. Po wykrytym błędzie wyjścia są gaszone,
a odpowiednia tablica zerowana. To stan programu, nie pomiar elektryczny.
W teście magenty obie tablice powinny wynosić [255,0,255]. Gamma nie osłabia
pełnej czerwieni w tej próbie; dla pośrednich wartości kanałów jest wspólna.

Jeśli czysta czerwień nie świeci lub daje inną barwę, potrzebne jest sprawdzenie
czerwonego toru (połączenia, rezystor, dioda, GPIO), bez zgadywania usterki.
Jeśli czyste R/G/B są poprawne, a magenta ma nadmiar niebieskiego, można
zmniejszać secondGainB przy secondGainR=100 i obserwować wynik. Bez obserwacji
nie dobieramy arbitralnej korekcji. Testy kodu/UI i oba buildy: OK; upload
nowej wersji i optyczna weryfikacja barw niewykonane.
