/*
  SCD41-D-R2 Demo for ESP32-WROOM (Arduino)
  - I2C pins: SDA=GPIO21, SCL=GPIO22
  - Serial: 115200 baud
  - Library: "Sensirion I2C SCD4x" (Library Manager)
  - Auto-reads CO2, temp, RH every 5 seconds
*/
#include <config.h>
// Matter Manager
#include <Matter.h>
#include <Wire.h>
#include <SensirionI2CScd4x.h>
#include <SensirionCore.h>
SensirionI2CScd4x scd4x;

unsigned long lastReadMs = 0;

// List of Matter Endpoints for this Node
// Matter Humidity Sensor Endpoint
MatterHumiditySensor HumiditySensor;
MatterTemperatureSensor TemperatureSensor;
MatterPressureSensor Co2Sensor; // Dummy Pressure Sensor for Co2 Endpoint, as Co2 is not yet supported directly

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println();
  Serial.println(F("Willkommen zum SCD4x ESP32 Demo!"));
  
  Serial.print("Connecting to WiFi");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.setClock(100000);
  scd4x.begin(Wire);  
  HumiditySensor.begin(95.00); // 95% RH max
  TemperatureSensor.begin(40.00); // 40 C max
  Co2Sensor.begin(2000); // 2000 hPa max - Dummy for Co2
  
  // Matter beginning - Last step, after all EndPoints are initialized
  Matter.begin();

  // Stop any running measurement
  scd4x.stopPeriodicMeasurement();
  delay(500);

  // Start periodic measurement
  uint16_t err = scd4x.startPeriodicMeasurement();
  if (err) {
    Serial.println(F("Error starting measurement!"));
    while(1); // halt
  }

    if (!Matter.isDeviceCommissioned()) {
    Serial.println("");
    Serial.println("Matter Node is not commissioned yet.");
    Serial.println("Initiate the device discovery in your Matter environment.");
    Serial.println("Commission it to your Matter hub with the manual pairing code or QR code");
    Serial.printf("Manual pairing code: %s\r\n", Matter.getManualPairingCode().c_str());
    Serial.printf("QR code URL: %s\r\n", Matter.getOnboardingQRCodeUrl().c_str());
    // waits for Matter Humidity Sensor Commissioning.
    uint32_t timeCount = 0;
    while (!Matter.isDeviceCommissioned()) {
      delay(100);
      if ((timeCount++ % 50) == 0) {  // 50*100ms = 5 sec
        Serial.println("Matter Node not commissioned yet. Waiting for commissioning.");
      }
    }
    Serial.println("Matter Node is commissioned and connected to the network. Ready for use.");
  }
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
  Serial.println("WiFi disconnected, reconnecting...");
  WiFi.reconnect();
  }
  unsigned long now = millis();
  
  if (now - lastReadMs >= CO2_UPDATE_INTERVAL) {
    lastReadMs = now;
    
    uint16_t co2;
    float temp;
    float rh;

    uint16_t err = scd4x.readMeasurement(co2, temp, rh);
    
    if (err) {
      Serial.println(F("Error reading measurement"));
      return;
    }

    // co2==0 means no new data yet
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
    HumiditySensor.setHumidity(rh);
    TemperatureSensor.setTemperature(temp);
    Co2Sensor.setPressure(co2); // Dummy for Co2
  }
}