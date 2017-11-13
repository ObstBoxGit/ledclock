/*
 * 1. почему состояние кнопки сбрасывается в DEPRESSED не только внутри
 * ReadMainBatton() но и в обработчиках
 * 
 * 2. LDR учитывать в режимах WATCH и UNSET
 */

 
 
// pressures
#define DEPRESSED          0
#define SHORT              1
#define LONG               2

// modes
#define UNSET              0
#define WATCH              1
#define SETTING            2

// circles drawning
#define CIRCLE_MAGIC       40
#define N_CIRCLES          8
#define BLINK_MIN          100
#define BLINK_MAX          150

// pins
#define NO_LED             0
#define HOUR_TENS_LED      A0
#define HOUR_UNITS_LED     A1
//#define MINUTE_TENS_LED  A2
#define MINUTE_TENS_LED    2
//#define MINUTE_UNITS_LED A3
#define MINUTE_UNITS_LED   3
//#define LDR_PIN          A6
#define LDR_PIN            A2

#define BUTTON_PIN         4

#define LED_G              12
#define LED_B              11
#define LED_C              10
#define LED_D              9
#define LED_A              8
#define LED_F              7
#define LED_E              6

// symbols
#define SYMBOL_MINUS  253
#define SYMBOL_NONE   252

// other stuff
#define DELAY_FULL 2000 // us
#define BUTTON_THRESHOLD1  5
#define BUTTON_THRESHOLD2  50
#define LDR_THRESHOLD_MIN  600
#define LDR_THRESHOLD_MED  900
#define LDR_THRESHOLD_MAX  1010 // <= 1024

// variables
volatile struct newTime{
  unsigned char hourTens;
  unsigned char hourUnits;  
  unsigned char minuteTens;
  unsigned char minuteUnits;
  unsigned char seconds;
  boolean flag; 
} 
currentTime;

volatile struct newButton{
  unsigned char counter;
  unsigned char state;  
} 
mainButton;

volatile unsigned char deviceMode;
volatile byte temp;
volatile byte blinkCounter;
volatile byte selector;
volatile unsigned int secondsCounter;
volatile unsigned int LDRSignal;
volatile unsigned int delayOn;
volatile unsigned int delayOff;
volatile unsigned int i = 0;

// function prototypes
void LedSwitch(unsigned char Led);
void ShowSymbol(unsigned char Symbol);
void ReadMainButton();
int  ReadLDR();
void GetLedDelays();


//--------------------------------------------------
void setup() {
  digitalWrite(HOUR_TENS_LED,    HIGH);
  digitalWrite(HOUR_UNITS_LED,   HIGH);
  digitalWrite(MINUTE_TENS_LED,  HIGH);
  digitalWrite(MINUTE_UNITS_LED, HIGH); 
  digitalWrite(LED_G, HIGH);
  delay(1234);

  digitalWrite( LED_A, LOW);
  digitalWrite( LED_B, LOW);
  digitalWrite( LED_C, LOW);
  digitalWrite( LED_D, LOW);
  digitalWrite( LED_E, LOW);
  digitalWrite( LED_F, LOW);
  digitalWrite( LED_G, LOW);
  digitalWrite( 5, LOW);

  pinMode(BUTTON_PIN, INPUT);
  pinMode(LDR_PIN, INPUT);
  digitalWrite(BUTTON_PIN, HIGH);
  digitalWrite(LDR_PIN, HIGH);

  pinMode(LED_A, OUTPUT);
  pinMode(LED_B, OUTPUT);
  pinMode(LED_C, OUTPUT);
  pinMode(LED_D, OUTPUT);
  pinMode(LED_E, OUTPUT);
  pinMode(LED_F, OUTPUT);
  pinMode(LED_G, OUTPUT);
  //pinMode( 5, OUTPUT);
  pinMode(HOUR_TENS_LED, OUTPUT);
  pinMode(HOUR_UNITS_LED, OUTPUT);
  pinMode(MINUTE_TENS_LED, OUTPUT);
  pinMode(MINUTE_UNITS_LED, OUTPUT);

  currentTime.hourTens = 0;
  currentTime.hourUnits  = 7;
  currentTime.minuteTens  = 2;
  currentTime.minuteUnits  = 9;  
  currentTime.flag    = false;
  currentTime.seconds = 0;

  mainButton.counter  = 0;  
  mainButton.state = DEPRESSED;  

  delayOn = 0;
  delayOff = 0;  
  blinkCounter = 0;
  LDRSignal = 0;
  temp = 0;
  selector = 0;      
  deviceMode = UNSET;
  //deviceMode = WATCH;

}

