/***************************************
name:Stopwatch
function: you can see the number increases by one per second on the 4-digit 7-segment display.
***********************************/
//Email:support@sunfounder.com
//Website:www.sunfounder.com

/***************************************/
#include <TimerOne.h>
#include <Wire.h>
//----CONSTANTS--------------------------------------------//
//
//
//---------------------------------------------------------//

//----ADDRESSES--------------------------------------------//
const int CALEB = 8;
const int KHAYMAN = 10;
const int KYLE = 12;
const int NICK = 14;
const int MASTER = KHAYMAN;

//----COMM CODES--------------------------------------------//
const byte RESET_MODULE = 0;
const byte MODULE_DISARMED = 1;
const byte STRIKE = 2;
const byte TERMINATE_MODULE_FUNCTION = 3;
const long TRANSMISSION_SPEED = 9600; 

//----END CONSTANTS---------------------------------------//
byte strikeCount = 0;
bool strikeUpdated = false;

int modulesDisarmed = 0;
const int modules = 3;

int okays = 0;

//the pins of 4-digit 7-segment display attach to pin2-13 respectively 
int a = 2;
int b = 3;
int c = 4;
int d = 5;
int e = 6;
int f = 7;
int g = 8;
int p = 9;

int d4 = 10;
int d3 = 11;
int d2 = 12;
int d1 = 13;

int strike1 = A1;
int strike2 = A0;
int BUZZER_PIN = A3;

long n = 300;// n represents the value displayed on the LED display. For example, when n=0, 0000 is displayed. The maximum value is 9999. 
long t = 300;
int x = 100;
int del = 5;//Set del as 5; the value is the degree of fine tuning for the clock
int count = 0;//Set count=0. Here count is a count value that increases by 1 every 0.1 second, which means 1 second is counted when the value is 10

