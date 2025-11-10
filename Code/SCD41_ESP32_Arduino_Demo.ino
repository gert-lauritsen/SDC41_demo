/*
  SCD41-D-R2 Demo for ESP32-WROOM (Arduino)
  - I2C pins: SDA=GPIO21, SCL=GPIO22
  - Serial: 115200 baud
  - Library: "Sensirion I2C SCD4x" (Library Manager)

  Serial commands (type and press Enter):
    h                   : help
    start               : start periodic measurement
    stop                : stop periodic measurement
    reinit              : software re-init of the sensor
    selftest            : run internal self-test
    abc on              : enable Automatic Baseline Correction
    abc off             : disable Automatic Baseline Correction
    alt <meters>        : set sensor altitude (ex: alt 45)
    press <mbar>        : set ambient pressure in mbar (ex: press 1013)
*/

#include <Wire.h>
#include <SensirionI2CScd4x.h>
#include <SensirionCore.h>  // for errorToString()

SensirionI2CScd4x scd4x;

static const int SDA_PIN = 21;
static const int SCL_PIN = 22;

const unsigned long MEAS_PERIOD_MS = 5000; // SCD41 updates ~every 5 s
unsigned long lastReadMs = 0;

// User settings (you can change defaults here)
bool abcEnabled = true;            // Automatic Baseline Correction
uint16_t altitudeMeters = 0;       // 0 = sea level
uint16_t pressureMbar = 0;         // 0 = not set (disabled)

void printErrorIfAny(uint16_t error, const char* context) {
  if (error) {
    char err[64];
    errorToString(error, err, sizeof(err));
    Serial.print("[ERR] ");
    Serial.print(context);
    Serial.print(": 0x");
    Serial.print(error, HEX);
    Serial.print(" -> ");
    Serial.println(err);
  }
}

void printHelp() {
  Serial.println();
  Serial.println(F("Commands:"));
  Serial.println(F("  h                 : help"));
  Serial.println(F("  start             : start periodic measurement"));
  Serial.println(F("  stop              : stop periodic measurement"));
  Serial.println(F("  reinit            : software re-init of the sensor"));
  Serial.println(F("  selftest          : run internal self-test"));
  Serial.println(F("  abc on/off        : enable/disable ABC"));
  Serial.println(F("  alt <meters>      : set altitude, ex: alt 45"));
  Serial.println(F("  press <mbar>      : set ambient pressure, ex: press 1013"));
  Serial.println();
}

void applyUserSettings() {
  uint16_t err;

  err = scd4x.setAutomaticSelfCalibration(abcEnabled);
  printErrorIfAny(err, "setAutomaticSelfCalibration");

  if (altitudeMeters > 0) {
    err = scd4x.setSensorAltitude(altitudeMeters);
    printErrorIfAny(err, "setSensorAltitude");
  }

  if (pressureMbar > 0) {
    err = scd4x.setAmbientPressure(pressureMbar);
    printErrorIfAny(err, "setAmbientPressure");
  }
}

void startMeasurement() {
  uint16_t err;

  // safety stop first
  err = scd4x.stopPeriodicMeasurement();
  if (!err) delay(50);

  // apply settings before start
  applyUserSettings();

  err = scd4x.startPeriodicMeasurement();
  if (err) {
    printErrorIfAny(err, "startPeriodicMeasurement");
  } else {
    Serial.println(F("[OK] Measurement started"));
  }
}

void stopMeasurement() {
  uint16_t err = scd4x.stopPeriodicMeasurement();
  if (err) {
    printErrorIfAny(err, "stopPeriodicMeasurement");
  } else {
    Serial.println(F("[OK] Measurement stopped"));
  }
}

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println();
  Serial.println(F("SCD41-D-R2 Demo - ESP32-WROOM (Arduino)"));
  Serial.println(F("I2C: SDA=21, SCL=22"));
  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.setClock(100000); // 100kHz is fine for SCD41

  scd4x.begin(Wire);

  // Read and show serial number (optional)
  uint16_t err;
  uint16_t serial0, serial1, serial2;
  err = scd4x.getSerialNumber(serial0, serial1, serial2);
  if (!err) {
    Serial.printf("Sensor SN: %04X-%04X-%04X\r\n", serial0, serial1, serial2);
  } else {
    printErrorIfAny(err, "getSerialNumber");
  }

  // Re-init (prepare sensor)
  err = scd4x.stopPeriodicMeasurement(); // if running
  printErrorIfAny(err, "stopPeriodicMeasurement");

  // (optional) Reinit sensor
  err = scd4x.reinit();
  if (!err) delay(20);
  printErrorIfAny(err, "reinit");

  // Apply defaults and start measurement
  startMeasurement();

  printHelp();
}