void loop() {
  switch(deviceMode) {
  case UNSET:    

    GetLedDelays();

    LedSwitch(MINUTE_UNITS_LED);
    ShowSymbol(SYMBOL_MINUS);
    delayMicroseconds(delayOn);
    LedSwitch(NO_LED);
    delayMicroseconds(delayOff);

    LedSwitch(MINUTE_TENS_LED);
    ShowSymbol(SYMBOL_MINUS);        
    delayMicroseconds(delayOn);
    LedSwitch(NO_LED);
    delayMicroseconds(delayOff);

    LedSwitch(HOUR_UNITS_LED);
    ShowSymbol(SYMBOL_MINUS);                
    delayMicroseconds(delayOn);
    LedSwitch(NO_LED);
    delayMicroseconds(delayOff);

    LedSwitch(HOUR_TENS_LED);
    ShowSymbol(SYMBOL_MINUS);
    delayMicroseconds(delayOn);
    LedSwitch(NO_LED);
    delayMicroseconds(delayOff);

    ReadMainButton(); 

    if (mainButton.state == LONG) {
      temp = 0;
      currentTime.seconds = 0;
      mainButton.state  = DEPRESSED;
      deviceMode = SETTING;          
    }
    break;

  case WATCH:

    GetLedDelays();  // calculate delayOn and delayOff

    LedSwitch(HOUR_TENS_LED);
    ShowSymbol(currentTime.hourTens);
    delayMicroseconds(delayOn);
    LedSwitch(NO_LED);
    delayMicroseconds(delayOff);

    LedSwitch(HOUR_UNITS_LED);
    ShowSymbol(currentTime.hourUnits);
    delayMicroseconds(delayOn);
    LedSwitch(NO_LED);
    delayMicroseconds(delayOff);

    LedSwitch(MINUTE_TENS_LED);
    ShowSymbol(currentTime.minuteTens);
    delayMicroseconds(delayOn);
    LedSwitch(NO_LED);
    delayMicroseconds(delayOff);

    LedSwitch(MINUTE_UNITS_LED);
    ShowSymbol(currentTime.minuteUnits);
    delayMicroseconds(delayOn);
    LedSwitch(NO_LED);
    delayMicroseconds(delayOff);


    /*
    Serial.begin(9600);
          i++;
          if (i % 200 == 0) {
            i = 0;
            Serial.println(delayOn);
            Serial.println(delayOff);
            Serial.println("----");
            }
            */

    /*
    delay(1000);
    Serial.begin(9600);
    while(1) {      
    Serial.print("got: ");
    Serial.print(delayOn, DEC);
    Serial.print(" , ");
    Serial.println(delayOff, DEC);
    }
    */

    ReadMainButton();
    if (mainButton.state == LONG) {
      deviceMode = SETTING;
      mainButton.state  = DEPRESSED;
    }
    break;

  case SETTING:
    switch(selector) {
    case 0:            
      ShowSymbol(currentTime.hourTens);             
      digitalWrite(MINUTE_UNITS_LED, LOW);   
      digitalWrite(MINUTE_TENS_LED, LOW);                                   
      digitalWrite(HOUR_UNITS_LED, LOW); 

      if (blinkCounter++ > BLINK_MIN) {
        digitalWrite( HOUR_TENS_LED, LOW);          
        if (blinkCounter > BLINK_MAX) blinkCounter = 0;
      }
      else digitalWrite( HOUR_TENS_LED, HIGH);  

      ReadMainButton();

      if (mainButton.state == SHORT) {
        currentTime.hourTens++;
        if (currentTime.hourTens > 2) currentTime.hourTens = 0;
        mainButton.state  = DEPRESSED;
      }
      if (mainButton.state == LONG) {
        selector++;
        blinkCounter = 0;
        mainButton.state  = DEPRESSED;
      }   
      delayMicroseconds(DELAY_FULL); 
      break;

    case 1:     
      ShowSymbol(currentTime.hourUnits);             
      digitalWrite(MINUTE_UNITS_LED, LOW);   
      digitalWrite(MINUTE_TENS_LED, LOW);                                   
      digitalWrite(HOUR_TENS_LED, LOW); 

      if (blinkCounter++ > BLINK_MIN) {
        digitalWrite( HOUR_UNITS_LED, LOW);          
        if (blinkCounter > BLINK_MAX) blinkCounter = 0;
      }
      else digitalWrite( HOUR_UNITS_LED, HIGH);  

      ReadMainButton();

      if (mainButton.state == SHORT) {
        currentTime.hourUnits++;
        if (currentTime.hourTens < 2) {
          if (currentTime.hourUnits > 9) currentTime.hourUnits = 0;
        }
        else {
          if (currentTime.hourUnits > 3) currentTime.hourUnits = 0;
        }
        mainButton.state  = DEPRESSED;
      }
      if (mainButton.state == LONG) {
        selector++;
        blinkCounter = 0;
        mainButton.state  = DEPRESSED;
      }   
      delayMicroseconds(DELAY_FULL); 
      break;

    case 2:
      ShowSymbol(currentTime.minuteTens);      
      digitalWrite(MINUTE_UNITS_LED, LOW);
      digitalWrite(HOUR_UNITS_LED, LOW);             
      digitalWrite(HOUR_TENS_LED, LOW); 

      if (blinkCounter++ > BLINK_MIN) {
        digitalWrite( MINUTE_TENS_LED, LOW);          
        if (blinkCounter > BLINK_MAX) blinkCounter = 0;
      }
      else digitalWrite( MINUTE_TENS_LED, HIGH);  

      ReadMainButton();

      if (mainButton.state == SHORT) {
        currentTime.minuteTens++;
        if (currentTime.minuteTens > 5) currentTime.minuteTens = 0;
        mainButton.state  = DEPRESSED;
      }
      if (mainButton.state == LONG) {
        selector++;
        blinkCounter = 0;
        mainButton.state  = DEPRESSED;
      }   
      delayMicroseconds(DELAY_FULL); 
      break;

    case 3:
      ShowSymbol(currentTime.minuteUnits); 
      digitalWrite(MINUTE_TENS_LED, LOW);        
      digitalWrite(HOUR_UNITS_LED, LOW);             
      digitalWrite(HOUR_TENS_LED, LOW);

      if (blinkCounter++ > BLINK_MIN) {
        digitalWrite( MINUTE_UNITS_LED, LOW);          
        if (blinkCounter > BLINK_MAX) blinkCounter = 0;
      }
      else digitalWrite( MINUTE_UNITS_LED, HIGH);  

      ReadMainButton();

      if (mainButton.state == SHORT) {
        currentTime.minuteUnits++;
        if (currentTime.minuteUnits > 9) currentTime.minuteUnits = 0;
        mainButton.state  = DEPRESSED;
      }
      if (mainButton.state == LONG) {
        selector = 0;
        deviceMode = WATCH;
        blinkCounter = 0;
        mainButton.state  = DEPRESSED;
      }   
      delayMicroseconds(DELAY_FULL); 
      break;       

    default:
      break;
    }

  default:
    break;
  }  
}

