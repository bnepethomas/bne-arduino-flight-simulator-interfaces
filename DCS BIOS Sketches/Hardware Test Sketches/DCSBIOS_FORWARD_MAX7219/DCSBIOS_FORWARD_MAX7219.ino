//We always have to include the library
#include "LedControl.h"

/*
  Now we need a LedControl to work with.
 ***** These pin numbers will probably not work with your hardware *****
  pin 12 is connected to the DataIn
  pin 11 is connected to the CLK
  pin 10 is connected to LOAD
  We have only a single MAX72XX.
*/
int NoOfUnits = 9;
LedControl lc = LedControl(16, 14, 15, NoOfUnits);

int UnitUnderTest = 0;

/* we always wait a bit between updates of the display */
unsigned long delaytime = 20;

void setup() {
  pinMode(5, OUTPUT);
  /*
    The MAX72XX is in power-saving mode on startup,
    we have to do a wakeup call
  */

  for (int displayunit = 0; displayunit < NoOfUnits - 1; displayunit++) {
    lc.shutdown(displayunit, false);
    /* Set the brightness to a medium values */
    lc.setIntensity(displayunit, 8);
    /* and clear the display */
    lc.clearDisplay(displayunit);
  }
  Serial.begin(250000);

}

/*
  This method will display the characters for the
  word "Arduino" one after the other on the matrix.
  (you need at least 5x7 leds to see the whole chars)
*/
void writeArduinoOnMatrix() {
  /* here is the data for the characters */
  byte a[5] = {B01111110, B10001000, B10001000, B10001000, B01111110};
  byte r[5] = {B00111110, B00010000, B00100000, B00100000, B00010000};
  byte d[5] = {B00011100, B00100010, B00100010, B00010010, B11111110};
  byte u[5] = {B00111100, B00000010, B00000010, B00000100, B00111110};
  byte i[5] = {B00000000, B00100010, B10111110, B00000010, B00000000};
  byte n[5] = {B00111110, B00010000, B00100000, B00100000, B00011110};
  byte o[5] = {B00011100, B00100010, B00100010, B00100010, B00011100};

  /* now display them one by one with a small delay */
  lc.setRow(UnitUnderTest, 0, a[0]);
  lc.setRow(UnitUnderTest, 1, a[1]);
  lc.setRow(UnitUnderTest, 2, a[2]);
  lc.setRow(UnitUnderTest, 3, a[3]);
  lc.setRow(UnitUnderTest, 4, a[4]);
  delay(delaytime);
  lc.setRow(UnitUnderTest, 0, r[0]);
  lc.setRow(UnitUnderTest, 1, r[1]);
  lc.setRow(UnitUnderTest, 2, r[2]);
  lc.setRow(UnitUnderTest, 3, r[3]);
  lc.setRow(UnitUnderTest, 4, r[4]);
  delay(delaytime);
  lc.setRow(UnitUnderTest, 0, d[0]);
  lc.setRow(UnitUnderTest, 1, d[1]);
  lc.setRow(UnitUnderTest, 2, d[2]);
  lc.setRow(UnitUnderTest, 3, d[3]);
  lc.setRow(UnitUnderTest, 4, d[4]);
  delay(delaytime);
  lc.setRow(UnitUnderTest, 0, u[0]);
  lc.setRow(UnitUnderTest, 1, u[1]);
  lc.setRow(UnitUnderTest, 2, u[2]);
  lc.setRow(UnitUnderTest, 3, u[3]);
  lc.setRow(UnitUnderTest, 4, u[4]);
  delay(delaytime);
  lc.setRow(UnitUnderTest, 0, i[0]);
  lc.setRow(UnitUnderTest, 1, i[1]);
  lc.setRow(UnitUnderTest, 2, i[2]);
  lc.setRow(UnitUnderTest, 3, i[3]);
  lc.setRow(UnitUnderTest, 4, i[4]);
  delay(delaytime);
  lc.setRow(UnitUnderTest, 0, n[0]);
  lc.setRow(UnitUnderTest, 1, n[1]);
  lc.setRow(UnitUnderTest, 2, n[2]);
  lc.setRow(UnitUnderTest, 3, n[3]);
  lc.setRow(UnitUnderTest, 4, n[4]);
  delay(delaytime);
  lc.setRow(UnitUnderTest, 0, o[0]);
  lc.setRow(UnitUnderTest, 1, o[1]);
  lc.setRow(UnitUnderTest, 2, o[2]);
  lc.setRow(UnitUnderTest, 3, o[3]);
  lc.setRow(UnitUnderTest, 4, o[4]);
  delay(delaytime);
  lc.setRow(UnitUnderTest, 0, 0);
  lc.setRow(UnitUnderTest, 1, 0);
  lc.setRow(UnitUnderTest, 2, 0);
  lc.setRow(UnitUnderTest, 3, 0);
  lc.setRow(UnitUnderTest, 4, 0);
  delay(delaytime);
}

