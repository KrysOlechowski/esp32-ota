"""Lokalna konfiguracja bez wypisywania sekretów."""
import json
from pathlib import Path

ALIASES = {"nodemcu-32s": "esp32-1", "esp32-1-usb": "esp32-1", "esp32-2-usb": "esp32-2"}

def load_config(root, environment):
    path = Path(root) / "secrets.local.json"
    if not path.is_file():
        raise ValueError("Skopiuj secrets.example.json do secrets.local.json i uzupełnij dane.")
    try:
        data = json.loads(path.read_text(encoding="utf-8"))
        name = ALIASES.get(environment, environment)
        device = data["devices"][name]
        result = {key: device.get(key, data.get(key)) for key in
                  ("wifi_ssid", "wifi_password", "ota_password", "hostname", "upload_host")}
        if not all(isinstance(v, str) and v and not v.startswith(("UZUPELNIJ", "CHANGE_THIS_VALUE")) for v in result.values()):
            raise ValueError()
        return result
    except (ValueError, KeyError, TypeError):
        raise ValueError("Niepoprawna konfiguracja lokalna dla wybranego środowiska.") from None
