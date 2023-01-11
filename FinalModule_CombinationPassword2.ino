//----COMBINATION PASSWORD----------------------------------------------------------------------------------------------------//
//
//
//
//
//
//---------------------------------------------------------------------------------------------------------------------------//

#include <LiquidCrystal.h>
#include <TimeLib.h>
#include <Wire.h>
#include <Constants.h>
#include <stdlib.h>
#include <stdio.h>

//----CONSTANTS AND VARIABLES------------------------------------------------------------------------------------------------//

//The LCD
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

//Set constants for the LEDs
const int LED_GREEN = 9;
const int LED_RED = 8;
const int PUSH_BUTTON = 7;
int buttonState = 0;

//Set constants for the joystick
//const int SW_pin = 3;  // digital pin connected to switch output
const int X_pin = 3;     // analog pin connected to X output
const int Y_pin = 2;     // analog pin connected to Y output
int xValue = 0;
int yValue = 0;

//Pointer to show user which digit they have selected
byte pointer[8] = 
{ B00000,
  B00100,
  B01110,
  B11111,
  B00000,
  B00000,
  B00000,
  B00000};

//Blank byte to clear specific cell
byte empty[8] = 
{ B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000};

//Pointer coordinates
const int NUM_DIGITS = 5;                                                     //The number of letters in the password
const int MIN_POINTER_COORDINATE = 1;                                         //The farthest left column the pointer can occupyon the LCD
const int MAX_POINTER_COORDINATE = MIN_POINTER_COORDINATE + (NUM_DIGITS - 1); //The farthest right column the pointer can occupy on the LCD 
int pointerLocation = MIN_POINTER_COORDINATE;                                 //The column on the LCD with the pointer
int currentTumbler = 0;                                                       //The tumbler to which the pointer is currently pointing

const int MOVE_RIGHT = 1;
const int MOVE_LEFT = 0;

//Digits displayed on the LCD
char alphabet[26] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 
                     'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};
                     
char vowels[5] = {'A', 'E', 'I', 'O', 'U'};
const int numVowels = 5;
char numbers[10] = {'0', '1', '2', '0', '4', '5', '6', '7', '8', '9'};
const int numNumbers = 10;
char consonants[20] = {'B', 'C', 'D', 'F', 'G', 'H', 'J', 'K', 'L', 'M', 'N', 'P', 'Q', 'R', 'S', 'T', 'V', 'W', 'X', 'Z'};
const int numConsonants = 20;

const int LETTERS_PER_TUMBLER = 6; //The number of letters on each tumbler
struct Tumbler
{
  char letters[LETTERS_PER_TUMBLER];
  int currentLetter;
};

struct Tumbler tumbler0;
struct Tumbler tumbler1;
struct Tumbler tumbler2;
struct Tumbler tumbler3;
struct Tumbler tumbler4;
Tumbler tumblers[NUM_DIGITS] = {tumbler0, tumbler1, tumbler2, tumbler3, tumbler4};

const int ROTATE_UP = 1;
const int ROTATE_DOWN = 0;

//The correct combinations to defuse this module
String password = "";
int libraryIndex = 0;
String library[] =   //List of all possible correct words 
  {"ALPHA", "AMPLE", "ARMED", "BOLTS", "BOMBS",
   "BRAVO", "CHAOS", "COURT", "CRIME", "DAISY",
   "DAZED", "DIZZY", "EMPTY", "EXCEL", "EXILE",
   "FAITH", "FORTS", "FUZZY", "PIPES", "PIZZA",
   "QUACK", "QUEEN", "ROMEO", "RITZY", "RUINS"};
const int LIBRARY_SIZE = 25;
boolean diffused = true;

//Error messages for unlisted password
String error1 = "WRONG PASSWORD!!";
String error2 = "MODULE DISARMED";
String error3 = "INITIALIZING...";
String error4 = "KABOOM!";
//int message = MODULE_NOMINAL;

//The serial number for simon says
String serial = "XXXXX";
boolean containsVowel = false;


