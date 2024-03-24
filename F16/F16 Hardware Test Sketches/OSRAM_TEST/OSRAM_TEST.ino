

int selectedmodule;

void write_0() {
  digitalWrite(22, LOW);   //D0
  digitalWrite(23, LOW);   //D1
  digitalWrite(24, LOW);   //D2
  digitalWrite(25, LOW);   //D3
  digitalWrite(26, HIGH);  //D4
  digitalWrite(27, HIGH);  //D5
  digitalWrite(28, LOW);   //D6
}

void write_1() {
  digitalWrite(22, HIGH);   //D0
  digitalWrite(23, LOW);   //D1
  digitalWrite(24, LOW);   //D2
  digitalWrite(25, LOW);   //D3
  digitalWrite(26, HIGH);  //D4
  digitalWrite(27, HIGH);  //D5
  digitalWrite(28, LOW);   //D6
}

void write_2() {
  digitalWrite(22, LOW);   //D0
  digitalWrite(23, HIGH);   //D1
  digitalWrite(24, LOW);   //D2
  digitalWrite(25, LOW);   //D3
  digitalWrite(26, HIGH);  //D4
  digitalWrite(27, HIGH);  //D5
  digitalWrite(28, LOW);   //D6
}

void write_3() {
  digitalWrite(22, HIGH);   //D0
  digitalWrite(23, HIGH);   //D1
  digitalWrite(24, LOW);   //D2
  digitalWrite(25, LOW);   //D3
  digitalWrite(26, HIGH);  //D4
  digitalWrite(27, HIGH);  //D5
  digitalWrite(28, LOW);   //D6
}

void selectdigit(int digittoselect) {
  if (digittoselect == 0) {
    digitalWrite(30, HIGH);  //A0
    digitalWrite(31, HIGH);  //A1
  } else if (digittoselect == 1) {
    digitalWrite(30, LOW);   //A0
    digitalWrite(31, HIGH);  //A1
  } else if (digittoselect == 2) {
    digitalWrite(30, HIGH);  //A0
    digitalWrite(31, LOW);   //A1
  } else if (digittoselect == 3) {
    digitalWrite(30, LOW);  //A0
    digitalWrite(31, LOW);  //A1
  }
}

void selectmodule(int moduletoselect) {
  selectedmodule = moduletoselect;
}

void clockit() {

  if (selectedmodule == 0) {
    digitalWrite(8, HIGH);
    delay(1);
    digitalWrite(8, LOW);
    delay(1);
    digitalWrite(8, HIGH);
  } else if (selectedmodule == 1) {
    digitalWrite(9, HIGH);
    delay(1);
    digitalWrite(9, LOW);
    delay(1);
    digitalWrite(9, HIGH);
  } else if (selectedmodule == 2) {
    digitalWrite(10, HIGH);
    delay(1);
    digitalWrite(10, LOW);
    delay(1);
    digitalWrite(10, HIGH);
  } else if (selectedmodule == 3) {
    digitalWrite(11, HIGH);
    delay(1);
    digitalWrite(11, LOW);
    delay(1);
    digitalWrite(11, HIGH);
  }
}



// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(8, OUTPUT);   //OSRAM 1 Write
  pinMode(9, OUTPUT);   //OSRAM 1 Write
  pinMode(10, OUTPUT);  //OSRAM 1 Write
  pinMode(11, OUTPUT);  //OSRAM 1 Write

  digitalWrite(8, HIGH);
  digitalWrite(9, HIGH);
  digitalWrite(10, HIGH);
  digitalWrite(11, HIGH);

  pinMode(22, OUTPUT);  //D0
  pinMode(23, OUTPUT);  //D1
  pinMode(24, OUTPUT);  //D2
  pinMode(25, OUTPUT);  //D3
  pinMode(26, OUTPUT);  //D4
  pinMode(27, OUTPUT);  //D5
  pinMode(28, OUTPUT);  //D6

  pinMode(30, OUTPUT);  //D5
  pinMode(31, OUTPUT);  //D6

  selectedmodule = 0;

  selectmodule(0);
  selectdigit(0);
  write_0();
  clockit();
  selectdigit(1);
  write_0();
  clockit();
  selectdigit(2);
  write_0();
  clockit();
  selectdigit(3);
  write_0();
  clockit();

  selectedmodule = 1;
  selectdigit(0);
  write_1();
  clockit();
  selectdigit(1);
  write_1();
  clockit();
  selectdigit(2);
  write_1();
  clockit();
  selectdigit(3);
  write_1();
  clockit();

  selectedmodule = 2;
  selectdigit(0);
  write_2();
  clockit();
  selectdigit(1);
  write_2();
  clockit();
  selectdigit(2);
  write_2();
  clockit();
  selectdigit(3);
  write_2();
  clockit();

  selectedmodule = 3;
  selectdigit(0);
  write_3();
  clockit();
  selectdigit(1);
  write_3();
  clockit();
  selectdigit(2);
  write_3();
  clockit();
  selectdigit(3);
  write_3();
  clockit();
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
  delay(1000);                      // wait for a second
  digitalWrite(LED_BUILTIN, LOW);   // turn the LED off by making the voltage LOW
  delay(1000);                      // wait for a second
}
