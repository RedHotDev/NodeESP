#pragma once

// Пины
#define DHT_PIN 4
#define Fan_Pin_PWM 15
#define Fan_Pin_Tach 12
#define Fan_STOP 5
#define RELAY_PIN_WARM 16
#define RELAY_PIN_RAIN 2
#define RELAY_PIN_LIGHT 14

// WiFi
#define WLAN_SSID "MERCUSYS"
#define WLAN_PASS "alcm7bvn"

// Тайминги
#define PERIOD_SECOND 1000
#define FAN_PERIOD 10

// Параметры системы
const uint8_t SETPOINT_TEMP = 24;
const unsigned long WORK_TIME = 25; // сек работы освещения
const uint32_t RAIN_PERIOD = 60;    // период полива (сек)
const uint8_t RAIN_WORK_TIME = 10;  // время полива (сек)

// MQTT
const char *MQTT_SERVER = "test.mosquitto.org";
const int MQTT_PORT = 1883;
const char *MQTT_CLIENT_NAME = "ESP8266Client";
const char *MQTT_TOPIC = "topic_esp";