//----FUNCTIONS--------------------------------------------------------------------------------------------------------------//
//
void generateSerialNumber()
{
  char serialArray[NUM_DIGITS];
  int arrayID = 0;
  if(containsVowel) //The serial number contains a vowel
  {
    serialArray[0] = vowels[random(0, numVowels)];
    for(int i = 1; i <  NUM_DIGITS; i++)
    {
      arrayID = random(0, 100);
      arrayID = arrayID % 3;
      switch(arrayID)
      {
        case 1 :
          serialArray[i] = vowels[random(0, numVowels)];
          break;
        case 2 :
          serialArray[i] = consonants[random(0, numConsonants)];
          break;
        default :
          serialArray[i] = numbers[random(0, numNumbers)];
          break;
      } 
    }
  }
  else //The serial number does NOT  contain a vowel
  {
    for(int i = 0; i <  NUM_DIGITS; i++)
    {
      arrayID = random(0, 101);
      arrayID = arrayID % 2;
      switch(arrayID)
      {
        case 1 :
          serialArray[i] = consonants[random(0, numConsonants)];
          break;
        default :
          serialArray[i] = numbers[random(0, numNumbers)];
          break;
      }
    }
  }

  //permute the serial number
  char temp = 'a';
  int index = 0;
  for(int i = 0; i < NUM_DIGITS; i++)
  {
    temp = serialArray[i];
    index = random(0, (NUM_DIGITS - 1));
    serialArray[i] = serialArray[index];
    serialArray[index] = temp;
  }

  //Move array to String
  serial = "";
  for(int i = 0; i < NUM_DIGITS; i++)
  {
    serial += serialArray[i];
  }
}

//Prints the serial number on the right side of the LCD
void printSerialNumber()
{
  //11 12 13 14 15
  //    S/N
  //   XXXXX
  lcd.setCursor((12), 0);
  lcd.print("S/N");
  lcd.setCursor((16-NUM_DIGITS), 1);
  lcd.print(serial);
}

/*void clearLCD()
{
  lcd.clear();
}*/

//Sets the LCD cursor to the given coordinate. Returns true if the cursor is set
//and false otherwise.
boolean setNewCursor(int column, int row)
{
  if(column > -1 && column < 16) //Check that the column value is within the LCD range 
  {
    if(row == 0 || row == 1)  //Check that the row value is within the LCD range
    {
      lcd.setCursor(column, row); //Set the LCD cursor
      return true;
    }
  }
  return false;
}

//Randomly sets the correct combination
void setCombination(long index)
{
  password = library[index];
}

//Prints the first letter on each tumbler
void printDigits()
{
    lcd.setCursor(MIN_POINTER_COORDINATE, 0);
    for(int i = 0; i < NUM_DIGITS; i++)
    {
      lcd.print(tumblers[i].letters[tumblers[i].currentLetter]);
    }
}

//Populates each tumbler with random characters but ensures
//that each tumbler contains the neccessary character to 
//spell the password.
//
//TODO: Complexity is high
void populateTumblers()
{
  //Fill the first slot of each tumbler with the corresponding letter of the password
  for(int i = 0; i < NUM_DIGITS; i++)
  {
    tumblers[i].letters[0] = password.charAt(i);
    tumblers[i].currentLetter = 0;
  }

  //Fill the rest of the tumblers with random letters
  for(int i = 0; i < NUM_DIGITS; i++)
  {
    for(int j = 1; j < LETTERS_PER_TUMBLER; j++)
    {
      tumblers[i].letters[j] = alphabet[random(0, 25)]; //TODO: would like to make sure no tumbler contains duplicate letters
    }
  }

  //Permute each tumbler
  char temp;
  int index;
  for(int i = 0; i < NUM_DIGITS; i++)
  {
    for(int j = 0; j < LETTERS_PER_TUMBLER; j++)
    {
      index = random(0, LETTERS_PER_TUMBLER);
      temp = tumblers[i].letters[j];
      tumblers[i].letters[j] = tumblers[i].letters[index];
      tumblers[i].letters[index] = temp;
    }
  }
}

/*
//Fills every tumbler with '!' so that no tumbler contains letters
void clearTumblers()
{
  for(int i = 0; i < NUM_DIGITS; i++)
  {
    for(int j = 0; j < LETTERS_PER_TUMBLER; j++)
    {
      tumblers[i][j] = '!';
    }
  }
}
*/

