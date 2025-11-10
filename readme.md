# 🌿 SCD41_D_R2 Demo for ESP32-WROOM (Arduino)

Dette projekt demonstrerer, hvordan man bruger **Sensirion SCD41-D-R2** til at måle **CO₂**, **temperatur** og **relativ fugtighed** med et **ESP32-WROOM** udviklingsboard i **Arduino**.

## 🔌 Hardware (I²C forbindelse)
| SCD41 pin | ESP32 pin | Funktion |
|-----------|-----------|----------|
| VDD       | 3V3       | Forsyning |
| GND       | GND       | Jord |
| SDA       | GPIO21    | I²C Data |
| SCL       | GPIO22    | I²C Clock |

> Du kan vælge andre pins, men husk at ændre `Wire.begin(SDA, SCL)` i koden.

## 🧰 Software
- **Arduino IDE v2.x** eller **PlatformIO**
- **ESP32 by Espressif Systems** (Boards Manager)
- Bibliotek: **Sensirion I2C SCD4x** (Library Manager)

## 🚀 Kom i gang
1. Åbn mappen i Arduino IDE og vælg board **ESP32 Dev Module**.
2. Installer biblioteket **Sensirion I2C SCD4x**.
3. Tilslut sensoren som angivet ovenfor.
4. Upload `SCD41_ESP32_Arduino_Demo.ino` og åbn Serial Monitor @ **115200 baud**.
5. Skriv `h` + Enter for hjælpetekst/kommandoer.

## 🎛️ Seriel kommando-menu
- `h` — hjælp
- `start` / `stop` — start/stop måling
- `reinit` — software re-init
- `selftest` — kør intern selvtest
- `abc on` / `abc off` — Automatic Baseline Correction til/fra
- `alt <meter>` — sæt sensorhøjde, fx `alt 45`
- `press <mbar>` — sæt omgivende tryk, fx `press 1013`

## 🧪 Forventet output
```
SCD41-D-R2 Demo - ESP32-WROOM (Arduino)
I2C: SDA=21, SCL=22
Sensor SN: 1234-5678-9ABC
[OK] Measurement started
CO2: 413 ppm | Temp: 23.7 °C | RH: 44.6 %
...
```

## 📚 Referencer
- Sensirion SCD41-D-R2 datasheet
- Sensirion Arduino Library for SCD4x
- ESP32 Arduino Core Docs


## 👤 Forfatter
Gert Lauritsen — Fieldtest / Airborne Instruments
