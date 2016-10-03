#include <TaskScheduler.h>
void tInputCallback();
void tCommandCallback();
void tTimerCallback();
int adcToButtons();

Task tInput(100,TASK_FOREVER,&tInputCallback);
Task tCommand(200,1,&tCommandCallback);
Task tTimer(1000,TASK_FOREVER,&tTimerCallback);
Scheduler runner;

#include <EEPROM.h>
//______________________________________
#include <LiquidCrystal.h>
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);           // select the pins used on the LCD panel

int lcd_key     = 0;
int adValue  = 0;
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

byte smiley[8] = {
  B00000,
  B10001,
  B00000,
  B00000,
  B10001,
  B01110,
  B00000,
};
//______________________________________
void setup() {
   lcd.createChar(0, smiley);
  lcd.begin(16, 2);               // start the library  COL ROW
  lcd.setCursor(0, 0);            // set the LCD cursor   position
  lcd.print("Testing LCD Shed");  // print a simple message on the LCD
  Serial.begin(115200);
  Serial.println("Scheduler TEST");

  runner.init();
  runner.addTask(tInput);
  tInput.enable();
  runner.addTask(tTimer);
 
  tTimer.enableDelayed(3000);
  runner.addTask(tCommand);

  }
//______________________________________
void loop() {
  // put your main code here, to run repeatedly:
  runner.execute();
}
//______________________________________
void tInputCallback(){
lcd_key = adcToButtons();
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

  if (adValue > 1000) return btnNONE;

  // For V1.1 us this threshold
  if (adValue < 50)   return btnRIGHT;
  if (adValue < 250)  return btnUP;
  if (adValue < 450)  return btnDOWN;
  if (adValue < 650)  return btnLEFT;
  if (adValue < 850)  return btnSELECT;
  return btnNONE;                // when all others fail, return this.
}
//______________________________________
void tCommandCallback()
{
  lcd.setCursor(0, 1); 
  Serial.println("Cmd callB");
  switch (lcd_key) {              // depending on which button was pushed, we perform an action

    case btnRIGHT: {            //  push button "RIGHT" and show the word on the screen
        lcd.print("RIGHT ");
        break;
      }
    case btnLEFT: {
        lcd.print("LEFT   "); //  push button "LEFT" and show the word on the screen
        break;
      }
    case btnUP: {
        lcd.print("UP    ");  //  push button "UP" and show the word on the screen
        break;
    }
      
    case btnDOWN: {
        lcd.print("DOWN  ");  //  push button "DOWN" and show the word on the screen
        break;
      }
    case btnSELECT: {
        lcd.print("SELECT");  //  push button "SELECT" and show the word on the screen
        break;
      }
    case btnNONE: {
        lcd.print("NONE  ");  //  No action  will show "None" on the screen
        break;
      }
  }
//if (tCommand.isFirstIteration()){
  //tTimer.enable();
  //tCommand.delay(2000);
  //stringToLCD(0,0,"FirstCommand    ",-1); 

//}

}
//______________________________________
  
void stringToLCD( int row,int col, String string, int blanks)
{
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
static int value=0;
  value+=1;
  Serial.println("Timer");
  stringToLCD(0,0,(String)value,16); 
   
}
