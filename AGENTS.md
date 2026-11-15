# Instrukcje dla GPT / Codex

## Cel i język

To projekt testowy PlatformIO dla dwóch ESP-WROOM-32 i eksperymentów z modułami.
Rozmawiaj z użytkownikiem po polsku. README na GitHub utrzymuj po angielsku.
Dokumentację pracy, sprzętu i instrukcje dla agentów pisz po polsku.
Wystarczy ten plik i docs/AI_WORKFLOW.md: osobny skill nie jest obecnie wymagany.

## Przed pracą

1. Przeczytaj platformio.ini, zmieniane źródła oraz odpowiednią dokumentację docs/.
2. Sprawdź git status; zachowaj cudze zmiany. Nie resetuj repo ani firmware urządzeń.
3. Dla informacji o stanowisku przeczytaj .local/STANOWISKO.md, jeśli istnieje.
   Jego treści, IP i ścieżek nie przenoś do plików publicznych.
4. Nie otwieraj sąsiednich projektów: nie są częścią tego repozytorium.
5. Nie wypisuj secrets.local.json ani kopii .private-backup, nawet w logach narzędzi.
   Gdy potrzebujesz odczytać sekret programowo, użyj scripts/project_config.py.

## OTA — obowiązkowo zachowaj

- OtaService::begin() w setup po połączeniu Wi-Fi; OtaService::handle() w loop.
- src/ota_service.cpp i include/ota_service.h to wydzielona infrastruktura.
- Zachowaj partycje obsługujące OTA (obecnie default.csv), hasło i tożsamość urządzenia.
- Nie usuwaj wspólnego upload_command i wrappera ani nie wstawiaj --auth do konfiguracji.
- Unikaj blokujących pętli eksperymentu; używaj millis() zamiast długiego delay().
- Po zmianach buduj oba środowiska; nodemcu-32s jest aliasem esp32-1.
- Build nie dowodzi działania OTA. Nie obiecuj, że OTA jest niemożliwe do usunięcia.
- Przed wgraniem ustal płytkę i zgodność hasła z obecnym firmware.
  Wgrywaj tylko w zakresie zlecenia użytkownika; samo przygotowanie repo nie obejmuje flashowania.

## Sprzęt

- Przed instrukcją okablowania rozpoznaj dokładny model modułu i układ pinów płytki.
  Nie zgaduj modelu serwa, czujnika temperatury ani numerów pól płytki stykowej.
- Podaj tabelę pin ESP32 → element → pole płytki (gdy położenie jest znane).
- Potwierdź napięcie logiki, pobór prądu i wspólną masę; żadnego 5 V na GPIO.
- GPIO i 3V3 nie są zasilaniem serwa/silnika. Nie polecaj przecinania kabla USB.
- Uwzględnij GPIO wejściowe, piny flash i piny startowe; sprawdź dokumentację modułu.
- Zmieniaj przewody przy odłączonym zasilaniu. Nie zakładaj ciągłości szyn breadboardu.

## Prowadzenie użytkownika przez okablowanie

1. Przed doborem pinów przeczytaj docs/PINOUT.md i docs/HARDWARE.md oraz lokalną
   .local/BREADBOARD.md, jeśli istnieje. Karta pinów zawiera sprawdzone podstawy;
   nie szukaj ich ponownie w Internecie bez konkretnej potrzeby weryfikacji.
   docs/HARDWARE.md zawiera przepisane oznaczenia NodeMCU ESP-32S V1.1 i SG90,
   sprawdzony referencyjny tor USB → D1 → 5V oraz granice pewności tych ustaleń.
   Nie pytaj ponownie o zapisane napisy ani nie utożsamiaj schematu referencyjnego
   z pomiarem wydajności zasilania rzeczywistej płytki. Obecny eksperyment to
   DHT11 + KY-016 (docs/CLIMATE_RGB.md); stara LED i przewody zostały usunięte
   według potwierdzenia użytkownika. 2026-11-14 użytkownik wrócił do planowania
   SG90 oraz krokowca 28BYJ-48, następnie wybrał krokowiec z ULN2003.
   Kod i plan montażu: docs/STEPPER_WEB.md; działające DHT11/RGB zostają.
   Krokowiec działa według użytkownika; mostek ULN2003 pozostaje.
   Użytkownik następnie zlecił SG90: docs/SERVO_WEB.md. Kod i plan przygotowane;
   działanie podstawowego SG90 potwierdzone przez użytkownika. Dodatki zera NVS
   i prędkości wymagają testu urządzenia. Nie uruchamiać serwa samoczynnie.
   Goła RGB rozpoznana testem użytkownika: 1 B, 2 G, 3 wspólna katoda, 4 R.
   Plan dodatkowych wyjść GPIO17/32/33 i panelu: docs/RGB_BARE.md. Montaż
   docelowy i działanie potwierdzone przez użytkownika; nie przywracać testowego
   połączenia 3V3 do wejścia rezystora po podłączeniu go do GPIO.
   Efekty LED i jednorazowe sekwencje ruchu: docs/EFFECTS.md. Zachowaj Stop,
   brak samoczynnego startu oraz blokadę obu napędów także podczas pauz sekwencji.