/*
  This function lights up a some Leds in a row.
  The pattern will be repeated on every row.
  The pattern will blink along with the row-number.
  row number 4 (index==3) will blink 4 times etc.
*/
void rows() {
  for (int row = 0; row < 8; row++) {
    delay(delaytime);
    lc.setRow(UnitUnderTest, row, B10100000);
    delay(delaytime);
    lc.setRow(UnitUnderTest, row, (byte)0);
    for (int i = 0; i < row; i++) {
      delay(delaytime);
      lc.setRow(UnitUnderTest, row, B10100000);
      delay(delaytime);
      lc.setRow(UnitUnderTest, row, (byte)0);
    }
  }
}

/*
  This function lights up a some Leds in a column.
  The pattern will be repeated on every column.
  The pattern will blink along with the column-number.
  column number 4 (index==3) will blink 4 times etc.
*/
void columns() {
  for (int col = 0; col < 8; col++) {
    delay(delaytime);
    lc.setColumn(UnitUnderTest, col, B10100000);
    delay(delaytime);
    lc.setColumn(UnitUnderTest, col, (byte)0);
    for (int i = 0; i < col; i++) {
      delay(delaytime);
      lc.setColumn(UnitUnderTest, col, B10100000);
      delay(delaytime);
      lc.setColumn(UnitUnderTest, col, (byte)0);
    }
  }
}

/*
  This function will light up every Led on the matrix.
  The led will blink along with the row-number.
  row number 4 (index==3) will blink 4 times etc.
*/
void single() {
  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      if (col != 9) {
        delay(delaytime);
        lc.setLed(UnitUnderTest, row, col, true);
        delay(delaytime);
        for (int i = 0; i < col; i++) {
          lc.setLed(UnitUnderTest, row, col, false);
          delay(delaytime);
          lc.setLed(UnitUnderTest, row, col, true);
          delay(delaytime);
        }
      }
    }
  }

  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      lc.setLed(UnitUnderTest, row, col, false);
    }
  }
}

void AllOn() {
  for (int displayunit = 0; displayunit < 9; displayunit++) {
    for (int row = 0; row < 8; row++) {
      for (int col = 0; col < 8 ; col++) {
        if (col != 9 && col != 9 && col != 9)
          lc.setLed(displayunit, row, col, true);
      }
    }
  }
}

void AllOff() {
  for (int displayunit = 0; displayunit < 9; displayunit++) {
    for (int row = 0; row < 8; row++) {
      for (int col = 0; col < 8; col++) {
        lc.setLed(displayunit, row, col, false);
      }
    }
  }
}

void ChipOn(int selectedChip) {

  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      lc.setLed(selectedChip, row, col, true);
    }
  }
}

void ChipOff(int selectedChip) {

  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      lc.setLed(selectedChip, row, col, false);
    }
  }
}



void WalkA7129() {

  int UnitUnderTest = 4;

  delaytime = 3000;
  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      if (col != 9) {
        delay(delaytime);
        lc.setLed(UnitUnderTest, row, col, true);

        Serial.println("Testing Unit:" + String(UnitUnderTest)  + " Column:" + String(col)+ " Row: " + String(row));
        delay(delaytime);

        for (int i = 0; i < col; i++) {
          lc.setLed(UnitUnderTest, row, col, false);
          delay(delaytime);
          lc.setLed(UnitUnderTest, row, col, true);
          delay(delaytime);
        }
      }
    }
  }

  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      lc.setLed(UnitUnderTest, row, col, false);
    }
  }

}



void reset7219() {
  lc.shutdown(UnitUnderTest, false);
  /* Set the brightness to a medium values */
  lc.setIntensity(UnitUnderTest, 8);
  /* and clear the display */
  lc.clearDisplay(UnitUnderTest);


}

void flashstatusled() {
  digitalWrite(5, HIGH);
  delay(500);
  digitalWrite(5, LOW);
  delay(500);
}

