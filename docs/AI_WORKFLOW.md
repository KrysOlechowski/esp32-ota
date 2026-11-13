# Współpraca z GPT i Codex

Rozmawiamy po polsku. README pozostaje po angielsku. Projekt służy do nauki,
testów modułów i współpracy z AI; propozycje agenta wymagają weryfikacji sprzętowej.
Projektowy AGENTS.md zawiera zasady utrzymania OTA, sekretów i okablowania.
Osobny skill nie daje obecnie korzyści — nie trzeba niczego instalować.

## Stała wiedza o stanowisku

Agent przed instrukcją montażu czyta docs/PINOUT.md (mapa P/L i ograniczenia)
oraz .local/BREADBOARD.md (konkretne otwory i aktualne przewody). Nie musi ponownie
szukać opisanych podstaw w dokumentacji. Nowe moduły lub nieopisane parametry wymagają
weryfikacji. Każdy przewód powinien mieć podane oba końce: GPIO/sygnał, pozycję P/L,
rzeczywisty wolny otwór i pole przy module. Nieznanych adresów agent nie zgaduje.
Mapa P/L nie jest jeszcze kompletną instrukcją montażu na breadboardzie.

## Dobry początek zadania

Podaj agentowi:

- wybraną płytkę: esp32-1 lub esp32-2;
- cel eksperymentu i dokładne modele elementów;
- aktualne przewody, wolne piny i orientację breadboardu;
- źródło zasilania i jego ograniczenia;
- czy zadanie obejmuje tylko kod/instrukcję, czy również upload.

Przykład do skopiowania:

> Pracujemy po polsku w tym repo. Przeczytaj AGENTS.md. Chcę przygotować miganie
> zewnętrzną LED na esp32-1. Zaproponuj połączenia po sprawdzeniu modelu płytki,
> zachowaj OTA i HTTP, unikaj blokujących pętli, zbuduj oba środowiska.
> Nie wgrywaj jeszcze firmware. Nie wypisuj danych dostępowych.

Gdy brakuje modelu, prześlij oznaczenia lub zdjęcie modułu zamiast zgadywać pinout.
Przed montażem oczekuj tabeli pinów, napięć i kolejności podłączeń. Agent nie powinien
wymyślać numerów pól breadboardu bez informacji o fizycznym ułożeniu.

## Po wykonaniu

Oczekuj opisu zmienionego zachowania, wyników builda i dokładnej komendy uploadu
na wskazaną płytkę. Odróżniaj build od sprawdzenia urządzenia. Po dozwolonym uploadzie
sprawdź HTTP i następną aktualizację OTA; zanotuj wynik bez haseł w .local/.

Nie wklejaj secrets.local.json ani kopii odzyskiwania do czatu, issue czy README.
Współdzielony opis błędu powinien zawierać zanonimizowane komunikaty, wersje i kroki
odtworzenia. Adresy stanowiska i prywatne ścieżki zachowaj w notatkach lokalnych.
