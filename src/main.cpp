#include "SystemController.h"

SystemController systems;

void setup()
{
  Serial.begin(115200);

  // Подключение WiFi
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");

  systems.setup();
}

void loop()
{
  systems.loop();
}