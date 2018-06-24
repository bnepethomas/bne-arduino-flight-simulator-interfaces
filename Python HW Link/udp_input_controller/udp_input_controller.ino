/* 
Heavily based on 
https://github.com/calltherain/ArduinoUSBJoystick

Instead of sending to USB - sends over UDP

Mega2560 R3, digitalPin 22~ 37 used as row0 ~ row 15, 
digital pin 38~53 used as column 0 ~ 15,
it's a 16 * 16  matrix, 

row 0, 1, 2 ,3 will be used to support 32 rotary encoder
row 4, 5 will be used to support 16 On - off - On toggle switches, 
note: this application will make the On-off-on toggle switch generate a button push signal when toggle from On to off, 
but  this is not implemented yet
for normal on- on toggle switch or if you don't need this feature then just plug the switch to push button area
row 6~15 will be used to support push button or normal on-on toggle switch

If using 5100 series Ethernet Shield may need to remove C3 as per 
https://forum.arduino.cc/index.php?topic=99880.15

*/

#define NUM_BUTTONS 256
#define NUM_AXES  8        // 8 axes, X, Y, Z, etc
#define STATUS_LED_PORT 7
#define FLASH_TIME 100

// 
typedef struct joyReport_t {
  int16_t axis[NUM_AXES];
  uint8_t button[(NUM_BUTTONS+7)/8]; // 8 buttons per byte
};

const unsigned int ScanDelay = 4;

joyReport_t joyReport;
joyReport_t prevjoyReport;

/*
RotaryEncoder
BA BA BA BA
CW >>>  00 01 11 10    <<< CCW
*/

/*
this value sets the threshold for the CW/CCW drift in milliseconds
any direction drift within this time limit will be ignored, the previous direction will be used.
*/
byte CWCCWDriftTimeLimit = 20;

//totally 32 rotary encoders, 
const byte ENCODER_COUNT = 32;


//store the rotary encoder's AB pin value in last scan
byte prevEncoderValue[ENCODER_COUNT];

//store the delay counter for each encoder
byte EncoderPulseDelayCounter[ ENCODER_COUNT * 2];
byte StateDelayLength = 30;

//store the rotary encoder's last effective CW/ CCW state
int prevEncoderDirection[ENCODER_COUNT];

//indicate the current stage  of thie encoder
// for every pulse, left shift stage 2 bits and append the new 2 bits at the lower end, 
// and compare the value with B00011110 and B00101101, then it's CW or CCW, 
// if it's CW or CCW, reset stage to 0
byte EncoderState[ENCODER_COUNT];

//const byte CWPattern1 = B00011110, CWPattern2 = B01111000, CWPattern3 = B11100001, CWPattern4 = B10000111;
//const byte CCWPattern1 = B00101101, CCWPattern2 = B10110100, CCWPattern3 = B11010010, CCWPattern4 = B01001110;
const byte CWPattern1 = B00000001, CWPattern2 = B00000111, CWPattern3 = B00001110, CWPattern4 = B00001000;
const byte CCWPattern1 = B00000010, CCWPattern2 = B00001011, CCWPattern3 = B00001101, CCWPattern4 = B00000100;
//const byte CWCCWCompareMask = B11111111;
const byte CWCCWCompareMask = B00001111;

//store the rotary encoder's last pulse time, this will be used to remove the drift effect
unsigned long prevEncoderMillis[ENCODER_COUNT];

unsigned long prevLEDTransition = millis();


//unsigned long loopTime;

void setup() 
{
  
  // Output Port for Status Monitoring
  pinMode(STATUS_LED_PORT, OUTPUT);
  digitalWrite(STATUS_LED_PORT, LOW);
  
  
  for( int portId = 22; portId < 38; portId ++ )
  {
    pinMode( portId, OUTPUT );
  }
  for( int portId = 38; portId < 54; portId ++ )
  {
    pinMode( portId, INPUT_PULLUP);
  }
  PORTD = B10000000;
  PORTG = B00000111;
  PORTL = B11111111;
  PORTB = B00001111;

  Serial.begin(115200);
  delay(200);

  /*================axis and key state initialization=======================*/
  for (uint8_t ind=0; ind < NUM_AXES; ind++) {
    joyReport.axis[ind] = 0;
  }

  for (uint8_t ind=0; ind<sizeof(joyReport.button); ind++) {
    joyReport.button[ind] = 0;
  }

  for ( int i = 0; i < ENCODER_COUNT; i++)
  {
    prevEncoderMillis[i] = millis();
  }
  /*---------------------------------------*/

  

}

