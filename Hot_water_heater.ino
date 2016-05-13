/* Hot water automation written for Arduino by Chase
 *
 * Licenced under the GNU GPL Version 3.
 * Contact: anona@privacyrequired.com
 * www.doin.science
 * https://github.com/tan-tan-san
 *
 */


#include <Wire.h>
#include "Sodaq_DS3231.h"
#include <SPI.h>
#include <Servo.h>

//Define temp and time vars
float maxTemperature=0;
float minTemperature=200;
char charMinTemperature[10];
char charMaxTemperature[10];
char timeChar[100];
char dateChar[50];
char temperatureChar[10];
float temperature = 0;
float previousTemperature = 0;
String dateString;
int minuteNow=0;
int minutePrevious=0;
uint32_t old_ts;

// Define manual override pushbuttons
const int buttonPinOn = 2;     
const int buttonPinOff = 3;

//servo and LED vars
Servo ServoMotor;
int Redpin = 13;
int Greenpin = 12;

//Foward declaring functions
void HeatWater(int heat);

void setup () {
  
    Serial.begin(57600);
    Wire.begin();
    rtc.begin();
    ServoMotor.attach(11);
    pinMode(Redpin, OUTPUT);
    pinMode(Greenpin, OUTPUT);
    pinMode(buttonPinOn, INPUT);
    pinMode(buttonPinOff, INPUT);
    //setRTCTime();    //Uncomment and run this once to set the time on your RTC.
}

void loop () {
  delay(1000);
  DateTime now = rtc.now();
  int n = now.hour();
    if ( (n >= '5' && n <= '9') || (n >= '17' && n <= '21' ) )
    {
      HeatWater(1);
    } else {
      HeatWater(0);
    }   
  rtc.convertTemperature();
  float temperature = Celcius2Fahrenheit(rtc.getTemperature());
  uint32_t ts = now.getEpoch();
  String hours = String(now.hour());
    
  //manual override with button press with flashing LED
  if (buttonPinOn, HIGH) {
    digitalWrite(Greenpin, HIGH);
    delay(100);
    digitalWrite(Greenpin, LOW);
    delay(100);
    digitalWrite(Greenpin, HIGH);
    delay(100);
    digitalWrite(Greenpin, LOW);
    delay(100);
    digitalWrite(Greenpin, HIGH);
    HeatWater(1);
    Serial.println("Manual Override ON, Water heater on HIGH");
  }
  else if (buttonPinOff, HIGH) {
    digitalWrite(Redpin, HIGH);
    delay(100);
    digitalWrite(Redpin, LOW);
    delay(100);
    digitalWrite(Redpin, HIGH);
    delay(100);
    digitalWrite(Redpin, LOW);
    delay(100);
    digitalWrite(Redpin, HIGH);
    ServoMotor.write(90);
    Serial.println("Manual Override ON, Water heater on HIGH");
  }
    if (old_ts == 0 || old_ts != ts) {
  old_ts = ts;
  
  minuteNow = now.minute();
  if(minuteNow!=minutePrevious)
  {
    dateString = getDayOfWeek(now.dayOfWeek())+", ";
    dateString = dateString+String(now.date())+"/"+String(now.month());
    dateString= dateString+"/"+ String(now.year()); 
    minutePrevious = minuteNow;
   //##original code declaration of String hours = String(now.hour());
    if(now.minute()<10)
    {
      hours = hours+":0"+String(now.minute());
    }
    else {
      hours = hours+":"+String(now.minute());
    }
    
    hours.toCharArray(timeChar,100);
    dateString.toCharArray(dateChar,50);
  }
  
  if(temperature != previousTemperature)
    {
    previousTemperature = temperature;
    String temperatureString = String(temperature,0);
    temperatureString.toCharArray(temperatureChar,10);

    if(temperature>maxTemperature)
    {
      maxTemperature = temperature;
    }
    if(temperature<minTemperature)
    {
      minTemperature = temperature;
    }
  }
}

}


String getDayOfWeek(int i) //possibly forward declare
{
  switch(i)
  {
    case 1: return "Monday";break;
    case 2: return "Tuesday";break;
    case 3: return "Wednesday";break;
    case 4: return "Thursday";break;
    case 5: return "Friday";break;
    case 6: return "Saturday";break;
    case 7: return "Sunday";break;
    default: return "Monday";break;
  }
}

float Celcius2Fahrenheit(float celsius)
{
  return 1.8 * celsius + 32;
}

void setRTCTime()
{
  DateTime dt(2015, 8, 27, 9, 35, 30, 4); //Year, Month, Day, Hour, Minutes, Seconds, Day of the Week
  rtc.setDateTime(dt); //Adjust date-time as defined 'dt' above 
}

void HeatWater(int heat) { // servo to move the temp gauge between LOW and HIGH.
  if (heat) {
  digitalWrite(Redpin, LOW);
  digitalWrite(Greenpin, HIGH);
  ServoMotor.write(90);
  Serial.println("Water heater on HIGH");
  }
  else { 
    digitalWrite(Redpin, HIGH);  //timeofday = dont need water heated
    digitalWrite(Greenpin, LOW);
    ServoMotor.write(11);
    Serial.println("Water heater on LOW");
  }
}


