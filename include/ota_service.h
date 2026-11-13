#pragma once

// Infrastruktura OTA: zachowaj begin() w setup oraz handle() w każdej iteracji loop.
namespace OtaService {
void begin();
void handle();
}
