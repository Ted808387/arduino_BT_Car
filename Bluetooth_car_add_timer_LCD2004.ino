#include <Wire.h> 
#include <DS3231.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <FastLED.h>

Servo myservo;
LiquidCrystal_I2C lcd(0x27,20,4);
DS3231 Clock;
#define LED_PIN     32
#define NUM_LEDS    11
CRGB leds[NUM_LEDS];

int E1 = 10;  //M1 Speed Control
int M1 = 12;  //M1 Direction Control
int E2 = 11;  //M2 Speed Control
int M2 = 13;  //M2 Direction Control
int const beep = 4;
int echoPin = 3;
int trigPin =2;
int EPin = 8;
int TPin =7;
int servo = 9;

bool Century=false;
bool h12;
bool PM;
char buffer [16];
char time [16];

int Distance = 0;
int dis = 0;
unsigned long d;
unsigned long x;
unsigned long ping() {
  digitalWrite(trigPin, HIGH);   // 觸發腳設定成高電位
  delayMicroseconds(5);          // 持續 5 微秒
  digitalWrite(trigPin, LOW);    // 觸發腳設定成低電位 
  return pulseIn(echoPin, HIGH); // 傳回高脈衝的持續時間
}
unsigned long caculate() {
  digitalWrite(TPin, HIGH);   // 觸發腳設定成高電位
  delayMicroseconds(5);          // 持續 5 微秒
  digitalWrite(TPin, LOW);    // 觸發腳設定成低電位 
  return pulseIn(EPin, HIGH); // 傳回高脈衝的持續時間
}

void setup() {
  Serial1.begin(9600);
  Serial.begin(9600);
  pinMode(echoPin, INPUT); // 輸入
  pinMode(trigPin, OUTPUT); // 輸出
  pinMode(EPin, INPUT); // 輸入
  pinMode(TPin, OUTPUT); // 輸出
  pinMode(beep, OUTPUT); // 蜂鳴器
  
  myservo.attach(servo, 500, 2400);
  myservo.write(65);
  delay(20);
  
  pinMode(M1, OUTPUT);
  pinMode(M2, OUTPUT);
  pinMode(beep, OUTPUT);
  
  lcd.begin(20, 4);
  lcd.init(); 
  lcd.clear(); 
  lcd.backlight();
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
}

void loop() {
  delay(50);
  if (Serial1.available()) {
    car();
  }else{
    Cacul_test();
    lcd.setCursor(0, 3);
    lcd.print("Capacity : ");
    lcd.print(dis);
    lcd.print("%");
  }
  delay(10);
  getLCDdisplay();
  delay(10);
  if(dis < 30){
    redFlash(); // 閃爍紅燈
  }
  else if(dis < 25){
    red(); // 閃爍紅燈
  }
  else{
    white(); // 白光呼吸
  }
  delay(10);
  Distance_test();
}
//========== Car ===============
void car(){
  char ch;
  ch = Serial1.read();
  //PC顯示Arduino所接收到的字元
  switch(ch) {
    case 'W':
      forward();
      break;
    case 'X':
      back();
      break;
    case 'A':
      right();
      break;
    case 'D':
      left();
      break;
    case 'S':
      brake(2);
      break;
    case 'B':
      beepAlert();
      break;
    case 'C':
      digitalWrite(beep, LOW);
      break;
    case 'Q':
      spin_left(0.5);
      break;
    case 'E':
      spin_right(0.5);
      break;
  }
}
//========= Beep =============
void beepAlert(){
  digitalWrite(beep, HIGH);
  delay(1000);
  digitalWrite(beep, LOW);
  delay(1000);
}
//========= Time =============

