/*******************************************************
     LCD and Keypad on ESP32S
     For Elegoo Smart Car
********************************************************/
// Author:    Dan Nadeau
// Date:      Nov 10,2020
// Last Rev:  Nov 20,2020

#include <LiquidCrystal.h>

// select the pins used on the LCD panel
const int rs = 32, en = 33, d4 = 14, d5 = 27, d6 = 26, d7 = 25;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);     // instantiate a display panel and call it lcd 

// define some values used by the panel and buttons
#define BUZZER_PIN          12
#define SOUND_PWM_CHANNEL   0
#define SOUND_RESOLUTION    8 // 8 bit resolution
#define SOUND_ON            (1<<(SOUND_RESOLUTION-1)) // 50% duty cycle
#define SOUND_OFF           0
#define btnLEFT   0     // values assigned from key_in
#define btnRIGHT  1
#define btnUP     2
#define btnDOWN   3
#define btnSELECT 4
#define btnNONE   5
int adc_key_in  = 36;   // keyboard adc input pin #
int key_in     = 0;     // key value read in
int lcd_key = 0;        // value returned from read_LCD_buttons
bool sound = false;     // sound off by default
//------------------------------------------------------
void tone(int pin, int frequency, int duration)
{
  ledcSetup(SOUND_PWM_CHANNEL, frequency, SOUND_RESOLUTION);  // Set up PWM channel
  ledcAttachPin(pin, SOUND_PWM_CHANNEL);                      // Attach channel to pin
  ledcWrite(SOUND_PWM_CHANNEL, SOUND_ON);
  delay(duration);
  ledcWrite(SOUND_PWM_CHANNEL,SOUND_OFF);
}
//----------------------------------------
String menu[] = {
"** Smart Car ** ",         // Title
"Auto            ",         // 1
   " Line            ",     // 2
   " Roam            ",     // 3 
"Manual          ",         // 4
   " Bluetooth       ",     // 5
   " IR Remote       ",     // 6
"Settings        ",         // 7 
   "Sonar Scan      ",      // 8
      " Single         ",   // 9
      " Continuous     ",   //10
   "Sounds        ",        //11
      " On             ",   //12
      " Off            ",   //13
};
int Llist[][5] = //Link List {L,R,U,D,Select}
{
  {14,14,14,14,14},   //size of array
  {0,2,0,4,0},   //1  Auto
  {1,0,0,3,1},   //2    Line
  {1,0,2,0,1},   //3    Roam
  {0,5,1,7,0},   //4  Manual
  {4,0,0,6,1},   //5    Bluetooth
  {4,0,5,0,1},   //6    IR
  {0,8,4,0,0},   //7  Settings
  {7,9,0,11,0},  //8    Sonar Scan
  {8,0,8,10,1},  //9      Single
  {8,0,9,0,1},   //10     Continuous
  {7,12,8,0,1},  //11   Sound
  {11,0,0,13,1}, //12     On
  {11,0,12,0,1}   //13    Off
};
int item_index = 1; 
// make some custom arrow characters:
byte left[8] = {
  0b00000,
  0b00100,
  0b01000,
  0b11111,
  0b01000,
  0b00100,
  0b00000,
  0b00000
};
byte right[8] = {
  0b00000,
  0b00100,
  0b00010,
  0b11111,
  0b00010,
  0b00100,
  0b00000,
  0b00000
};
byte up[8] = {
  0b00000,
  0b00100,
  0b01110,
  0b10101,
  0b00100,
  0b00100,
  0b00100,
  0b00000
};
byte down[8] = {
  0b00000,
  0b00100,
  0b00100,
  0b00100,
  0b10101,
  0b01110,
  0b00100,
  0b00000
};
byte rtn[8] = {
  0b00000,
  0b00001,
  0b00001,
  0b00101,
  0b01001,
  0b11111,
  0b01000,
  0b00100
};
//----------------------------------------
int read_LCD_buttons(){
  key_in = analogRead(adc_key_in);           // read the value from the sensor
  if (key_in > 4000) return btnNONE;         // 1st option for speed reasons since most likely result
  if (key_in < 200)   return btnLEFT;
  if (key_in < 1000)  return btnDOWN; 
  if (key_in < 1500)  return btnUP;
  if (key_in < 2000)  return btnRIGHT;
  if (key_in < 3000)  return btnSELECT;
  return btnNONE;                           // when all others fail, return this...
}
//------------------------------------------------------
void adjust_menu_cursor(int key){           // adjust item index according to the button pressed
  int count = 0;
  if(Llist[item_index][key] != 0 ){         // if there is a link
     item_index = Llist[item_index][key];   // get linked item's index
     tone(12,2960,30);                      // Positive feedback
     delay(50);
     count++;                               // count changes
  }
  else tone(12,200,100);     delay(50);     // negative feedback
}
//------------------------------------------------------
void print_arrows(){
  for (int i = 0; i < 5; i++){
    if( Llist[item_index][i] != 0){
         lcd.write(byte(i)); 
    }
  }
}
//------------------------------------------------------
void setup(){
  Serial.begin(115200);
  lcd.begin(16, 2);                           // enable lcd (16 Char, 2 lines)
  // create a new character
  lcd.createChar(0, left);
  // create a new character
  lcd.createChar(1, right);
  // create a new character
  lcd.createChar(2, up);
  // create a new character
  lcd.createChar(3, down);
  // create a new character
  lcd.createChar(4, rtn);
  lcd.setCursor(0,0);
  lcd.print(menu[0]);
  lcd.setCursor(0,1);                       // move to the begining of the second line
  lcd.print(menu[item_index]);              // print first menu item
  lcd.setCursor(12,1);                      // move to the begining of the second line
  print_arrows();
}
//--------------------------------------------------------
void loop(){
  lcd_key = read_LCD_buttons();            // read the buttons
  switch (lcd_key)                         // go adjust menu row and column
  {
    case btnRIGHT: case btnLEFT: case btnUP: case btnDOWN:
     {
       adjust_menu_cursor(lcd_key);        // adjust the item_index pointer
       lcd.setCursor(0,1);                 // move to the begining of the second line
       lcd.print(menu[item_index]);        // print a menu item
       lcd.setCursor(12,1);                // move to the end of the second line
       print_arrows();


       if( Llist[item_index][0] != 0 ){    // if not 0 then it is a subset
          lcd.setCursor(0,0);              // display  group title
          lcd.print(menu[Llist[item_index][0]]); 
       }
       if( Llist[item_index][0] == 0 ){    // if 0 then at top level
          lcd.setCursor(0,0);              //  display title
          lcd.print(menu[0]);
       }
       break;
     }

     case btnSELECT:
     {
          if( Llist[item_index][4] != 0 ){  
             process();
             break;
          }
          else{
             negative_tone();            // negative feedback  
          }
     }
     
     case btnNONE:                      // nothing, just continue
     {
        break;
     }
  }
  delay(500);
}
//-------------------------------------------
// process Selection

