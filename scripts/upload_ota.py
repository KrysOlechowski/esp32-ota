"""Uruchamia espota wewnątrz procesu, bez hasła w argv i bez debug logów."""
import argparse
import importlib.util
import logging
from pathlib import Path
from project_config import load_config

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--environment", required=True)
    parser.add_argument("--file", required=True)
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[1]
    try:
        config = load_config(root, args.environment)
    except ValueError as error:
        parser.exit(2, str(error) + "\n")
    # Korzystaj z katalogu pakietów tego samego Core co wywołujące PlatformIO.
    from platformio.package.manager.platform import PlatformPackageManager
    from platformio.project.config import ProjectConfig
    project = ProjectConfig(str(root / "platformio.ini"))
    platform = PlatformPackageManager().get_package(project.get("env:" + args.environment, "platform"))
    if platform is None:
        parser.exit(2, "Brak platformy. Najpierw wykonaj build.\n")
    from platformio.platform.factory import PlatformFactory
    installed = PlatformFactory.new(platform.path)
    framework = installed.get_package_dir("framework-arduinoespressif32")
    uploader = Path(framework) / "tools" / "espota.py"
    spec = importlib.util.spec_from_file_location("project_espota", uploader)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    # espota w DEBUG wypisuje obiekt opcji wraz z auth. Wymuś WARNING.
    logging.basicConfig(level=logging.WARNING, force=True)
    logging.disable(logging.INFO)
    return module.main(["-i", config["upload_host"], "--auth", config["ota_password"],
                        "--file", str(Path(args.file).resolve()), "--progress"])

if __name__ == "__main__":
    raise SystemExit(main())