//Parses input from the joystick and push button; checks the password if the push
//button is pressed while joystick is 'idle' or moves pointer or rotates tumbler
//accordingly.
void parseInput(int x, int y, int sw)
{
  if(buttonState == HIGH && x == 0 && y == 0)
    checkPassword();
  else
  {
    switch(x)   //check x-input; if x is 4 or -4, move pointer
    {
      case 4:   //move pointer right
        movePointer(MOVE_RIGHT);
        break;
      case -4:  //move pointer left
        movePointer(MOVE_LEFT);
        break;
      default:  
        switch(y)   //check y-input; if y is 4 or -4, rotate current tumbler
        {
          case -4:  //move digit "up"
            changeDigit(ROTATE_UP);
            break;
          case 4:   //move digit "down"
            changeDigit(ROTATE_DOWN);
            break;
          default:
            break;
        }
        break;
    }
  }
}

//Rotates the currently selected digit
void changeDigit(int dir)
{
  if(dir == ROTATE_UP)
  {
    if(tumblers[currentTumbler].currentLetter == 0)
      tumblers[currentTumbler].currentLetter = (LETTERS_PER_TUMBLER - 1);
    else
      tumblers[currentTumbler].currentLetter--;
  }
  else if(dir == ROTATE_DOWN)
  {
    if(tumblers[currentTumbler].currentLetter == (LETTERS_PER_TUMBLER - 1))
      tumblers[currentTumbler].currentLetter = 0;
    else
      tumblers[currentTumbler].currentLetter++;
  }
  printDigits();
}

//Moves the pointer right or left on the LCD
void movePointer(int dir)
{
  if(dir == MOVE_RIGHT)
  {
     if(pointerLocation < MAX_POINTER_COORDINATE) //Move the pointer to the right if it can move right
     {
        lcd.setCursor(pointerLocation, 1);
        lcd.write(byte(1));
        pointerLocation++;
        lcd.setCursor(pointerLocation, 1);
        lcd.write(byte(0));
        currentTumbler++;
     }
  }
  else if(dir == MOVE_LEFT)
  {
    if(pointerLocation > MIN_POINTER_COORDINATE) //Move the pointer to the left if it can move left
     {
        lcd.setCursor(pointerLocation, 1);
        lcd.write(byte(1));
        pointerLocation--;
        lcd.setCursor(pointerLocation, 1);
        lcd.write(byte(0));
        currentTumbler--;
     }
  }
}

//Prints a message onto the LCD; returns true if the message is printed and false otherwise.
boolean printMessage(int column, int row, String message)
{
  lcd.clear();
  if(setNewCursor(column, row))
  {
    lcd.print(message);
    return true;
  }
  return false;
}

//Checks to see if the tumblers match the password and illuminates
//the LEDs accordingly
void checkPassword()
{
  String str = "";
  for(int i = 0; i < NUM_DIGITS; i++)
  {
    str += tumblers[i].letters[tumblers[i].currentLetter];
  }

  //----TESTING----//
  //lcd.setCursor(11,1); //Print the String of tumbler letters for testing
  //lcd.print(str);
  
  if(str == password)
  {
    digitalWrite(LED_GREEN, HIGH);
    diffused = true;
    sendDisarm();
    lcd.clear();
    printMessage(0, 0, error2);
    delay(1000);
    lcd.clear();
    printDigits();
    printSerialNumber();
    lcd.setCursor(MIN_POINTER_COORDINATE, 1);
    lcd.write(byte(0));
  }
  else if(!contains(str))
  {
    digitalWrite(LED_RED, HIGH);
    sendStrike();
    printMessage(0, 0, error1);
    delay(1000);
    digitalWrite(LED_RED, LOW);
    lcd.clear();
    printDigits();
    printSerialNumber();
    lcd.setCursor(pointerLocation, 1); //TODO: would like point to restart where it ended
    lcd.write(byte(0));
    //currentTumbler = 0;
    //pointerLocation = MIN_POINTER_COORDINATE;
  }
  else
  {
    digitalWrite(LED_RED, HIGH);
    delay(100);
    digitalWrite(LED_RED, LOW);
  }
}

//Changes the x/y input from the joystick into more useful numbers
int treatValue(int value)
{
  return value * 9 / 1024 -4;
}

//Returns true if the String str is contained in the word library and false otherwise
boolean contains(String str)
{
  for(int i = 0; i < LIBRARY_SIZE; i++)
  {
    if(library[i] == str)
      return true;
  }
  return false;
}

