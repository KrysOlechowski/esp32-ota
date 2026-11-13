"""Sprawdza HTTP pod adresem z lokalnego configu, bez wypisywania sekretów."""
import sys
from pathlib import Path
from urllib.request import urlopen
from urllib.error import URLError
from project_config import load_config

if len(sys.argv) != 2:
    raise SystemExit("Użycie: python3 scripts/check_http.py esp32-1|esp32-2")
try:
    config = load_config(Path(__file__).resolve().parents[1], sys.argv[1])
    with urlopen("http://" + config["upload_host"] + "/", timeout=5) as response:
        body = response.read(8192).decode("utf-8")
    if ("Urzadzenie: " + config["hostname"]) not in body:
        raise SystemExit("HTTP odpowiedziało, ale brak oczekiwanej tożsamości nowego firmware.")
    print("HTTP OK: " + config["hostname"])
except (ValueError, URLError, TimeoutError, OSError):
    raise SystemExit("Nie udało się potwierdzić HTTP. Sprawdź lokalny adres, zasilanie i sieć.") from None
