#define DHT_PIN 4
#include "DHT.h"
#include <Adafruit_Sensor.h>
#include "SensorDHT.h"
#include <Stamp.h>
#include "GyverPID.h"
#include <ESP8266WiFi.h>
#include "Fan.h"
#include <GyverNTP.h>

#define Fan_Pin_PWM 15
#define Fan_Pin_Tach 12
#define Fan_STOP 5

#define SETPOINT 24.5

uint32_t timer, timer_fan = 0; // переменная таймера
#define DHT_PERIOD 5000        // период опроса DHT
#define FAN_PERIOD 10          // период опроса Fan

// PID регулятор

GyverPID regulator(25, 0.5, 0, 10);
SensorDHT sensor_dht(DHT_PIN);
Fan fan(Fan_Pin_PWM,  Fan_STOP);

uint32_t ds;

// wifi
#define WLAN_SSID "MERCUSYS"
#define WLAN_PASS "alcm7bvn"

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
  NTP.updateNow();
};



bool ReleFlag = 0;
unsigned long work_time = 10000; //время работы в сек
unsigned long work_timer;
uint32_t start_second;
uint32_t stop_second;

void loop()
{
  

  // DHT
  if (millis() - timer >= DHT_PERIOD)
  {                   // таймер 1000ms
    timer = millis(); // сброс

    Datime dt(2025, 1, 30, 14, 14, 30);
    // или Datime dt(NTP)
    Serial.print("DataTime: ");
    Serial.print(dt.toString());

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
    // fan.SetFanLevel(20);
  }

  // now = datetime.datetime.now()
  // todayon = now.replace(hour=time_obj.tm_hour,
  //                       minute=time_obj.tm_min, second=0, microsecond=0)
  // seconds = (now - todayon).total_seconds()

  // if now >= todayon and seconds < timeReleWork and not ReleState or now > todayon and seconds > timeReleWork and ReleState:
  // ReleState = not ReleState
  // GPIO.output(RELE_PIN, ReleState)
  
  Datime dt = NTP;

  Datime ds(2025, 1, 30, 14, 14, 30);
  Datime dn(2025, 1, 30, 14, 14, 40);
   
  start_second =  ds.daySeconds(); 
  stop_second =  dn.daySeconds();
  // проверить 
  //stop_second =  start_second + work_time;
  
  NTP.daySeconds();
  
  uint32_t ss = StampUtils::timeToSeconds(12, 35, 0);
  //stop_second = ds.addSeconds(work_timer);
  //включение освещения

  Stamp s;
  Datime d = s.now(); 

  if ( NTP > DaySeconds(12, 35, 0) && ()  && !ReleFlag)
  {
    ReleFlag = true;
    Serial.println("ON");
    Datime work_timer = NTP;
    work_timer.addSeconds(work_time);
  }

  if (NTP > work_timer && ReleFlag)
  {
    ReleFlag = false;
    Serial.println("OFF");
  }
}
