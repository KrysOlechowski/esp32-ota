"""Audyt nazw i zawartości bez wypisywania znalezionych wartości."""
import json
import re
import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
PRIVATE = {".local", ".private-backup", ".pio", ".git", "__pycache__"}

def private(path):
    return (bool(PRIVATE.intersection(path.parts)) or path.name == "secrets.local.json"
            or path.name.endswith(".local.json") or path.name == "platformio.local.ini"
            or path.suffix in {".bin", ".elf", ".pem", ".key"}
            or path.name == ".env" or path.name.startswith(".env."))

def git(*args):
    return subprocess.check_output(["git", "-C", str(ROOT), *args], stderr=subprocess.DEVNULL)

def main():
    secrets = []
    config = ROOT / "secrets.local.json"
    if config.exists():
        def collect(value):
            if isinstance(value, dict):
                for key, item in value.items():
                    if key in {"wifi_ssid", "wifi_password", "ota_password"} and isinstance(item, str):
                        secrets.append(item.encode())
                    else:
                        collect(item)
        try:
            collect(json.loads(config.read_text()))
        except ValueError:
            raise SystemExit("Nie można zweryfikować lokalnej konfiguracji; audyt przerwany.") from None
    try:
        names = git("ls-files", "--cached", "--others", "--exclude-standard", "-z").decode().split("\0")
        indexed = set(git("ls-files", "-z").decode().split("\0"))
    except subprocess.CalledProcessError:
        names = [str(f.relative_to(ROOT)) for f in ROOT.rglob("*") if f.is_file()
                 and not private(f.relative_to(ROOT)) and ".vscode" not in f.relative_to(ROOT).parts]
        indexed = set()
    home_pattern = "/" + "(?:Users|home)/[^/\\s]+/"
    patterns = [re.compile(home_pattern.encode()),
                re.compile(rb"(?<![0-9])(?:192\.168\.[0-9]+\.[0-9]+|10\.[0-9]+\.[0-9]+\.[0-9]+|172\.(?:1[6-9]|2[0-9]|3[01])\.[0-9]+\.[0-9]+)(?![0-9])")]
    failures = set()
    count = 0
    for name in sorted(set(names) - {""}):
        path = Path(name)
        if private(path):
            failures.add(name)
            continue
        versions = []
        if name in indexed:
            versions.append(git("show", ":" + name))
        file = ROOT / name
        if file.is_symlink():
            failures.add(name)
            continue
        if file.is_file():
            versions.append(file.read_bytes())
        count += 1
        for data in versions:
            if any(value and value in data for value in secrets) or any(p.search(data) for p in patterns):
                failures.add(name)
    if failures:
        print("Audyt: sprawdź poniższe pliki; wartości zostały ukryte:")
        for name in sorted(failures):
            print("- " + name)
        return 1
    print(f"Audyt OK: {count} plików. Brak lokalnych sekretów, prywatnych IP i ścieżek domowych.")
    return 0

if __name__ == "__main__":
    raise SystemExit(main())
