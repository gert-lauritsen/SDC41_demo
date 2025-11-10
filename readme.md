\# 🌿 SCD41\_D\_R2 Demo for ESP32-WROOM (Arduino)



Dette projekt demonstrerer, hvordan man bruger \*\*Sensirion SCD41-D-R2\*\* sensoren til at måle \*\*CO₂-koncentration\*\*, \*\*temperatur\*\* og \*\*relativ fugtighed\*\* ved hjælp af et \*\*ESP32-WROOM\*\* udviklingsboard.  

Projektet er skrevet til \*\*Arduino framework\*\* og fungerer som et simpelt udgangspunkt for udvikling og test.



---



\## ⚙️ Hardwareopsætning



\### 🧩 Krævede komponenter

\- ESP32-WROOM DevKitC eller kompatibelt board  

\- Sensirion SCD41-D-R2 CO₂-sensor  

\- Dupont-kabler (han–hun)  



\### 🔌 Tilslutning (I²C)



| SCD41 pin | ESP32 pin | Funktion         |

|------------|------------|------------------|

| VDD        | 3V3        | Forsyning        |

| GND        | GND        | Jord             |

| SDA        | GPIO21     | I²C Data         |

| SCL        | GPIO22     | I²C Clock        |



> Du kan bruge andre GPIO’er til SDA/SCL, men så skal du ændre `Wire.begin(SDA, SCL)` i koden.



---



\## 🧰 Softwarekrav



\- \*\*Arduino IDE v2.x\*\* eller \*\*PlatformIO\*\*

\- ESP32 board support:



