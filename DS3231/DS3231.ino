#include <DS3231.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,20,4);

DS3231 Clock;
bool Century=false;
bool h12;
bool PM;
byte ADay, AHour, AMinute, ASecond, ABits;
bool ADy, A12h, Apm;
byte year, month, date, DoW, hour, minute, second;
int const beep = 4;

char buffer [16];
char time [16];

void setup() {
  Wire.begin();
  Serial.begin(9600);
  pinMode(beep,OUTPUT);
  lcd.begin(20, 4);
  lcd.init(); 
  lcd.clear(); 
  lcd.backlight(); 
//  Clock.setSecond(40);//Set the second
//  Clock.setMinute(44);//Set the minute
//  Clock.setHour(15); //Set the hour
//  Clock.setDoW(2); //Set the day of the week
//  Clock.setDate(22); //Set the date of the month
//  Clock.setMonth(7); //Set the month of the year
//  Clock.setYear(20); //Set the year (Last two digits of the year)
}

void loop() {
  int second,minute,hour,date,month,year,temperature;
  lcd.clear();
  second = Clock.getSecond();
  minute = Clock.getMinute();
  hour = Clock.getHour(h12, PM);
  date = Clock.getDate();
  month = Clock.getMonth(Century);
  year = Clock.getYear();

  sprintf(time,"20%02u-%02u-%02u",year,month,date); //設定印出01,02,而非1,2
  sprintf(buffer,"%02u:%02u:%02u",hour,minute,second); //設定印出01,02,而非1,2

  lcd.setCursor(0, 0);
  lcd.print(time);
  lcd.setCursor(0, 2);
  lcd.print(buffer);

  if(minute == 15){ // 蜂鳴器響起時間設定
    digitalWrite(beep, HIGH);
    delay(300);
    digitalWrite(beep, LOW);
    delay(300);
  }
  digitalWrite(beep, LOW);
  delay(1000);
}