void getLCDdisplay(){ // get時間
  int second,minute,hour,date,month,year,temperature;
  second = Clock.getSecond();
  minute = Clock.getMinute();
  hour = Clock.getHour(h12, PM);
  date = Clock.getDate();
  month = Clock.getMonth(Century);
  year = Clock.getYear();

 // ===== 蜂鳴器 ======
  if(hour == 10 && minute == 30){
    beepAlert();
  }
  
  sprintf(time,"20%02u-%02u-%02u",year,month,date); //設定印出01,02,而非1,2
  sprintf(buffer,"%02u:%02u:%02u",hour,minute,second); //設定印出01,02,而非1,2
  delay(50);
//  Cacul_test();
  
  lcd.setCursor(0, 1);
  lcd.print(time);
  lcd.setCursor(0, 2);
  lcd.print(buffer);

//  lcd.setCursor(0, 3);
//  lcd.print(dis);
//  lcd.print("%");
}
//=========================== LED白光呼吸 ==============================
void white(){
  for(int j=0; j<=NUM_LEDS; j++){
    leds[j] = CRGB(150, 150, 150);
  }
  FastLED.show();
  delay(10);
}
//=========================== LED紅燈 ==============================
void red(){
  for(int j=0; j<=NUM_LEDS; j++){
    leds[j] = CRGB::Red;
  }
  FastLED.show();
}
//=========================== LED紅燈閃爍 ==========================
void redFlash(){
  for(int j=0; j<=NUM_LEDS; j++){
    leds[j] = CRGB(150, 0, 0);
  }
  FastLED.show();
  delay(200);
  for(int j=0; j<=NUM_LEDS; j++){
    leds[j] = CRGB(0, 0, 0);
  }
  FastLED.show();
  delay(200);
}
//========== Distance =========================
void Distance_test()   // 測量出前方距離
{  
  delay(5);
  d = ping() / 58;      
  Distance = d;
  delay(10);
  Serial.println(Distance);
  // MG90S
  if(Distance < 20){
    for(int i = 65; i < 166; i+=2){
      myservo.write(i);
      delay(10);
    }
    delay(2000);
  }else{
    if(myservo.read() != 65){
      for(int i = 145; i > 64; i-=2){
        myservo.write(i);
        delay(10);
      } 
    }
    myservo.write(65);
  }
}
//========== 內容量測量 =====================
void Cacul_test()   // 測量內容量
{  
  x = (caculate() * 10) / 58;      
  dis = (x / 2) - 10;
}
//========== Arduino 小車的基本動作 ==========
void forward() {  //前進
//  Serial.println("* Forward ..........");
  digitalWrite(M1, HIGH);
  digitalWrite(M2, HIGH);
  analogWrite(E1, 160);
  analogWrite(E2, 165);
  //delay(time*60);  //執行時間，可以調整
}

void back() {  //後退
//  Serial.println("* Backward ..........");
  digitalWrite(M1, LOW);
  digitalWrite(M2, LOW);
  analogWrite(E1, 160);
  analogWrite(E2, 165);
  //delay(time*100);  //執行時間，可以調整
}

void brake(int time) {  //刹車、停車
//  Serial.println("* Brake ..........");
  digitalWrite(M1, LOW);
  digitalWrite(M2, LOW);
  analogWrite(E1, 0);
  analogWrite(E2, 0);
  delay(time*100);  //執行時間，可以調整
}

void left() {  //左轉(左輪不動，右輪前進)
//  Serial.println("* Left turn ..........");
  digitalWrite(M1, HIGH);
  digitalWrite(M2, LOW);
  analogWrite(E1, 150);
  analogWrite(E2, 115);
  //delay(time*90);  //執行時間，可以調整
}

void right() {  //右轉(右輪不動，左輪前進)
//  Serial.println("* Right turn ..........");
  digitalWrite(M1, LOW);
  digitalWrite(M2, HIGH);
  analogWrite(E1, 150);
  analogWrite(E2, 115);
  //delay(time*85);  //執行時間，可以調整
}


void spin_left(int time) {  //左旋轉(左輪後退，右輪前進)
//  Serial.println("* Spin left ..........");
  digitalWrite(M1, HIGH);
  digitalWrite(M2, LOW);
  analogWrite(E1, 170);
  analogWrite(E2, 100);
  delay(time*100);  //執行時間，可以調整
}

void spin_right(int time) {  //右旋轉(右輪後退、左輪前進)
//  Serial.println("* Spin right ..........");
  digitalWrite(M1, LOW);
  digitalWrite(M2, HIGH);
  analogWrite(E1, 100);
  analogWrite(E2, 170);
  delay(time*100);  //執行時間，可以調整
}
