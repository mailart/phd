//Bluetooth | Port Expander | Multiplexer | LCD |
//Current Sensor | Temperature Sensor | 2 LED's 
#include <stdlib.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <LiquidCrystal.h>
#include <Servo.h>
#include "Adafruit_MCP23017.h"

SoftwareSerial BT_Serial(8,7);//RX, TX
LiquidCrystal lcd(0,1,3,4,12,13);
Servo Servo1;
Servo Servo2;
Adafruit_MCP23017 mcp;

int tempSen = 3;
int currentSen = 3;
int s1 = 1;
int s2 = 2;
int s3 = 3;

int button1 = 7;
int button2 = 6;
int button3 = 5;
int button4 = 4;
int button1State;
int button2State;
int button3State;
int button4State;

//All relay variables below are MCP.
int air = 8;
int prePump = 9;
int vacPump = 10;
int fan = 11;
int heat = 12;
int ultra = 13;
// end of relay variables

int currentVal;
int tempVal;
int data;
int globalData;
float tempC;
float voltage;

//command cache
char *command = NULL;
bool isCommandAccepted = false;

void setup()
{
  BT_Serial.begin(9600);
  delay(250);//Stability
  BT_Serial.print("Bluetooth Enabled");
  lcd.begin(16,2);
  lcd.clear();
  mcp.begin();
  mcp.pinMode(s1,OUTPUT);
  mcp.pinMode(s2,OUTPUT);
  mcp.pinMode(s3,OUTPUT);
  mcp.pinMode(air,OUTPUT);
  mcp.pinMode(prePump,OUTPUT);
  mcp.pinMode(vacPump,OUTPUT);
  mcp.pinMode(fan,OUTPUT);
  mcp.pinMode(heat,OUTPUT);
  mcp.pinMode(ultra,OUTPUT);
  
  mcp.pinMode(button1,INPUT);   mcp.pullUp(button1,HIGH);
  mcp.pinMode(button2,INPUT);   mcp.pullUp(button2,HIGH);
  mcp.pinMode(button3,INPUT);   mcp.pullUp(button3,HIGH);
  mcp.pinMode(button4,INPUT);   mcp.pullUp(button4,HIGH);
  
  lcd.print("LCD Enabled");
  delay(2000);
  lcd.clear();
  
  //Serial.begin(9600);
}

void resetCommand(){
  if (isCommandAccepted){
    delete [] command;
    command = NULL;
    isCommandAccepted = false;
  }
}

void restructionCommand(byte readedBytes){
  resetCommand();
  char *temp = NULL;
  int commandLen = 0;
  if (command){
    temp = command;
    commandLen = strlen(command);
  }
  
  command = new char[commandLen + 2];
  memset(command, 0, commandLen + 2);
  
  if (temp)
    memcpy (command, temp, commandLen);
    
  memset(command + commandLen, readedBytes,1);
  
  if (temp)
    delete []temp;
    
}

float getTemperature(int sensorIndex){
  switch (sensorIndex){
    case 0:{
      mcp.digitalWrite(s1,LOW);
      mcp.digitalWrite(s2,LOW);
      mcp.digitalWrite(s3,LOW);
      break;
    }
    case 1:{
      mcp.digitalWrite(s1,LOW);
      mcp.digitalWrite(s2,LOW);
      mcp.digitalWrite(s3,HIGH);
      break;
    }
  }
  
  tempVal = analogRead(tempSen);
  voltage = (tempVal/1024.0) * 5.0;
  return voltage/0.01;
}

float getCurrent(int deviceIndex){
  switch (deviceIndex){
    case 0:{
      mcp.digitalWrite(s1,HIGH);
      mcp.digitalWrite(s2,LOW);
      mcp.digitalWrite(s3,LOW);
      break;
    }
    case 1:{
      mcp.digitalWrite(s1,LOW);
      mcp.digitalWrite(s2,HIGH);
      mcp.digitalWrite(s3,LOW);
      break;
    }
    case 2:{
      mcp.digitalWrite(s1,HIGH);
      mcp.digitalWrite(s2,HIGH);
      mcp.digitalWrite(s3,LOW);
      break;
    }
  }
  
  return analogRead(currentSen);
}

