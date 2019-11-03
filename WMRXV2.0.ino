#include <SoftwareSerial.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const int MotorPin = 2;
const int HC12_TX  = 3;
const int HC12_RX  = 4;
const int PushBttn = 6;
const int  totDist = 68;//depth of the tank in cm measured by HC-sr04 sensor
const int  lowLvl  = 10;//lower cut-off percentage to turn on motor

const long timeout = 3000;

bool MotorState   = 0;
bool prevMtrState = 0;
bool BttnState    = 0;
bool State        = 0;

byte Status     =  0;
byte prevStatus =  0;

int  WaterLvl     = 0;
int  count        = 0;

unsigned long Currtime = 0;
unsigned long Prevtime = 0;

SoftwareSerial HC12(HC12_TX, HC12_RX);
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  Serial.begin(9600);
  HC12.begin(9600);
  lcd.begin();
  pinMode(MotorPin, OUTPUT);
  pinMode(PushBttn, INPUT_PULLUP);
  lcd.setCursor(0, 0);
  lcd.print("  Water  Level  ");
  lcd.setCursor(0, 1);
  lcd.print("   Controller   ");
  delay(2000);
  lcd.setCursor(0, 0);
  lcd.print("       by       ");
  lcd.setCursor(0, 1);
  lcd.print("    Maker BR    ");
  delay(1000);
  lcd.clear();
}
void loop() {

  if (digitalRead(PushBttn) == LOW)
  {
    BttnState = !BttnState;
    delay(500);
  }
  /////////////////////////////////////Sensor Check//////////////////////////////////
  count = 0;
  do
  {
    WaterLvl = getWaterLvl();
    delay(10);
    count++;
    if (count > 5)
    {
      lcd.setCursor(0, 0);
      lcd.print("  Sensor Error  ");
      MotorState = 0;
      BttnState  = 0;
      updtMotor();
      continue;
    }
  } while (WaterLvl <= -1);
  ////////////////////////////////Display Water Level////////////////////////////////
  char buff1[16];
  sprintf(buff1, "Water Level:%3d%c", WaterLvl, '%');
  lcd.setCursor(0, 0);
  lcd.print(buff1);
  ////////////////////////////////Automatic control for motor////////////////////////
  if (WaterLvl <= lowLvl)
  {
    MotorState = 1;
  }
  else if (WaterLvl == 100)
  {
    MotorState = 0;
    BttnState  = 0;
  }
  updtMotor();
}
//////////////////////////////////////Read HC12 Data//////////////////////////////////
int getWaterLvl()
{
  int res = 0;
  //////////////////////////////////Connection check/////////////////////////////////
  do
  {
    Currtime = millis();
    if (Currtime - Prevtime >= timeout)
    {
      lcd.setCursor(0, 0);
      lcd.print("Connection Error");
      MotorState = 0;
      BttnState  = 0;
      updtMotor();
      continue;
    }
  } while (!HC12.available());
  Prevtime = Currtime;
  Status = HC12.read();
  
  if ( Status == 255)
  {
    res = 100;
  }
  else if ( (Status >= 0 )&& (Status <= 250))
  {
   Serial.print(Status);
   Serial.print("  || ");
   Serial.print(totDist);
   Serial.print("  || ");
    res = ((totDist - Status)*100)/totDist;
    
  }
  else
  {
    res = -1;
  }
  Serial.println(res); 
  return res;
}
///////////////////////////Update and display MotorState///////////////////////////////
void updtMotor()
{
  State = MotorState ^ BttnState;
  //Serial.print("  ||");
  //Serial.println(State);
  lcd.setCursor(0, 1);
  if (State != prevMtrState)
  {
    digitalWrite(MotorPin, State);
  }
  if (State == 0)
  {
    if (MotorState)
      lcd.print(" Emergency  OFF ");
    else
      lcd.print("   Motor: OFF   ");
  }
  else
    lcd.print("   Motor:  ON   ");
  prevMtrState = State;
}
///////////////////////////////////////////end/////////////////////////////////////////