void ShowSymbol(unsigned char Symbol) {
  switch(Symbol)
  {
  case 0:
    digitalWrite( LED_A, HIGH);
    digitalWrite( LED_B, HIGH);
    digitalWrite( LED_C, HIGH);
    digitalWrite( LED_D, HIGH);
    digitalWrite( LED_E, HIGH);
    digitalWrite( LED_F, HIGH);
    digitalWrite( LED_G, LOW);
    break;

  case 1:
    digitalWrite( LED_A, HIGH);
    digitalWrite( LED_B, HIGH);
    digitalWrite( LED_C, LOW);
    digitalWrite( LED_D, LOW);
    digitalWrite( LED_E, LOW);
    digitalWrite( LED_F, LOW);
    digitalWrite( LED_G, LOW);
    break;

  case 2:
    digitalWrite( LED_A, HIGH);
    digitalWrite( LED_B, LOW);
    digitalWrite( LED_C, HIGH);
    digitalWrite( LED_D, HIGH);
    digitalWrite( LED_E, LOW);
    digitalWrite( LED_F, HIGH);
    digitalWrite( LED_G, HIGH);
    break;

  case 3:
    digitalWrite( LED_A, HIGH);
    digitalWrite( LED_B, HIGH);
    digitalWrite( LED_C, HIGH);
    digitalWrite( LED_D, LOW);
    digitalWrite( LED_E, LOW);
    digitalWrite( LED_F, HIGH);
    digitalWrite( LED_G, HIGH); 
    break;

  case 4:
    digitalWrite( LED_A, HIGH);
    digitalWrite( LED_B, HIGH);
    digitalWrite( LED_C, LOW);
    digitalWrite( LED_D, LOW);
    digitalWrite( LED_E, HIGH);
    digitalWrite( LED_F, LOW);
    digitalWrite( LED_G, HIGH);
    break;

  case 5:
    digitalWrite( LED_A, LOW);
    digitalWrite( LED_B, HIGH);
    digitalWrite( LED_C, HIGH);
    digitalWrite( LED_D, LOW);
    digitalWrite( LED_E, HIGH);
    digitalWrite( LED_F, HIGH);
    digitalWrite( LED_G, HIGH);
    break;

  case 6:
    digitalWrite( LED_A, LOW);
    digitalWrite( LED_B, HIGH);
    digitalWrite( LED_C, HIGH);
    digitalWrite( LED_D, HIGH);
    digitalWrite( LED_E, HIGH);
    digitalWrite( LED_F, HIGH);
    digitalWrite( LED_G, HIGH);
    break;

  case 7:
    digitalWrite( LED_A, HIGH);
    digitalWrite( LED_B, HIGH);
    digitalWrite( LED_C, LOW);
    digitalWrite( LED_D, LOW);
    digitalWrite( LED_E, LOW);
    digitalWrite( LED_F, HIGH);
    digitalWrite( LED_G, LOW);
    break;

  case 8:
    digitalWrite( LED_A, HIGH);
    digitalWrite( LED_B, HIGH);
    digitalWrite( LED_C, HIGH);
    digitalWrite( LED_D, HIGH);
    digitalWrite( LED_E, HIGH);
    digitalWrite( LED_F, HIGH);
    digitalWrite( LED_G, HIGH);
    break;

  case 9:
    digitalWrite( LED_A, HIGH);
    digitalWrite( LED_B, HIGH);
    digitalWrite( LED_C, HIGH);
    digitalWrite( LED_D, LOW);
    digitalWrite( LED_E, HIGH);
    digitalWrite( LED_F, HIGH);
    digitalWrite( LED_G, HIGH);
    break;

  case SYMBOL_NONE: //252 
    digitalWrite( LED_A, LOW);
    digitalWrite( LED_B, LOW);
    digitalWrite( LED_C, LOW);
    digitalWrite( LED_D, LOW);
    digitalWrite( LED_E, LOW);
    digitalWrite( LED_F, LOW);
    digitalWrite( LED_G, LOW);
    break;

  case SYMBOL_MINUS: //253
    digitalWrite( LED_A, LOW);
    digitalWrite( LED_B, LOW);
    digitalWrite( LED_C, LOW);
    digitalWrite( LED_D, LOW);
    digitalWrite( LED_E, LOW);
    digitalWrite( LED_F, LOW);
    digitalWrite( LED_G, HIGH);
    break;


  case 254:
    digitalWrite( LED_A, HIGH);
    digitalWrite( LED_B, LOW);
    digitalWrite( LED_C, LOW);
    digitalWrite( LED_D, LOW);
    digitalWrite( LED_E, HIGH);
    digitalWrite( LED_F, HIGH);
    digitalWrite( LED_G, HIGH);
    break;

  case 255:
    digitalWrite( LED_A, LOW);
    digitalWrite( LED_B, HIGH);
    digitalWrite( LED_C, HIGH);
    digitalWrite( LED_D, HIGH);
    digitalWrite( LED_E, LOW);
    digitalWrite( LED_F, LOW);
    digitalWrite( LED_G, HIGH);
    break;

  default:
    break;
  }
}