void loop()
{
  if(BT_Serial.available())
  {
    data = BT_Serial.read();
    globalData = data;
    restructionCommand(globalData);
    lcd.clear();
    lcd.print(command);
   // Serial.print(globalData); 
  }
  
  //button 1
  button1State = mcp.digitalRead(button1);
  if(button1State == LOW)
  {
 //   Serial.print("button1State"); 
    mcp.digitalWrite(fan,HIGH);
    lcd.print("Fan On!");
    delay(100);
    lcd.clear();
  }
  else
  {
    mcp.digitalWrite(fan,LOW);
    delay(5);
  }
  
  //button 2
  button2State = mcp.digitalRead(button2);
  if(button2State == LOW)
  {
    mcp.digitalWrite(prePump,HIGH);
    lcd.print("Pressure Pumps!");
    delay(100);
    lcd.clear();
  }
  else
  {
    mcp.digitalWrite(prePump,LOW);
    delay(5);
  }
  
  //button 3
  button3State = mcp.digitalRead(button3);
  if(button3State == LOW)
  {
    mcp.digitalWrite(heat,HIGH);
    lcd.print("Heater");
    delay(100);
    lcd.clear();
  }
  else
  {
    mcp.digitalWrite(heat,LOW);
    delay(5);
  }
  
  button4State = mcp.digitalRead(button4);
  if(button4State == LOW)
  {
    mcp.digitalWrite(ultra,HIGH);
    lcd.print("Ultrasonic");
    delay(100);
    lcd.clear();
  }
  else
  {
    mcp.digitalWrite(ultra,LOW);
    delay(5);
  }
  
  if (strcmp(command, "0") == 0) {
      //temp sensor code
      isCommandAccepted = true;
      
      tempC = getTemperature(0);
      BT_Serial.print("Temp = ");
      BT_Serial.println(tempC);
      lcd.clear();
      lcd.print("Temp = ");
      lcd.print(tempC);
      delay(250);
  }
  else if (strcmp(command, "1") == 0) {
    //Pressure Pumps Current
      isCommandAccepted = true;
      currentVal = getCurrent(0);
      BT_Serial.print("Pressure Current = ");
      BT_Serial.println(currentVal);
      lcd.clear();
      lcd.print("Pressure Current");
      lcd.setCursor(0,1);
      lcd.print(currentVal);
      delay(250); 
  }
  else if (strcmp(command, "2") == 0) {
    //Vacuum Pumps Current
      isCommandAccepted = true;
      currentVal = getCurrent(1);
      BT_Serial.print("Vacuum Current = ");
      BT_Serial.println(currentVal);
      lcd.clear();
      lcd.print("Vacuum Current");
      lcd.setCursor(0,1);
      lcd.print(currentVal);
      delay(250); 
  }
  else if (strcmp(command, "3") == 0) {
    //Servo Current
      isCommandAccepted = true;
      currentVal = getCurrent(2);
      BT_Serial.print("Servo Current = ");
      BT_Serial.println(currentVal);
      lcd.clear();
      lcd.print("Servo Current");
      lcd.setCursor(0,1);
      lcd.print(currentVal);
      delay(250); 
  }
  else if (strcmp(command, "4") == 0) {
    //temperature2
        isCommandAccepted = true;
        tempC = getTemperature(1);
        BT_Serial.print("Temperature2 = ");
        BT_Serial.println(tempC);
        lcd.clear();
        lcd.print("Temperature2 ");
        lcd.setCursor(0,1);
        lcd.print(tempC);
        delay(250); 
  }
  else if (strcmp(command, "V") == 0) {
    //get version
        isCommandAccepted = true;
        resetCommand();
        BT_Serial.println("PHD10 FW1.0");
        delay(250); 
  }
  else if (strcmp(command, "S") == 0) {
    //get version
        isCommandAccepted = true;
        resetCommand();
        float tf,th,pc,vc,sc;
        tf = getTemperature(1);
        th = getTemperature(0);
        BT_Serial.println("PHD10 FW1.0");
        delay(250); 
  }
  else if (strcmp(command, "Q") == 0) {
    //get version
        isCommandAccepted = true;
        resetCommand();
        float tf,th,pc,vc,sc;
        tf = getTemperature(1);
        th = getTemperature(0);
        BT_Serial.println("PHD10 FW1.0");
        delay(250); 
  }
      
}  

