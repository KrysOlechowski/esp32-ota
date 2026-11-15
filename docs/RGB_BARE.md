# Druga RGB — goła dioda 4-pin

Status: użytkownik rozpoznał kanały próbą świecenia z 3,3 V przez 1 kΩ.
Pierwsze próby przy nieprawidłowym umieszczeniu diody nie dawały światła;
po poprawieniu montażu potwierdzono poniższy pinout. Model/producent nieznany.
Kod i plan docelowy przygotowane; upload i sterowanie trzema kanałami
jednocześnie jeszcze niepotwierdzone.

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