void LedSwitch(unsigned char Led) {
  switch(Led) 
  {
  case HOUR_TENS_LED:
    digitalWrite(HOUR_TENS_LED,    HIGH);
    digitalWrite(HOUR_UNITS_LED,   LOW);
    digitalWrite(MINUTE_TENS_LED,  LOW);
    digitalWrite(MINUTE_UNITS_LED, LOW);            
    break;
  case HOUR_UNITS_LED:
    digitalWrite(HOUR_TENS_LED,    LOW);
    digitalWrite(HOUR_UNITS_LED,   HIGH);
    digitalWrite(MINUTE_TENS_LED,  LOW);
    digitalWrite(MINUTE_UNITS_LED, LOW);            
    break;
  case MINUTE_TENS_LED:
    digitalWrite(HOUR_TENS_LED,    LOW);
    digitalWrite(HOUR_UNITS_LED,   LOW);
    digitalWrite(MINUTE_TENS_LED,  HIGH);
    digitalWrite(MINUTE_UNITS_LED, LOW);            
    break;
  case MINUTE_UNITS_LED:
    digitalWrite(HOUR_TENS_LED,    LOW);
    digitalWrite(HOUR_UNITS_LED,   LOW);
    digitalWrite(MINUTE_TENS_LED,  LOW);
    digitalWrite(MINUTE_UNITS_LED, HIGH);            
    break;
  default:// NO_LED
    digitalWrite(HOUR_TENS_LED,    LOW);
    digitalWrite(HOUR_UNITS_LED,   LOW);
    digitalWrite(MINUTE_TENS_LED,  LOW);
    digitalWrite(MINUTE_UNITS_LED, LOW);            
  }
}

