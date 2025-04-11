#define DHT_PIN 4
#include "DHT.h"
#include <Adafruit_Sensor.h>
#include "SensorDHT.h"
#include "RelyWarm.h"
#include "RelyRain.h"
#include "GyverPID.h"

#include <ESP8266WiFi.h>
#include "Fan.h"
#include <GyverNTP.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>


#define Fan_Pin_PWM 15 //вентилятор PWM
#define Fan_Pin_Tach 12 //вентилятор тахометр
#define Fan_STOP 5 //вентилятор реле на остановку 
#define  RELAY_PIN_WARM 16 //реле нагрева
#define  RELAY_PIN_RAIN 2 //реле полива
#define  RELAY_PIN_LIGHT 14 //реле освещения

#define SETPOINT 24 // установка

// wifi
#define WLAN_SSID "MERCUSYS"
#define WLAN_PASS "alcm7bvn"


uint32_t timer, timer_fan = 0; // переменная таймера
#define PERIOD_SECOND 1000        //  таймер период опроса 1 сек
#define FAN_PERIOD 10          // период опроса Fan

GyverPID regulator(25, 0.5, 0, 10); // PID регулятор
SensorDHT sensor_dht(DHT_PIN); // Сенсор
Fan fan(Fan_Pin_PWM,  Fan_STOP); // вентилятор



//mqtt
const char* mqtt_server = "test.mosquitto.org";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

JsonDocument JSONencoder;
String jsonout;
Datime work_timer;

// флаги
bool relayStateWarm = false; // флаг реле обогрева
bool relayStateRain = false; // флаг реле полива
bool relayStateLight = false; // флаг  реле освещения

unsigned long work_time = 25; //время работы освещения в сек

Datime ds(2025, 1, 30, 21, 18, 00);  // время включения освещения
uint32_t start_second =  ds.daySeconds(); // время включения освещения в сек с нач суток

uint32_t stop_second =  start_second + work_time; // время отключения освещения в сек

uint32_t period_second = 60; // период полива
uint8_t work_raine = 10; // время работы полива 

uint32_t second;
uint32_t now_second;


void ICACHE_RAM_ATTR HandleInterrupt() { 
  fan.tick();
};

void setup()
{

  Serial.begin(115200);
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  regulator.setDirection(REVERSE); // направление регулирования (NORMAL/REVERSE). ПО УМОЛЧАНИЮ СТОИТ NORMAL
  regulator.setLimits(0, 100);     // пределы (ставим для 8 битного ШИМ). ПО УМОЛЧАНИЮ СТОЯТ 0 И 255
  regulator.setpoint = SETPOINT;
  Serial.begin(115200);

  //Rely  
  

  sensor_dht.sensor_init();
  digitalWrite(Fan_STOP, 0);
 // пин тахометра вентилятора подтягиваем к VCC
  pinMode(Fan_Pin_Tach, INPUT_PULLUP);
  
  pinMode(RELAY_PIN_LIGHT, OUTPUT);
  pinMode(RELAY_PIN_RAIN, OUTPUT);
  pinMode(RELAY_PIN_WARM, OUTPUT);
 // настраиваем прерывание
 
 attachInterrupt(digitalPinToInterrupt(Fan_Pin_Tach), HandleInterrupt, FALLING);
  
  NTP.onError([]()
              {
                Serial.println(NTP.readError());
                Serial.print("online: ");
                Serial.println(NTP.online());
              });

  NTP.begin(3);
  //NTP.updateNow();
 
  client.setServer(mqtt_server, mqtt_port);
  if (client.connect("ESP"))
    {
      Serial.println("connected");
    }
    else
    {
      Serial.print("failed with state ");
      Serial.println(client.state());
      delay(2000);
    }            
};

void printPort() {
  // вывод сообщения
  Serial.print("DataTime: ");
  Serial.print(NTP.toString());
  Serial.print("T: ");
  Serial.print(sensor_dht.get_DHT().Temperature);
  Serial.print("H: ");
  Serial.print(sensor_dht.get_DHT().Humidity);
  Serial.print("Fan RPM: ");
  Serial.print(fan.getRPM(), DEC);
  Serial.print("FanReg: ");
  Serial.println(regulator.getResultTimer(), DEC);
  Serial.print("Light: ");
  Serial.print(relayStateLight);
  Serial.print("Rain: ");
  Serial.print(relayStateRain);
  Serial.print("Warm: ");
  Serial.print(relayStateWarm);
}

void mqttSend() {
  // отправка mqtt
  JSONencoder["DateStamp"] = NTP.toString();
  JSONencoder["T"] = sensor_dht.get_DHT().Temperature;
  JSONencoder["H"] = sensor_dht.get_DHT().Humidity;
  JSONencoder["Fan"] = fan.getRPM();
  JSONencoder["FanReg"] = regulator.getResultTimer();
  JSONencoder["Light"] = relayStateLight;
  JSONencoder["Rain"] = relayStateRain;
  JSONencoder["Warm"] = relayStateWarm;

  // serializeJson(JSONencoder, Serial);

  serializeJson(JSONencoder, jsonout);
  Serial.println(jsonout);

  client.publish("topic_esp", jsonout.c_str());
}


void loop()
{
  NTP.tick();
  now_second = NTP.daySeconds();
 
  // mqtt
  if(!client.loop())
    client.connect("ESP8266Client");

  
  if (millis() - timer >= PERIOD_SECOND)
  {                   // таймер 1000ms
    timer = millis(); // сброс

    // Изменение температуры
    static float T; 
    if ((sensor_dht.get_DHT().Temperature > T + 1)
      || (sensor_dht.get_DHT().Temperature > T - 1))
      {
        mqttSend();
        T = sensor_dht.get_DHT().Temperature;
    }

    

    // включение полива
    if (relayStateRain != releRain(period_second, work_raine))
    {
      relayStateRain = releRain(period_second, work_raine);
      digitalWrite(RELAY_PIN_RAIN, relayStateRain);
      mqttSend();
      }


    // включение обогрева
    if (relayStateWarm != releWarm(sensor_dht.get_DHT().Temperature, SETPOINT))
    {
      relayStateWarm = releWarm(sensor_dht.get_DHT().Temperature, SETPOINT);
      Serial.println(relayStateWarm);
      digitalWrite(RELAY_PIN_WARM, relayStateWarm);
      mqttSend();
    }
    printPort();
  }


  // включение освещения
  if (now_second > start_second && now_second < stop_second && !relayStateLight)
  {
    relayStateLight = true;
    digitalWrite(RELAY_PIN_LIGHT, relayStateLight);
    mqttSend();
  }

  if (now_second > stop_second && relayStateLight)
  {
    relayStateLight = false;
    digitalWrite(RELAY_PIN_LIGHT, relayStateLight);
    mqttSend();
  }

  if (millis() - timer_fan >= FAN_PERIOD)
  {                       // 10ms
    timer_fan = millis(); // сброс
    regulator.input = sensor_dht.get_DHT().Temperature;
    fan.SetFanLevel(regulator.getResultTimer());
    // fan.SetFanLevel(0);
  }
}