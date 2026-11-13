"""Generuje prywatny nagłówek wyłącznie w ignorowanym katalogu build."""
Import("env")
import json
import sys
from pathlib import Path

root = Path(env.subst("$PROJECT_DIR"))
sys.path.insert(0, str(root / "scripts"))
from project_config import load_config
try:
    config = load_config(root, env.subst("$PIOENV"))
except ValueError as error:
    print(str(error))
    env.Exit(1)

build = Path(env.subst("$BUILD_DIR"))
build.mkdir(parents=True, exist_ok=True)
header = build / "private_config.h"
values = {"WIFI_SSID": config["wifi_ssid"], "WIFI_PASSWORD": config["wifi_password"],
          "OTA_PASSWORD": config["ota_password"], "DEVICE_HOSTNAME": config["hostname"]}
content = "#pragma once\n" + "".join(
    f"#define {key} {json.dumps(value, ensure_ascii=True)}\n" for key, value in values.items())
if not header.exists() or header.read_text() != content:
    header.write_text(content)
header.chmod(0o600)
env.Append(CPPPATH=[str(build)])