// Send an HID report to the USB interface
void sendJoyReport(struct joyReport_t *report)
{
  //check if time has elapsed enough time since last report sent.
  long static prevMillis;
  //if ( millis() - prevMillis >= 10 )
  //{
    if (memcmp( report, &prevjoyReport, sizeof( joyReport_t ) ) != 0)
    {
      //Serial.write((uint8_t *)report, sizeof(joyReport_t));
      memcpy ( &prevjoyReport, report, sizeof( joyReport_t ) );
    }


    for ( int buttonid = 0; buttonid < 4; buttonid ++ )
    {
      Serial.print(buttonid);
      if (joyReport.button[buttonid] == 1)
        Serial.println("-1 ");
      else
        Serial.println("-0 ");    
    }
          
    Serial.print("Timecheck ");
    Serial.println(millis());

    delay(300);

//  Serial.println( millis() - prevMillis );
  //  prevMillis = millis();
  //}
}

void loop() 
{
  //Serial.println( millis() - loopTime );
  //loopTime = millis();

  //scan rotary encoder

  PORTC = 0xFF;
  for ( int rowid = 0; rowid < 4; rowid ++ )
  {
    //turn on the current row
    PORTA = ~( B00000001 << rowid);

    //we must have such a delay so the digital pin output can go LOW steadily, 
    //without this delay, the row PIN will not 100% at LOW during the scan,
    //so check the first column pin's value will return incorrect result.
    delayMicroseconds(ScanDelay);

    byte colResult[16];

    //pin 38, PD7
    colResult[0] = (PIND & B10000000 )== 0 ? 1 : 0;
    //pin 39, PG2
    colResult[1] = (PING & B00000100 )== 0 ? 1 : 0;
    //pin 40, PG1
    colResult[2] = (PING & B00000010 )== 0 ? 1 : 0;
    //pin 41, PG0
    colResult[3] = (PING & B00000001 )== 0 ? 1 : 0;

    //pin 42, PL7
    colResult[4] = (PINL & B10000000 )== 0 ? 1 : 0;
    //pin 43, PL6
    colResult[5] = (PINL & B01000000 )== 0 ? 1 : 0;
    //pin 44, PL5
    colResult[6] = (PINL & B00100000 )== 0 ? 1 : 0;
    //pin 45, PL4
    colResult[7] = (PINL & B00010000 )== 0 ? 1 : 0;

    //pin 46, PL3
    colResult[8] = (PINL & B00001000 )== 0 ? 1 : 0;
    //pin 47, PL2
    colResult[9] = (PINL & B00000100 )== 0 ? 1 : 0;
    //pin 48, PL1
    colResult[10] =( PINL & B00000010)== 0 ? 1 : 0;
    //pin 49, PL0
    colResult[11] =( PINL & B00000001)== 0 ? 1 : 0;

    //pin 50, PB3
    colResult[12] =( PINB & B00001000)== 0 ? 1 : 0;
    //pin 51, PB2
    colResult[13] =( PINB & B00000100)== 0 ? 1 : 0;
    //pin 52, PB1
    colResult[14] =( PINB & B00000010)== 0 ? 1 : 0;
    //pin 53, PB0
    colResult[15] =( PINB & B00000001)== 0 ? 1 : 0;

    for ( int colid = 0; colid < 16; colid += 2 )
    {
      byte encoderValue = colResult[colid] << 1 | colResult[colid + 1];
      
      //byte encoderValue = digitalRead(colid+38) << 1 | digitalRead(colid + 39);
      
      byte encoderId = rowid * 8 + colid / 2;
      
      //int encoderState = encoderTable[ prevEncoderValue[ encoderId ] << 2 | encoderValue ];

      int direction = 0;
      if ( encoderValue != prevEncoderValue[encoderId]  )
      {
        //we have a "pulse" from rotary encoder
        EncoderState[encoderId] = (EncoderState[encoderId] << 2) | encoderValue;

        if ((EncoderState[encoderId] & CWCCWCompareMask) == CWPattern1  
          || (EncoderState[encoderId] & CWCCWCompareMask) == CWPattern3 
          || (EncoderState[encoderId] & CWCCWCompareMask) == CWPattern2  
          || (EncoderState[encoderId] & CWCCWCompareMask) == CWPattern4)
        {
          //this is a full CW pattern,
          direction = 1;
          //EncoderState[encoderId] = 0;
        }
        else if ((EncoderState[encoderId] & CWCCWCompareMask)== CCWPattern1  
          || (EncoderState[encoderId] & CWCCWCompareMask) == CCWPattern3 
          || (EncoderState[encoderId] & CWCCWCompareMask) == CCWPattern2  
          || (EncoderState[encoderId] & CWCCWCompareMask) == CCWPattern4)
        {
          //this is a full CCW pattern,
          direction = -1;
        //  EncoderState[encoderId] = 0;
        }
        prevEncoderValue[encoderId ] = encoderValue;

        unsigned long timeDiff = millis() - prevEncoderMillis[encoderId];
        if ( timeDiff < CWCCWDriftTimeLimit && direction != 0 )
        {
          // we get a CW/CCW in less then a very short time, let's check if the current direction is identical with the previous direction

          if ( prevEncoderDirection[encoderId] != direction && prevEncoderDirection[encoderId] != 0 )
          {
            //the current direction is different with the previous one, let's use the previous direction instead
            direction = prevEncoderDirection[encoderId];
          }
        }

        if ( direction == 1 )
        {
          //set bit to 10, so we can lenghten the "pressed" state for some loops 
          if ( EncoderPulseDelayCounter[ encoderId * 2 + 1] == 0 ) 
          { //start/restart the delay counter on the encoder
            EncoderPulseDelayCounter[ encoderId * 2 + 1] = 1;
            EncoderPulseDelayCounter[ encoderId * 2 ] = 0;
          }
          prevEncoderMillis[ encoderId ] = millis();
          prevEncoderDirection[ encoderId ] = direction;
        }
        else if ( direction == -1 )
        {
          //set bit to 01, so we can lenghten the "pressed" state for some loops
          if ( EncoderPulseDelayCounter[ encoderId * 2 ] == 0 ) 
          { //start/restart the delay counter on the encoder
            EncoderPulseDelayCounter[ encoderId * 2 + 1 ] = 0;
            EncoderPulseDelayCounter[ encoderId * 2 ] = 1;
          }
          prevEncoderMillis[ encoderId ] = millis();
          prevEncoderDirection[ encoderId ] = direction;
        } //end of direction judgement
      }
    
      /*  lengthen the press state for some loop
        If we don't lenthen the press state for several loop it seems that Windows ( Not tested on linux so far ) 
        can miss some "pulses" from the rotary encoder, each round of the loop costs 3~5ms, so a "pulse" is only valid for 3~5ms
        maybe this time span is too short for windows to recognize.
        Some talks: 
        http://forums.eagle.ru/showpost.php?p=2197399&postcount=65
        http://forums.eagle.ru/showpost.php?p=2198566&postcount=67

        The code below will try to keep the "pulse" high for some loops. It's based on 64 counters( we have 32 encoders and 
        each encoder has 2 counters), if we found a CW/CCW pulse from encoder then we set the corresponding counter to 1,
        then we keep the button press bit in Joystick report to 1 and add the counter in each loop, 
        until the counter is reach the predefined limit we reset the counter to 0 and reset the button press bit in joystick report to 0.
      */
      if ( EncoderPulseDelayCounter[encoderId * 2 + 1] > 0)
      {
        //CW
        //add the counter
        EncoderPulseDelayCounter[ encoderId * 2 + 1]++;
        if ( EncoderPulseDelayCounter[ encoderId * 2 + 1] >=  StateDelayLength)
        { //we have keep the key-press state long enough so reset the bit to 00
          joyReport.button[ rowid * 2 + colid / 8 ] &= ~(0x1 << ( colid % 8 + 1 ));
          joyReport.button[ rowid * 2 + colid / 8 ] &= ~(0x1 << ( colid % 8 ));
          //reset the counter to 0
          EncoderPulseDelayCounter[ encoderId * 2 + 1] = 0;
          //Serial.println( "counter reset");
        }
        else
        { //the press time is not long enough, keep the bit to 10
          joyReport.button[ rowid * 2 + colid / 8 ] |= 0x1 << ( colid % 8 + 1);
          joyReport.button[ rowid * 2 + colid / 8 ] &= ~(0x1 << ( colid % 8 ));
          //Serial.println( EncoderPulseDelayCounter[ encoderId * 2 + 1] );
        }
      }
      else if ( EncoderPulseDelayCounter[ encoderId * 2 ] > 0 ) 
      {
        //CCW
        //add the counter
        EncoderPulseDelayCounter[ encoderId * 2 ]++;
        if ( EncoderPulseDelayCounter[ encoderId * 2 ] >= StateDelayLength)
        { //we have keep the key-press state long enough so reset the bit to 00
          joyReport.button[ rowid * 2 + colid / 8 ] &= ~(0x1 << ( colid % 8 + 1));
          joyReport.button[ rowid * 2 + colid / 8 ] &= ~(0x1 << ( colid % 8 ));
          //reset the counter to 0
          EncoderPulseDelayCounter[ encoderId * 2 ] = 0;
        }
        else
        { //the press time is not long enough, keep set bit to 01
          joyReport.button[ rowid * 2 + colid / 8 ] &= ~(0x1 << ( colid % 8 + 1));
          joyReport.button[ rowid * 2 + colid / 8 ] |= 0x1 << ( colid % 8 );
        }
      }
    }
  }

  //turn off all rows first
  for ( int rowid = 4; rowid < 16; rowid ++ )
  {
    //turn on the current row
    if (rowid < 8)
    {
      PORTA = ~(0x1 << rowid);
    }
    else
    {
      PORTA = 0xFF;
      PORTC = ~(0x1 << (15 - rowid) );
    }
    //we must have such a delay so the digital pin output can go LOW steadily, 
    //without this delay, the row PIN will not 100% at LOW during yet,
    //so check the first column pin's value will return incorrect result.
    delayMicroseconds(ScanDelay);

    byte colResult[16];
    //pin 38, PD7
    colResult[0] = (PIND & B10000000)== 0 ? 1 : 0;
    //pin 39, PG2
    colResult[1] = (PING & B00000100)== 0 ? 1 : 0;
    //pin 40, PG1
    colResult[2] = (PING & B00000010)== 0 ? 1 : 0;
    //pin 41, PG0
    colResult[3] = (PING & B00000001)== 0 ? 1 : 0;

    //pin 42, PL7
    colResult[4] = (PINL & B10000000)== 0 ? 1 : 0;
    //pin 43, PL6
    colResult[5] = (PINL & B01000000)== 0 ? 1 : 0;
    //pin 44, PL5
    colResult[6] = (PINL & B00100000)== 0 ? 1 : 0;
    //pin 45, PL4
    colResult[7] = (PINL & B00010000)== 0 ? 1 : 0;

    //pin 46, PL3
    colResult[8] = (PINL & B00001000)== 0 ? 1 : 0;
    //pin 47, PL2
    colResult[9] = (PINL & B00000100)== 0 ? 1 : 0;
    //pin 48, PL1
    colResult[10] =(PINL & B00000010) == 0 ? 1 : 0;
    //pin 49, PL0
    colResult[11] =(PINL & B00000001) == 0 ? 1 : 0;

    //pin 50, PB3
    colResult[12] =(PINB & B00001000) == 0 ? 1 : 0;
    //pin 51, PB2
    colResult[13] =(PINB & B00000100) == 0 ? 1 : 0;
    //pin 52, PB1
    colResult[14] =(PINB & B00000010) == 0 ? 1 : 0;
    //pin 53, PB0
    colResult[15] =(PINB & B00000001) == 0 ? 1 : 0;

    for ( int colid = 0; colid < 16; colid ++ )
    {
      if ( colResult[ colid ] == 1 )
      //if ( digitalRead( colid + 38 ) == LOW )
      {
        joyReport.button[ rowid * 2 + colid / 8 ] |= (0x1 << ( colid % 8 ));
      }
      else
      {
        joyReport.button[ rowid * 2 + colid / 8 ] &= ~(0x1 << ( colid % 8 ));
      }
    }
  }

  for (uint8_t ind=0; ind< NUM_AXES; ind++) {
    //joyReport.axis[ind] = map(analogRead(54+ind), 0, 1023, -32768,32767 );
  }
  sendJoyReport(&joyReport);

  // Flash Led 
  if ( (millis() - prevLEDTransition) >=  FLASH_TIME)
    {
      digitalWrite(STATUS_LED_PORT, !digitalRead(STATUS_LED_PORT)); 
      prevLEDTransition = millis();

    }
  
//  delay(20);
}

