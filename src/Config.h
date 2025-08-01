#pragma once

// Пины
constexpr int DHT_PIN = 4;
constexpr int FAN_PWM_PIN = 15;
constexpr int FAN_TACH_PIN = 12;
constexpr int FAN_STOP_PIN = 5;
constexpr int RELAY_WARM_PIN = 16;
constexpr int RELAY_RAIN_PIN = 2;
constexpr int RELAY_LIGHT_PIN = 14;

// WiFi
constexpr const char *WLAN_SSID = "MERCUSYS";
constexpr const char *WLAN_PASS = "alcm7bvn";

// MQTT
constexpr const char *MQTT_SERVER = "m8.wqtt.ru";
constexpr int MQTT_PORT = 20606;
constexpr const char *MQTT_USER = "u_BJIUEH";
constexpr const char *MQTT_PASS = "jlNoV6gO";

// Тайминги
constexpr int PERIOD_SECOND = 1000;
constexpr int FAN_PERIOD = 10;

// Глобальные переменные (объявлены в main.cpp)
extern int setPoint;
extern int start_second;
extern int stop_second;
extern int period_second;
extern int work_raine;
extern int work_time;