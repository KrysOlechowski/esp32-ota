# Efekty LED i ruchu

Przygotowane programowo na istniejącym ESP32-1: bez zmian przewodów.
Nowa wersja wymaga uploadu i testu fizycznego. Po restarcie żaden efekt
nie uruchamia się samoczynnie. OTA i odczyty DHT pozostają obsługiwane w pętli.

## Światło

Na samej górze strony karta Efekty świetlne:
- Świeczka: łagodne, nieregularne migotanie ciepłego koloru.
- Ogień: mocniejsza zmienność czerwieni/pomarańczu.
- Oddychanie: spokojne rozjaśnianie/ściemnianie niebieskiego (cykl 4 s).
- Tęcza: płynny obieg barw na obu RGB (cykl 12 s).
- Duet: płynne, dopełniające barwy obu RGB.

Obliczenia z czasu, bez delay i pętli oczekujących. Aktualizacja około 30 Hz.
Efekt nadpisuje oba wyjścia także gdy ich zapisany tryb jest wyłączony lub
niezależny; Wyłącz efekt przywraca poprzednie kolory i tryby. Jasność pochodzi
z RGB 1 i może być zmieniana podczas efektu przez Zastosuj jasność.
Korekcja barw RGB 2 nadal działa. Symulacja światła, nie obraz płomienia.
Zastosowanie zwykłego koloru, auto/off pierwszej lampki lub trybu RGB 2 kończy
efekt. Zmiana samych zakresów DHT lub korekcji RGB 2 nie kończy efektu.
Błąd DHT w auto nie gasi aktywnego efektu; po jego wyłączeniu znów obowiązuje
normalne wygaszanie auto przy błędzie sensora.

POST /api/light-effect: effect=0..5 (off/candle/fire/breathe/rainbow/duet),
opcjonalnie brightness=0..100. Walidacja całości przed zmianą.
GET /api/state: lightEffect identyfikator 0..5. Pozostałe pola kolorów pokazują
aktualny wynik efektu; zapisane tryby i kolory ręczne nie są nadpisywane.

## Ruch — osobno na górze każdej karty

Krokowiec, stałe 100 półkroków/s:
- Wahadło: +512, −1024, +512 półkroków (powrót do logicznego początku).
- Taniec: +128, −128, +256, −256, +128, −128.
- Karuzela: cztery etapy po 1024 półkroki w jedną stronę, nominalnie obrót.

Serwo, stałe 50 jednostek umownej skali/s w już używanym zakresie:
- Machanie: 60, 120, 60, 120, 90.
- Kiwanie: 80, 100, 80, 100, 90.
- Skanowanie: 60, 75, 90, 105, 120, 105, 90, 75, 60, 90.

Przycisk Uruchom raz wykonuje pojedynczą sekwencję. Zapętl powtarza ją do Stop
lub osiągnięcia dotychczasowego limitu całej sesji 2 minut. Limit nie jest
zerowany po cyklu. Ponowne uruchomienie wymaga jawnego polecenia. Kolejny etap zaczyna się po zakończeniu
poprzedniego i 600 ms pauzy; serwo najpierw samo wyłącza sygnał jak w zwykłym
sterowaniu. Zapętlanie jest opcjonalne, tylko po jawnym poleceniu przyciskiem Zapętl. Limit całej sekwencji 120 s,
sprawdzany w pętli. Nie zastępuje zabezpieczenia sprzętowego na zawieszenie.

Przez całą sekwencję (także pauzy) drugi napęd i zwykłe polecenia ruchu są
blokowane (409). Zatrzymaj efekt i dotychczasowe Stop/Zwolnij serwo kończą
sekwencję danego napędu i wyłączają jego sygnał. Utrata Wi-Fi oraz początek
OTA zatrzymują oba napędy i kasują sekwencję. Nie odłączają zasilania fizycznie.
Nie zmieniają zapisanego zera serwa ani nie ustawiają automatycznie zera
krokowca. Pozycje serwa są umowne, bez kalibracji i pomiaru osi. Kąty krokowca nominalne. Pierwszy test bez obciążenia,
z wolnym miejscem na ruch. Po przerwaniu nie obiecujemy powrotu do początku.

POST /api/motion-effect: device=motor|servo oraz effect=swing|dance|tour dla
krokowca lub wave|nod|scan dla serwa; effect=stop zatrzymuje wskazany napęd.
GET /api/state: motionEffect = 0 brak, 1 swing, 2 dance, 3 tour, 4 wave, 5 nod,
6 scan. Sekwencja wymaga jawnego polecenia start i nie jest zapisywana w NVS.
Opcjonalny parametr loop=0|1 wybiera pojedynczy cykl lub pętlę (domyślnie 0);
parametr nie jest dozwolony w stop. Inne wartości są odrzucane przed zmianą.
Stan dodaje motionLoop. Stop, rozłączenie Wi-Fi i OTA nadal kasują sekwencję.

## Układ i weryfikacja

Wybór koloru przeniesiono bezpośrednio pod nagłówki obu kart RGB. Na desktopie
pola wyboru barwy są na tej samej wysokości. Efekty światła nad DHT, efekty
ruchu na górze kart napędów. Mobile zachowuje jedną kolumnę.

Testy host: płomienie/oddech/tęcza/duet, czas po przepełnieniu millis,
integracja wszystkich sześciu sekwencji z modelami napędów, liczba etapów,
koniec ruchu, logiczny powrót krokowca, końcowe 90° serwa, limit czasu i Stop.
Test symulowanego API strony: start/stop efektów i blokady przycisków.
Przegląd przeglądarki: 1280/390 px, równa wysokość pól koloru na desktopie,
brak poziomego przepełnienia i błędów JavaScript. Testy sprzętowe niewykonane.

Build końcowy esp32-1 i esp32-2: SUCCESS, 89,1% slotu aplikacji. Test host
nowych efektów przechodzi również z -Wall -Wextra -Werror. Upload i test
rzeczywistych diod/napędów tej wersji niewykonane.

Test rozszerzenia pętli obejmuje wszystkie 6 efektów: więcej niż jeden cykl,
koniec po 120 s oraz przyciski Loop/Stop obu napędów i blokady w UI.
Nowa wersja wymaga uploadu i testu na urządzeniu.
