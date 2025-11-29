

int selectedmodule = 0;

const int PIN_D0 = 22;
const int PIN_D1 = 23;
const int PIN_D2 = 24;
const int PIN_D3 = 25;
const int PIN_D4 = 26;
const int PIN_D5 = 27;
const int PIN_D6 = 28;

const int PIN_ADDR0 = 30;
const int PIN_ADDR1 = 31;

const int PIN_MODULE_SEL_0 = 8;
const int PIN_MODULE_SEL_1 = 9;
const int PIN_MODULE_SEL_2 = 10;
const int PIN_MODULE_SEL_3 = 11;

void writethebyte(uint8_t theByte) {
  digitalWrite(PIN_D0, theByte & B00000001);
  digitalWrite(PIN_D1, theByte & B00000010);
  digitalWrite(PIN_D2, theByte & B00000100);
  digitalWrite(PIN_D3, theByte & B00001000);
  digitalWrite(PIN_D4, theByte & B00010000);
  digitalWrite(PIN_D5, theByte & B00100000);
  digitalWrite(PIN_D6, theByte & B01000000);
}

void writechar(int moduletoselect, int digittoselect, uint8_t theByte) {

  selectmodule(moduletoselect);
  selectdigit(digittoselect);

  // Quick bounds check if invalid print X
  // Need to add spaces, - etc
  if ((theByte >= 0x5D) || (theByte <= 0x2F)) {
    theByte = 0x58;
  }
  writethebyte(theByte);
  clockit();
}


void selectdigit(int digittoselect) {
  if (digittoselect == 0) {
    digitalWrite(PIN_ADDR0, HIGH);  //A0
    digitalWrite(PIN_ADDR1, HIGH);  //A1
  } else if (digittoselect == 1) {
    digitalWrite(PIN_ADDR0, LOW);   //A0
    digitalWrite(PIN_ADDR1, HIGH);  //A1
  } else if (digittoselect == 2) {
    digitalWrite(PIN_ADDR0, HIGH);  //A0
    digitalWrite(PIN_ADDR1, LOW);   //A1
  } else if (digittoselect == 3) {
    digitalWrite(PIN_ADDR0, LOW);  //A0
    digitalWrite(PIN_ADDR1, LOW);  //A1
  }
}

void selectmodule(int moduletoselect) {
  selectedmodule = moduletoselect;
}

void clockit() {
  int pintowrite = 8;
  if (selectedmodule == 0) {
    pintowrite = PIN_MODULE_SEL_0;
  } else if (selectedmodule == 1) {
    pintowrite = PIN_MODULE_SEL_1;
  } else if (selectedmodule == 2) {
    pintowrite = PIN_MODULE_SEL_2;
  } else if (selectedmodule == 3) {
    pintowrite = PIN_MODULE_SEL_3;
  }
  digitalWrite(pintowrite, HIGH);
  delay(1);
  digitalWrite(pintowrite, LOW);
  delay(1);
  digitalWrite(pintowrite, HIGH);
}



// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(PIN_MODULE_SEL_0, OUTPUT);  //OSRAM 0 Write
  pinMode(PIN_MODULE_SEL_1, OUTPUT);  //OSRAM 1 Write
  pinMode(PIN_MODULE_SEL_2, OUTPUT);  //OSRAM 2 Write
  pinMode(PIN_MODULE_SEL_3, OUTPUT);  //OSRAM 3 Write

  digitalWrite(PIN_MODULE_SEL_0, HIGH);
  digitalWrite(PIN_MODULE_SEL_1, HIGH);
  digitalWrite(PIN_MODULE_SEL_2, HIGH);
  digitalWrite(PIN_MODULE_SEL_3, HIGH);

  pinMode(PIN_D0, OUTPUT);  //D0
  pinMode(PIN_D1, OUTPUT);  //D1
  pinMode(PIN_D2, OUTPUT);  //D2
  pinMode(PIN_D3, OUTPUT);  //D3
  pinMode(PIN_D4, OUTPUT);  //D4
  pinMode(PIN_D5, OUTPUT);  //D5
  pinMode(PIN_D6, OUTPUT);  //D6

  pinMode(PIN_ADDR0, OUTPUT);  //A0
  pinMode(PIN_ADDR1, OUTPUT);  //A1




  writechar(0, 0, 0x2F);
  writechar(0, 1, 0x5D);
  writechar(0, 2, 0x30);
  writechar(0, 3, 0x30);

  writechar(1, 0, 0x31);
  writechar(1, 1, 0x31);
  writechar(1, 2, 0x31);
  writechar(1, 3, 0x31);

  writechar(2, 0, 0x32);
  writechar(2, 1, 0x32);
  writechar(2, 2, 0x32);
  writechar(2, 3, 0x32);

  writechar(3, 0, 0x41);
  writechar(3, 1, 0x42);
  writechar(3, 2, 0x43);
  writechar(3, 3, 0x44);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
  delay(1000);                      // wait for a second
  digitalWrite(LED_BUILTIN, LOW);   // turn the LED off by making the voltage LOW
  delay(1000);                      // wait for a second
}