void ReadMainButton() {  
  delayMicroseconds(DELAY_FULL);         // have to wait in any case

  if (digitalRead(BUTTON_PIN) == LOW)    // is pressed
  {
    if (mainButton.counter <= BUTTON_THRESHOLD2) mainButton.counter++;
  }
  else
  {
    if ((mainButton.counter >= BUTTON_THRESHOLD1) && (mainButton.counter < BUTTON_THRESHOLD2))
    {
      mainButton.counter = 0;
      mainButton.state = SHORT;
    }
    else if ((mainButton.counter >= BUTTON_THRESHOLD1) && (mainButton.counter > BUTTON_THRESHOLD2))
    {
      mainButton.counter = 0;
      mainButton.state = LONG;
    }
    else  // mainButton.counter < BUTTON_THRESHOLD1 < BUTTON_THRESHOLD2
    {
      mainButton.counter = 0;
      mainButton.state = DEPRESSED;
    }
  }
} 

int ReadLDR() {
  int value = analogRead(LDR_PIN);
  return value;
}

void GetLedDelays() {
  LDRSignal  = analogRead(LDR_PIN);

  if      (LDRSignal  >= 0  && LDRSignal <=  400) {
    delayOn = 1980;
    delayOff = DELAY_FULL - delayOn;
  }  
  else if (LDRSignal > 400  && LDRSignal <=  800) {
    delayOn = 1200;
    delayOff = DELAY_FULL - delayOn;
  }
  else if (LDRSignal > 800  && LDRSignal <=  900) {
    delayOn = 600;
    delayOff = DELAY_FULL - delayOn;
  }
  else if (LDRSignal > 900  && LDRSignal <= 1000) {
    delayOn = 200;
    delayOff = DELAY_FULL - delayOn;
  }
  else {
    delayOn = 80;
    delayOff = DELAY_FULL - delayOn;
  }	
}

