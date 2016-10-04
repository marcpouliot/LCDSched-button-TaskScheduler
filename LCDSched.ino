/* 
  Arduino 16x2 LCD with buttons example working with the Arkhipenko/TaskScheduler

This is a very simple example on using the arkhipendo/TaskScheduler https://github.com/arkhipenko/TaskScheduler and the LCD/button shield. The intent is to comment the sketch file and understand how github works by testing pull requests.
I assume a Branch is for major versions.
Serial.print and println are used for debugging.
2016 Marc Pouliot
Note . initial master does not have comments. 
personal note : but since I did not create a branch right away in github, this version is not available anymore.)
*/
#include <TaskScheduler.h>  // library from https://github.com/arkhipenko/TaskScheduler
// callback routines
void tInputCallback();
void tCommandCallback();
void tTimerCallback();


Task tInput(100,TASK_FOREVER,&tInputCallback);  // input from buttons
Task tCommand(200,1,&tCommandCallback);         // issue the command
Task tTimer(1000,TASK_FOREVER,&tTimerCallback); // free running timer 

Scheduler runner;            // THE Scheduler

//______________________________________

#include <EEPROM.h>         // future use. check if this generates errors on compile
#include <LiquidCrystal.h>
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);           // select the pins used on the LCD panel

int LCDKey     = 0;
int adValue  = 0;         // Analog to digital value, read on input.

// NOTE : if backlit is used with this module, other a/d lines may not work properly,
// ... I lifted pin 10 since I had problems reading consisetnt temperature sensor 

#define BTNRIGHT  0
#define BTNUP     1
#define BTNDOWN   2
#define BTNLEFT   3
#define BTNSELECT 4
#define BTNNONE   5

byte smiley[8] = {
  B00000,
  B10001,
  B00000,
  B00000,
  B10001,
  B01110,
  B00000,
};
// function definitions
int adcToButtons();

//______________________________________

void setup() {
  lcd.createChar(0, smiley);
  lcd.begin(16, 2);               // start the library  COL ROW
  lcd.setCursor(0, 0);            // set the LCD cursor   position
  lcd.write(byte(0));             //smiley
  stringToLCD(0,1,"Testing LCDShed",15);  // print a simple message on the LCD
  Serial.begin(115200);
  Serial.println("Scheduler TEST");

  runner.init();
  
  runner.addTask(tInput);     // read buttons
  tInput.enable();
  
  runner.addTask(tTimer);
  tTimer.enableDelayed(3000); // WAIT 3 seconds, start seconds timer

  runner.addTask(tCommand);   // react to command

  }
//______________________________________

void loop() {
  // put your main code here, to run repeatedly:
  runner.execute();  // the scheduler greatly simplifies the loop and timing.
}
//______________________________________

void tInputCallback(){
LCDKey = adcToButtons();
Serial.print("input callB ");
Serial.println((int)adValue);

tCommand.setIterations(1);
tCommand.enable();
}
//______________________________________

int adcToButtons() {              // read the buttons
  adValue = analogRead(0);       // read the value from the sensor

  // my buttons when read are centered at these valies: 0, 144, 329, 504, 741
  // we add approx 50 to those values and check to see if we are close
  // We make this the 1st option for speed reasons since it will be the most likely result

  if (adValue > 1000) return BTNNONE;

  // For V1.1 us this threshold
  if (adValue < 50)   return BTNRIGHT;
  if (adValue < 250)  return BTNUP;
  if (adValue < 450)  return BTNDOWN;
  if (adValue < 650)  return BTNLEFT;
  if (adValue < 850)  return BTNSELECT;
  return BTNNONE;                // when all others fail, return this.
}
//______________________________________

void tCommandCallback()
{
  lcd.setCursor(0, 1); 
  Serial.println("Cmd callB");
  switch (LCDKey) {              // depending on which button was pushed, we perform an action

    case BTNRIGHT: {            //  push button "RIGHT" and show the word on the screen
        lcd.print("RIGHT ");
        break;
      }
    case BTNLEFT: {
        lcd.print("LEFT   "); //  push button "LEFT" and show the word on the screen
        break;
      }
    case BTNUP: {
        lcd.print("UP    ");  //  push button "UP" and show the word on the screen
        break;
    }
      
    case BTNDOWN: {
        lcd.print("DOWN  ");  //  push button "DOWN" and show the word on the screen
        break;
      }
    case BTNSELECT: {
        lcd.print("SELECT");  //  push button "SELECT" and show the word on the screen
        break;
      }
    case BTNNONE: {
        lcd.print("NONE  ");  //  No action  will show "None" on the screen
        break;
      }
  }
}
//______________________________________
  
void stringToLCD( int row,int col, String string, int blanks)
{
  // Blank and display the string at row, col (starts with 0.)
  // If blanks = -1 use String length to erase characters beforehand
  // otherwise erase just (number of) blanks spaces.
  int i;
  String blankStr="";
  lcd.setCursor(col,row); // initial call reverse logic.  should be row col.
  if (blanks == -1){
    blanks = string.length();
  }
  for (i = 0 ; i< blanks; i++){
    blankStr += " ";
  }
  lcd.print(blankStr);
  lcd.setCursor(col,row);
  lcd.print(string);
}
//______________________________________

void tTimerCallback(){
  // just add the seconds and display
static int value=0;
  value+=1;
  Serial.println("Timer");
  stringToLCD(0,0,(String)value,16); 
   
}