void setup()
{
  setupIC2();
  //set all the pins of the LED display as output
  pinMode(d1, OUTPUT);
  pinMode(d2, OUTPUT);
  pinMode(d3, OUTPUT);
  pinMode(d4, OUTPUT);
  pinMode(a, OUTPUT);
  pinMode(b, OUTPUT);
  pinMode(c, OUTPUT);
  pinMode(d, OUTPUT);
  pinMode(e, OUTPUT);
  pinMode(f, OUTPUT);
  pinMode(g, OUTPUT);
  pinMode(p, OUTPUT);
  pinMode(strike1, OUTPUT);
  pinMode(strike2, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
   
  Timer1.initialize(100000); // set a timer of length 100000 microseconds (or 0.1 sec - or 10Hz => the led will blink 5 times, 5 cycles of on-and-off, per second)
  Timer1.attachInterrupt( add ); // attach the service routine here
}

void pickDigit(int x) //light up a 7-segment display
{
  //The 7-segment LED display is a common-cathode one. So also use digitalWrite to  set d1 as high and the LED will go out
  digitalWrite(d1, HIGH);
  digitalWrite(d2, HIGH);
  digitalWrite(d3, HIGH);
  digitalWrite(d4, HIGH);

  switch(x)
  {
    case 0: 
    digitalWrite(d1, LOW);//Light d1 up 
    break;
    case 1: 
    digitalWrite(d2, LOW); //Light d2 up 
    break;
    case 2: 
    digitalWrite(d3, LOW); //Light d3 up 
    break;
    default: 
    digitalWrite(d4, LOW); //Light d4 up 
    break;
  }
}
//The function is to control the 7-segment LED display to display numbers. Here x is the number to be displayed. It is an integer from 0 to 9 
void pickNumber(int x)
{
  switch(x)
  {
    default: 
    zero(); 
    break;
    case 1: 
    one(); 
    break;
    case 2: 
    two(); 
    break;
    case 3: 
    three(); 
    break;
    case 4: 
    four(); 
    break;
    case 5: 
    five(); 
    break;
    case 6: 
    six(); 
    break;
    case 7: 
    seven(); 
    break;
    case 8: 
    eight(); 
    break;
    case 9: 
    nine();
    break;
  }
} 
void clearLEDs() //clear the 7-segment display screen
{
  digitalWrite(a, LOW);
  digitalWrite(b, LOW);
  digitalWrite(c, LOW);
  digitalWrite(d, LOW);
  digitalWrite(e, LOW);
  digitalWrite(f, LOW);
  digitalWrite(g, LOW);
  digitalWrite(p, LOW);
}

void zero() //the 7-segment led display 0
{
  digitalWrite(a, HIGH);
  digitalWrite(b, HIGH);
  digitalWrite(c, HIGH);
  digitalWrite(d, HIGH);
  digitalWrite(e, HIGH);
  digitalWrite(f, HIGH);
  digitalWrite(g, LOW);
}

void one() //the 7-segment led display 1
{
  digitalWrite(a, LOW);
  digitalWrite(b, HIGH);
  digitalWrite(c, HIGH);
  digitalWrite(d, LOW);
  digitalWrite(e, LOW);
  digitalWrite(f, LOW);
  digitalWrite(g, LOW);
}

void two() //the 7-segment led display 2
{
  digitalWrite(a, HIGH);
  digitalWrite(b, HIGH);
  digitalWrite(c, LOW);
  digitalWrite(d, HIGH);
  digitalWrite(e, HIGH);
  digitalWrite(f, LOW);
  digitalWrite(g, HIGH);
}

void three() //the 7-segment led display 3
{
  digitalWrite(a, HIGH);
  digitalWrite(b, HIGH);
  digitalWrite(c, HIGH);
  digitalWrite(d, HIGH);
  digitalWrite(e, LOW);
  digitalWrite(f, LOW);
  digitalWrite(g, HIGH);
}

void four() //the 7-segment led display 4
{
  digitalWrite(a, LOW);
  digitalWrite(b, HIGH);
  digitalWrite(c, HIGH);
  digitalWrite(d, LOW);
  digitalWrite(e, LOW);
  digitalWrite(f, HIGH);
  digitalWrite(g, HIGH);
}

void five() //the 7-segment led display 5
{
  digitalWrite(a, HIGH);
  digitalWrite(b, LOW);
  digitalWrite(c, HIGH);
  digitalWrite(d, HIGH);
  digitalWrite(e, LOW);
  digitalWrite(f, HIGH);
  digitalWrite(g, HIGH);
}

void six() //the 7-segment led display 6
{
  digitalWrite(a, HIGH);
  digitalWrite(b, LOW);
  digitalWrite(c, HIGH);
  digitalWrite(d, HIGH);
  digitalWrite(e, HIGH);
  digitalWrite(f, HIGH);
  digitalWrite(g, HIGH);
}

void seven() //the 7-segment led display 7
{
  digitalWrite(a, HIGH);
  digitalWrite(b, HIGH);
  digitalWrite(c, HIGH);
  digitalWrite(d, LOW);
  digitalWrite(e, LOW);
  digitalWrite(f, LOW);
  digitalWrite(g, LOW);
}

void eight() //the 7-segment led display 8
{
  digitalWrite(a, HIGH);
  digitalWrite(b, HIGH);
  digitalWrite(c, HIGH);
  digitalWrite(d, HIGH);
  digitalWrite(e, HIGH);
  digitalWrite(f, HIGH);
  digitalWrite(g, HIGH);
}

void nine() //the 7-segment led display 9
{
  digitalWrite(a, HIGH);
  digitalWrite(b, HIGH);
  digitalWrite(c, HIGH);
  digitalWrite(d, HIGH);
  digitalWrite(e, LOW);
  digitalWrite(f, HIGH);
  digitalWrite(g, HIGH);
}

void period()
{
  digitalWrite(a, LOW);
  digitalWrite(b, LOW);
  digitalWrite(c, LOW);
  digitalWrite(d, LOW);
  digitalWrite(e, LOW);
  digitalWrite(f, LOW);
  digitalWrite(g, LOW); 
  digitalWrite(p, HIGH);
}
/*******************************************/
void add()
{
  if(okays < 3)
  {
    return;
  }
  // Toggle LED
  count ++;
  if(count == 10)
  {
    count = 0;
    t--;
    
    if(n == 10000)
    {
      n = 0;
    }
  }
}

void setupIC2() {
  Wire.begin(MASTER);
  Wire.onReceive(receiveEvent);
//  Serial.begin(TRANSMISSION_SPEED);
}

void receiveEvent(int numBytes) {
  int x = 0;
  while (0 < Wire.available())
  {
    x += Wire.read();
  }
  
  if(x == 15)
  {
    okays++;
    return;
  }

  //int y = (x & 12) >> 2; 
  if(x == MODULE_DISARMED) {
    Serial.println("DISARMED");
      modulesDisarmed++;
  }
  
  if(x == STRIKE){
    strikeUpdated = true;
    strikeCount++;
  }
}
//}

void forwardStrike() {
  Wire.beginTransmission(CALEB);
  Wire.write(8 | strikeCount);
  Wire.endTransmission(true);
}

void sendDead() {
  Wire.beginTransmission(CALEB);
  Wire.write(12);
  Wire.endTransmission();
  Wire.beginTransmission(NICK);
  Wire.write(12);
  Wire.endTransmission();
  Wire.beginTransmission(KYLE);
  Wire.write(12);
  Wire.endTransmission();
}

void sendReset(){
  randomSeed(analogRead(A2));
  byte v = random(0,200000) % 2;
  Wire.beginTransmission(CALEB);
  Wire.write(v);
  Wire.endTransmission();
  Wire.beginTransmission(NICK);
  Wire.write(v);
  Wire.endTransmission();
  Wire.beginTransmission(KYLE);
  Wire.write(v);
  Wire.endTransmission();
}

/***************************************/ 
void loop()
{
  if(okays < 3)
  {
    return;
  }
  else if(okays == 3)
  {
    sendReset();
    okays++;
  }
   if (strikeUpdated && okays >= 3) { 
      forwardStrike();
      if(strikeCount == 3)
        tone(BUZZER_PIN, 1000, 1000); //Buzz for one second when the bomb explodes
      else
        tone(BUZZER_PIN, 1000, 250); //Buzz when the player earns a strike
      strikeUpdated = false;
   }
   else {
      strikeUpdated = false;
   }
  
  int minutes = t/60;
  minutes = minutes * 100;
  n = (t%60) + minutes;

  if(strikeCount >= 3) //If three strikes have been accrued, send dead signal
  {
    clearLEDs();
    sendDead();
    okays = 0;
  }

  if(modulesDisarmed >= modules) //If all modules are disarmed, send win signal
  {
    clearLEDs(); //TODO: should stop timer instead of clearing it
    sendDead();
    tone(BUZZER_PIN, 523, 250); //Play C
    tone(BUZZER_PIN, 659, 250); //Play E
    tone(BUZZER_PIN, 784, 250); //Play G
    okays = 0;
  }
  
  clearLEDs();//clear the 7-segment display screen
  pickDigit(0);//Light up 7-segment display d1
  pickNumber((n/1000));// get the value of thousand
  delay(del);//delay 5ms

  clearLEDs();//clear the 7-segment display screen
  pickDigit(1);//Light up 7-segment display d2
  period();
  pickNumber((n%1000)/100);// get the value of hundred
  delay(del);//delay 5ms

  clearLEDs();//clear the 7-segment display screen
  pickDigit(2);//Light up 7-segment display d3
  pickNumber(n%100/10);//get the value of ten
  delay(del);//delay 5ms

  clearLEDs();//clear the 7-segment display screen
  pickDigit(3);//Light up 7-segment display d4
  pickNumber(n%10);//Get the value of single digit
  delay(del);//delay 5ms

  /*if(modules == modulesDisarmed)
  {
    Serial.println("Winning!");
  }*/
  
  if(strikeCount >= 1)
  {
    digitalWrite(strike1, HIGH);
  }
  if(strikeCount >= 2)
  {
    digitalWrite(strike2, HIGH);
  }
}
/**************************************/ 