//
void reset()
{
    //Set the combination required to difuse this module
    randomSeed(analogRead(0));
    long index = random(0, (LIBRARY_SIZE - 1));
    setCombination(index);
    generateSerialNumber();

    //Populate the tumblers with random letters
    populateTumblers();

    //Initialize the LCD to read the first letter of each tumbler with
    //the pointer pointing to the first tumbler
    lcd.clear();
    printDigits();
    lcd.setCursor(MIN_POINTER_COORDINATE, 1);
    lcd.write(byte(0));
    currentTumbler = 0;

    //Print the serial number (if available)
    printSerialNumber();

    //Turn off the LED's
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_RED, LOW);

    //Initialize any other variables
    diffused = false;

    //----TESTING----//
    //lcd.setCursor(11,0);  //print the password for testing
    //lcd.print(password);
}

//send a message to another board
void sendMessage(int address, int message)
{
  Wire.beginTransmission(address);
  Wire.write(message);
  Wire.endTransmission(true);
}

//Executes whenever a message is received from the master controller; parses the
//message sent by the master and performs any requested functions.
void receiveEvent(int numBytes) {
  int x = 0;
  while (0 < Wire.available())
  {
    x += Wire.read();
  }

  switch((x & 12) >> 2) {
    case RESET_MODULE:
      if((x & 1) == 1)
        containsVowel = true;
      else
        containsVowel = false;
      reset();
      break;
    case 3:
      diffused = true;
      digitalWrite(LED_RED, HIGH);
      digitalWrite(LED_GREEN, LOW);
      printMessage(0, 0, error4);
      break;
    default:
      break;
  }
}

//Alerts the master that this module threw a strike
void sendStrike() {
  Wire.beginTransmission(MASTER);
  Wire.write(8);
  Wire.endTransmission(true);
}

//Alerts the master that this module has been disarmed
void sendDisarm() {
  Wire.beginTransmission(MASTER);
  Wire.write(4);
  Wire.endTransmission(true);
}

//Alerts the master that setup is complete
void sendOK() {
  Wire.beginTransmission(MASTER);
  Wire.write(15);
  Wire.endTransmission(true);
}

//----MAIN METHODS----------------------------------------------------------------------------------------------------//

void setup() 
{
    //Initialize Communications
    Wire.begin(NICK);
    Wire.onReceive(receiveEvent);
    Serial.begin(TRANSMISSION_SPEED);
  
    //Initialize the pins for the LEDs and buttons
    pinMode(LED_GREEN, OUTPUT);
    pinMode(LED_RED, OUTPUT);
    pinMode(PUSH_BUTTON, INPUT);

    //Create special characters and begin the lcd
    lcd.createChar(0, pointer);
    lcd.createChar(1, empty);
    lcd.begin(16, 2);

    //Send OK to alert master that setup is complete
    printMessage(0, 0, error3);
    sendOK();

    //Start a new session of the module
    //reset();

    //----TESTING----//
    /*
    lcd.setCursor(0,0);  //print constants from Constants.h for testing
    lcd.print(CALEB + " ");
    lcd.print(KHAYMAN + " ");
    lcd.print(KYLE + " ");
    lcd.print(NICK);
    lcd.setCursor(0,1);
    lcd.print(RESET_MODULE);
    lcd.print(MODULE_DISARMED);
    lcd.print(STRIKE);
    */
}

void loop() 
{ 
  while(!diffused)
  {
    //read joystick values
    xValue = analogRead(X_pin);
    delay(100);
    yValue = analogRead(Y_pin);
    delay(100);
    buttonState = digitalRead(PUSH_BUTTON);

    //parse the input from the joystick
    parseInput(treatValue(xValue), treatValue(yValue), buttonState);
    delay(250);

    //-----TESTING-----//
    //buttonState = digitalRead(PUSH_BUTTON);
    //if(digitalRead(PUSH_BUTTON) == HIGH)
      //digitalWrite(LED_GREEN, HIGH);
    //else
      //digitalWrite(LED_GREEN, LOW);
  }
  /*delay(6000);
  lcd.clear();
  digitalWrite(LED_GREEN, LOW);
  while(true)
  {};*/
}
