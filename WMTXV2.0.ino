#include<SoftwareSerial.h>

const int HC12_Tx  = 0;// HC12 TX_pin not needed for only sending the data
const int HC12_Rx  = 1;// HC12 RX_pin
const int sensPin  = 2;
const int echoPin  = 3;
const int trigPin  = 4;

bool full  =  0;
byte wtrLvl = 0;


SoftwareSerial HC12(HC12_Tx, HC12_Rx);

void setup() {
  // put your setup code here, to run once:
  HC12.begin(9600);
  pinMode(sensPin, INPUT);
  pinMode(echoPin, INPUT);
  pinMode(trigPin, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  full = digitalRead(sensPin);
  if (!full)
  {
    HC12.write(255);
    delay(200);
  }
  else
  {
    wtrLvl = getwtrLvl();
    wtrLvl = constrain(wtrLvl, 0, 250);
    HC12.write(wtrLvl);
    delay(200);
  }
}

byte getwtrLvl()
{
  long dist;

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  dist = pulseIn(echoPin, HIGH);
  dist = dist / 28 / 2;
  dist = dist *1.25;
  return dist;
}

