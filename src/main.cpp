#define DHT_PIN 4
#include "DHT.h"
#include <Adafruit_Sensor.h>
#include "SensorDHT.h"

#include "GyverPID.h"
#include <ESP8266WiFi.h>
#include "Fan.h"
#include <GyverNTP.h>

#define Fan_Pin_PWM 15
#define Fan_Pin_Tach 12
#define Fan_STOP 5

#define SETPOINT 24.5

uint32_t timer, timer_fan = 0; // переменная таймера
#define PERIOD_SECOND 1000        // период опроса DHT
#define FAN_PERIOD 10          // период опроса Fan

// PID регулятор

GyverPID regulator(25, 0.5, 0, 10);
SensorDHT sensor_dht(DHT_PIN);
Fan fan(Fan_Pin_PWM,  Fan_STOP);



// wifi
#define WLAN_SSID "MERCUSYS"
#define WLAN_PASS "alcm7bvn"

void ICACHE_RAM_ATTR HandleInterrupt() { 
  fan.tick();
};
Datime work_timer;

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

  sensor_dht.sensor_init();
  digitalWrite(Fan_STOP, 0);
 // пин тахометра вентилятора подтягиваем к VCC
  pinMode(Fan_Pin_Tach, INPUT_PULLUP);

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
 
};



bool ReleLightFlag = 0; // флаг включения реле
unsigned long work_time = 25; //время работы в сек
// unsigned long work_timer;
uint32_t start_second;
uint32_t stop_second;
uint32_t second;
uint32_t now_second;

uint32_t period_second = 60; // период полива
uint8_t work_raine = 10; // период полива
bool ReleRainFlag = 0; // флаг включения реле полива
Datime ds(2025, 1, 30, 21, 18, 00);  // время включения освещения

uint8_t s;

void loop()
{
  NTP.tick();
  now_second = NTP.daySeconds();
 
   
  start_second =  ds.daySeconds(); 
  stop_second =  start_second + work_time;

  // DHT
  if (millis() - timer >= PERIOD_SECOND)
  {                   // таймер 1000ms
    timer = millis(); // сброс
  
    Serial.print(" DataTime: ");
    Serial.print(start_second-now_second);
    Serial.print(" DataTime: ");
    Serial.print(NTP.toString());
    Serial.print(" H: ");
    Serial.print(sensor_dht.get_DHT().Humidity);
    Serial.print(" T: ");
    Serial.print(sensor_dht.get_DHT().Temperature);
    Serial.print(" Fan RPM:");
    Serial.print(fan.getRPM(), DEC);
    Serial.print(" reg ");
    Serial.println(regulator.getResultTimer(), DEC); 
    
  }

  if (millis() - timer_fan >= FAN_PERIOD)
  {                       // 10ms
    timer_fan = millis(); // сброс
    regulator.input = sensor_dht.get_DHT().Temperature;
    fan.SetFanLevel(regulator.getResultTimer());
    //fan.SetFanLevel(0);
  }

  // включение освещения
  if (now_second > start_second && now_second < stop_second && !ReleLightFlag )
  {
    ReleLightFlag = true;
    Serial.println("ON");
  }

  if (now_second > stop_second && ReleLightFlag  )
  {
    ReleLightFlag = false;
    Serial.println("OFF");
  }

   // включение полива
  // if (NTP.newSecond()) {
  //   s=s+1;
  //   Serial.println(s);
  //  if (s > period_second && !ReleRainFlag) {
  //    s=0;
  //    ReleRainFlag = true;
  //    Serial.println("rain on");
  //  }
  //  if (s>work_raine && ReleRainFlag) { 
  //    ReleRainFlag = false;
  //    Serial.println("rain off");
  //    s=0;
  //  }
  // }

   // включение полива
  if (NTP.newSecond()) {
    s++;
    Serial.println(s);
   if ((s > period_second && !ReleRainFlag) || (s>work_raine && ReleRainFlag))  {
     s=0;
     ReleRainFlag = !ReleRainFlag;
     Serial.println("rain flag ");
     Serial.print(ReleRainFlag);
   }
   
  }



}