2. Ustal, czy pracujemy na ESP32-1 czy ESP32-2. Użytkownik potwierdził identyczność
   obu płytek: mapa P/L jest wspólna, nie pytaj ponownie o jej zgodność.
   Użytkownik nie planuje innych modeli ESP32. Potwierdzone adresy otworów są
   w .local/BREADBOARD.md — korzystaj z nich bez ponownego pytania. Ustal jedynie
   urządzenie, którego dotyczy bieżący montaż, i brakujące informacje o przewodach.
   P/L to umowne rzędy płytki: widok z góry, USB po lewej, antena po prawej,
   01 od USB. Nie stosuj samego „lewo/prawo” bez P/L.
3. Zawsze rozróżniaj GPIO/nazwę sygnału, pozycję P-xx/L-xx i rzeczywisty adres otworu
   breadboardu. Numer pozycji nie wyznacza numeru rzędu/kolumny breadboardu.
4. Jeśli brak adresów otworów lub potwierdzonej orientacji, zapytaj o brakujące
   położenie/zdjęcie. Możesz podać plan sygnałów, ale oznacz go jako niekompletny
   do montażu; nie zgaduj współrzędnych. Nie pytaj ponownie o dane już potwierdzone.
5. Każdy przewód opisz w osobnym kroku: funkcja, sugerowany kolor, sygnał/GPIO,
   pozycja P/L, wolny otwór elektrycznie połączony z pinem i dokładny drugi koniec.
   Nie każ wkładać przewodu do otworu już zajętego nóżką płytki.
6. Przed połączeniami sprawdź aktualną zajętość, napięcia i możliwości GPIO według
   karty; uwzględnij wszystkie piny startowe, w tym GPIO5 (L-10). Pamięć flash
   GPIO6–11 jest wyłączona z eksperymentów. Wejściowe GPIO34–39 nie sterują wyjściami.
7. Montaż opisuj w kolejności przy odłączonym zasilaniu, z kontrolą przed włączeniem.
   Wspólna masa nie musi oznaczać jednego fizycznego otworu GND.
8. Lokalny stan aktualizuj dopiero po potwierdzeniu wykonania przez użytkownika.
   Oddziel planowane przewody od faktycznie wpiętych; nie oznaczaj propozycji jako montażu.
9. Do źródeł wracaj dla nowych modułów, wariantów PCB, parametrów nieopisanych
   w karcie lub sprzeczności. Rozpoznanie modelu i napięcia nowego modułu nadal jest wymagane.

## Prywatność i publikacja

- Hasła tylko w secrets.local.json; notatki bez haseł tylko w .local/.
- Nie commituj absolutnych ścieżek komputera, prywatnych IP, SSID ani haseł.
- Nie publikuj firmware/.pio — zawierają dane dostępowe.
- Zachowaj .gitignore; nie używaj git add -f do prywatnych plików.
- Przed commitem: python3 scripts/audit_public.py oraz przegląd plików staged.
- Nigdy nie wypisuj pełnej konfiguracji PlatformIO w trybie diagnostycznym bez oceny sekretów.
- Nie dodawaj remote, nie pushuj i nie twórz repo na GitHub bez osobnego zlecenia.
- Nie dodawaj nowej licencji bez wyboru użytkownika.

## Weryfikacja i raport

Użyj pio run -e esp32-1 oraz pio run -e esp32-2 (na tym Macu można użyć launchera
~/.platformio/penv/bin/pio). Nie zmieniaj wersji platformy bez potrzeby: bazowa wersja to 8.0.0.
Raportuj osobno build, upload i sprawdzenie HTTP. Brak testu urządzenia opisz wprost.
Nie zmieniaj systemowych uprawnień LAN ani routera w ramach builda.