void FindUnit() {
  int NumberofUnitsToscan = 6;
  String inputstr = "";
  Serial.println("Entering FindUnit");
  for (int i = 0; i <= NumberofUnitsToscan; i++) {
    ChipOn(i);
    Serial.println("Chip " + String(i));
    Serial.println("Press Send to continue");
    while (Serial.available() == 0) {
      // Wait for User to Input Data
    }
    inputstr = Serial.read(); //Read the data the user has input
  }
  for (int i = 0; i <= NumberofUnitsToscan; i++) {
    ChipOff(i);
  }

}

void FindLed(int unitToScan) {

  String inputstr = "";

  ChipOn(unitToScan);
  ChipOff(unitToScan);
  Serial.println("Entering FindUnit");

  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      lc.setLed(unitToScan, col, row, true);

      Serial.println("Testing Chip:" + String(unitToScan) + " Row: " + String(row) + " Column:" + String(col));
      Serial.println("click enter to continue");
      while (Serial.available() == 0) {
        // Wait for User to Input Data
      }
      inputstr = Serial.read(); //Read the data the user has input
      Serial.read();

    }
  }
}

#define LOCK_SHOOT 3

#define LOCK_COL_A 5
#define LOCK_ROW_A 2
#define LOCK_COL_B 5
#define LOCK_ROW_B 4

#define AOA_DIM 4

#define SHOOT_COL_A 6
#define SHOOT_ROW_A 3
#define SHOOT_COL_B 6
#define SHOOT_ROW_B 4

#define SHOOT_FLASH_COL_A 5
#define SHOOT_FLASH_ROW_A 3
#define SHOOT_FLASH_COL_B 6
#define SHOOT_FLASH_ROW_B 2

#define AMBER_FLAPS_COL_A 3
#define AMBER_FLAPS_ROW_A 2
#define AMBER_FLAPS_COL_B 4
#define AMBER_FLAPS_ROW_B 2

void SHOOT(unsigned int newValue) {
  lc.setLed(LOCK_SHOOT, SHOOT_COL_A, SHOOT_ROW_A, newValue);
  lc.setLed(LOCK_SHOOT, SHOOT_COL_B, SHOOT_ROW_B, newValue);
}


void LOCK(unsigned int newValue) {
  lc.setLed(LOCK_SHOOT, LOCK_COL_A, LOCK_ROW_A, newValue);
  lc.setLed(LOCK_SHOOT, LOCK_COL_B, LOCK_ROW_B, newValue);
}

void SHOOT_FLASH(unsigned int newValue) {
  lc.setLed(LOCK_SHOOT, SHOOT_FLASH_COL_A, SHOOT_FLASH_ROW_A, newValue);
  lc.setLed(LOCK_SHOOT, SHOOT_FLASH_COL_B, SHOOT_FLASH_ROW_B, newValue);
}

// ########################## AOA ##################
#define AOA_ABOVE_COL 0
#define AOA_ABOVE_ROW 1

#define AOA_ON_COL 1
#define AOA_ON_ROW 1

#define AOA_BELOW_COL 1
#define AOA_BELOW_ROW 0


void AOA_ABOVE(unsigned int newValue) {
  lc.setLed(AOA_DIM, AOA_ABOVE_COL, AOA_ABOVE_ROW, newValue);
}

void AOA_ON(unsigned int newValue) {
  lc.setLed(AOA_DIM, AOA_ON_COL, AOA_ON_ROW, newValue);
}
void AOA_BELOW(unsigned int newValue) {
  lc.setLed(AOA_DIM, AOA_BELOW_COL, AOA_BELOW_ROW, newValue);
}
#define SELECT_JET_PANEL 2
#define AMBER_FLAPS_COL_A 3
#define AMBER_FLAPS_ROW_A 2
#define AMBER_FLAPS_COL_B 4
#define AMBER_FLAPS_ROW_B 2

#define HALF_FLAPS_COL_A 0
#define HALF_FLAPS_ROW_A 2
#define HALF_FLAPS_COL_B 2
#define HALF_FLAPS_ROW_B 2


void FLAPS(unsigned int newValue) {
  lc.setLed(SELECT_JET_PANEL, AMBER_FLAPS_COL_A, AMBER_FLAPS_ROW_A, newValue);
  lc.setLed(SELECT_JET_PANEL, AMBER_FLAPS_COL_B, AMBER_FLAPS_ROW_B, newValue);
}
void loop() {

  //FLAPS(0);
  delay(1000);
  WalkA7129();
  flashstatusled();
  AllOn();
  //FLAPS(1);
  delay(1000);
  //ChipOff(3);
  flashstatusled();
  AllOff();


}