void process(){
      char option = 'c';             // local variables
      bool again = true;
      select_tone();
      
       lcd.home();
       lcd.print(menu[item_index]);     // print a menu item
       lcd.setCursor(0,1);              // move to the begining of the second line
       lcd.write(byte(0)); 
       lcd.print("                ");
       delay(30);
       lcd.setCursor(1,1);
       lcd.print("Cancel    Go");
       lcd.write(byte(1)); 

       while(again == true)
       {
         lcd_key = read_LCD_buttons();    // read the buttons
         switch(lcd_key)
         { 
            case btnUP: case btnDOWN: case btnSELECT:
            {
             negative_tone();                   // negative feedback 
             break; 
            }
            
            case btnRIGHT:
            {
               positive_tone();
               option = 'g';
               again = !again;
               break; 
            }
            case btnLEFT:                
            {
              positive_tone();
              option = 'c';
              again = !again;
              break;  
            }

           case btnNONE:                      // nothing, scan again
           {
               break;
           }
         }  
      }
      if(option != 'c' && option != 'g'){     // signal invalid key
         negative_tone();                     // negative feedback
      }
      else if(option == 'c'){ 
         lcd.setCursor(0,1);              // move to the begining of the second linebreak;
         lcd.print(menu[item_index]);     // print a menu item             
         lcd.setCursor(12,1);             // move near the end of the second line
         print_arrows();
      }
      else {                              // go was selected
         lcd.setCursor(0,1);              // move to the begining of the second line
         lcd.print("SELECTED        ");   // indicate Select button chosen
         select_tone();
         
//    *****   send command to Uno here *****
      }
}
//-------------------------------------
void select_tone(){
       tone(12,2860,30);                // Select feedback
       delay(100);
       tone(12,2960,30);
       delay(100);
       tone(12,3060,30);
       delay(100);
}
//-------------------------------------
void positive_tone(){
       tone(12,200,100);
       delay(50);                     // positive feedback
}
//-------------------------------------
void negative_tone(){
       tone(12,200,100);
       delay(50);                     // negative feedback
}
