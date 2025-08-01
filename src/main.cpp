#include "Config.h"
#include "SensorRelay.h"
#include "MqttManager.h"
#include "NetworkTime.h"
#include "Fan.h"
#include <GyverPID.h>

// Глобальные переменные
int setPoint = 26;
int start_second;
int stop_second;
int period_second;
int work_raine;
int work_time;

// Объекты
GyverPID regulator(25, 0.5, 0, 10);
Fan fan(FAN_PWM_PIN, FAN_STOP_PIN);
SensorRelay sensors;
WiFiClient wifiClient;
MqttManager mqtt(wifiClient);
NetworkTime ntp;

// Таймеры
int timer, timer_fan = 0;

// Прототипы функций
void ICACHE_RAM_ATTR HandleInterrupt();
// Исправленная сигнатура callback
void mqttCallback(char *topic, byte *payload, unsigned int length);
void printStats();

void setup()
{
    Serial.begin(115200);

    // Инициализация
    ntp.begin();
    sensors.begin();

    // Настройка PID
    regulator.setDirection(REVERSE);
    regulator.setLimits(0, 100);

    // Настройка вентилятора
    pinMode(FAN_TACH_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(FAN_TACH_PIN), HandleInterrupt, FALLING);

    // MQTT
    mqtt.begin();
    mqtt.setCallback(mqttCallback); // Теперь совместимая сигнатура
}

void loop()
{
    ntp.tick();
    mqtt.loop();

    // Переподключение MQTT при необходимости
    if (!mqtt.isConnected())
    {
        mqtt.reconnect();
    }

    uint32_t now = millis();
    uint32_t currentSecond = ntp.daySeconds();

    // Основной цикл (1 сек)
    if (now - timer >= PERIOD_SECOND)
    {
        timer = now;

        sensors.update();
        sensors.controlLight(currentSecond);

        // Отправка данных
        mqtt.publishData(
            sensors,
            fan.getRPM(),
            round(regulator.getResultTimer()),
            ntp.toString());
        printStats();
    }

    // Управление вентилятором (10 мс)
    if (now - timer_fan >= FAN_PERIOD)
    {
        timer_fan = now;
        regulator.input = sensors.temperature;
        regulator.setpoint = setPoint;
        fan.SetFanLevel(regulator.getResultTimer());
    }
}

// Прерывание для тахометра
void ICACHE_RAM_ATTR HandleInterrupt()
{
    fan.tick();
}

// Исправленный обработчик MQTT сообщений
void mqttCallback(char *topic, byte *payload, unsigned int length)
{
    String topicStr = String(topic);
    String messageTemp;
    for (int i = 0; i < length; i++)
    {
        messageTemp += (char)payload[i];
    }

    if (topicStr == "CFG")
    {
        StaticJsonDocument<500> doc;
        DeserializationError err = deserializeJson(doc, messageTemp);
        if (err)
            return;

        setPoint = doc["setTemp"];
        start_second = doc["setTimeLightMin"];
        start_second = start_second * 60;
        work_time = doc["setDurationLight"];
        work_time = work_time * 60;
        stop_second = start_second + work_time;
        period_second = doc["setTimeRain"];
        period_second = period_second * 60;
        work_raine = doc["setDurationRain"];
        work_raine = work_raine * 60;
    }
}

// Отладочный вывод
void printStats()
{
    Serial.print("Sett: ");
    Serial.print(setPoint);
    Serial.print(" T: ");
    Serial.print(sensors.temperature);
    Serial.print(" H: ");
    Serial.print(sensors.humidity);
    Serial.print(" Fan: ");
    Serial.print(fan.getRPM());
    Serial.print(" Light: ");
    Serial.print(sensors.relayLightState);
    Serial.print(" Rain: ");
    Serial.print(sensors.relayRainState);
    Serial.print(" Warm: ");
    Serial.println(sensors.relayWarmState);
}