void handleSerial() {
  if (!Serial.available()) return;

  String line = Serial.readStringUntil('\n');
  line.trim();
  if (line.length() == 0) return;

  // to lower for easy parsing
  String cmd = line;
  cmd.toLowerCase();

  if (cmd == "h" || cmd == "help" || cmd == "?") {
    printHelp();
    return;
  }

  if (cmd == "start") {
    startMeasurement();
    return;
  }

  if (cmd == "stop") {
    stopMeasurement();
    return;
  }

  if (cmd == "reinit") {
    uint16_t err = scd4x.reinit();
    printErrorIfAny(err, "reinit");
    if (!err) {
      delay(20);
      Serial.println(F("[OK] reinit done"));
    }
    return;
  }

  if (cmd == "selftest") {
    uint16_t selfTestResult = 0;
    uint16_t err = scd4x.performSelfTest(selfTestResult);
    printErrorIfAny(err, "performSelfTest");
    if (!err) {
      if (selfTestResult == 0) {
        Serial.println(F("[OK] Self-test passed"));
      } else {
        Serial.print(F("[WARN] Self-test indicates issues: 0x"));
        Serial.println(selfTestResult, HEX);
      }
    }
    return;
  }

  if (cmd.startsWith("abc ")) {
    if (cmd.endsWith("on")) {
      abcEnabled = true;
      uint16_t err = scd4x.setAutomaticSelfCalibration(true);
      printErrorIfAny(err, "setAutomaticSelfCalibration(on)");
      if (!err) Serial.println(F("[OK] ABC ON"));
    } else if (cmd.endsWith("off")) {
      abcEnabled = false;
      uint16_t err = scd4x.setAutomaticSelfCalibration(false);
      printErrorIfAny(err, "setAutomaticSelfCalibration(off)");
      if (!err) Serial.println(F("[OK] ABC OFF"));
    } else {
      Serial.println(F("[ERR] Usage: abc on | abc off"));
    }
    return;
  }

  if (cmd.startsWith("alt ")) {
    int value = cmd.substring(4).toInt();
    if (value < 0) value = 0;
    altitudeMeters = (uint16_t)value;
    uint16_t err = scd4x.setSensorAltitude(altitudeMeters);
    printErrorIfAny(err, "setSensorAltitude");
    if (!err) {
      Serial.print(F("[OK] Altitude set to "));
      Serial.print(altitudeMeters);
      Serial.println(F(" m"));
    }
    return;
  }

  if (cmd.startsWith("press ")) {
    int value = cmd.substring(6).toInt();
    if (value < 0) value = 0;
    pressureMbar = (uint16_t)value;
    uint16_t err = scd4x.setAmbientPressure(pressureMbar);
    printErrorIfAny(err, "setAmbientPressure");
    if (!err) {
      Serial.print(F("[OK] Ambient pressure set to "));
      Serial.print(pressureMbar);
      Serial.println(F(" mbar"));
    }
    return;
  }

  Serial.println(F("[ERR] Unknown command. Type 'h' for help."));
}

void readAndPrintMeasurement() {
  uint16_t co2;
  float temp;
  float rh;

  uint16_t err = scd4x.readMeasurement(co2, temp, rh);
  if (err) {
    printErrorIfAny(err, "readMeasurement");
    return;
  }

  // co2==0 indicates no new data yet
  if (co2 == 0) {
    return;
  }

  Serial.print(F("CO2: "));
  Serial.print(co2);
  Serial.print(F(" ppm | Temp: "));
  Serial.print(temp, 1);
  Serial.print(F(" °C | RH: "));
  Serial.print(rh, 1);
  Serial.println(F(" %"));
}

void loop() {
  handleSerial();

  unsigned long now = millis();
  if (now - lastReadMs >= MEAS_PERIOD_MS) {
    lastReadMs = now;
    readAndPrintMeasurement();
  }
}
