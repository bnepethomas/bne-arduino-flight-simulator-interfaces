//  Hornet UFC
//
// The project intends to drive the OLED displays on a F18C Hornet Up Front Controller
//
// The UFC has a large display on the top left hand corner, five mid sized display on the right hand side, and then two smaller 
// displays at the bottom left and bottom right hand side.
//
// As a number of the smae OLEDs are used, which the same target I2C addresses an I2C multiplexor is used, TCA9548A.
// The TCA9548A is an 8 Channel I2C switch.  It is possible for different devices to share a common bus.
// https://e2e.ti.com/blogs_/b/analogwire/archive/2015/10/15/how-to-simplify-i2c-tree-when-connecting-multiple-slaves-to-an-i2c-master
//
// Prototyping is being done with an Adafruit 2717 TCA9548A board.

// The initial test OLEDs have addresses of 0x3C - despite being labelled - 0x78 or 0x7A - selectable by soldering a jumper. I'm assuming these are the same SSD1306 
// used in the Hornet Altimeter.
// Can validate wha addresses are on the bus by using I2C scanner

// The following OLEDs where sourced from ebay
// OLEd for top  2.2" 128 * 32 SSD1305 (not 1306)

// OLED for 5 Right hand side digits 0.91" 128*32 SSD1306
// https://www.ebay.com/itm/0-91-Inch-128x32-IIC-I2c-White-Blue-OLED-LCD-Display-Module-3-3-5v-For-Arduino/392552169768?ssPageName=STRK%3AMEBIDX%3AIT&var=661536491479&_trksid=p2057872.m2749.l2649


//  OLED for Small Digits 0.96" SSD1306
// https://www.ebay.com/itm/0-96-White-Yellow-Blue-128X64-OLED-I2C-IIC-Serial-LCD-LED-SSD-Display-AU/302435912549?ssPageName=STRK%3AMEBIDX%3AIT&var=601268570499&_trksid=p2060353.m1438.l2649
// or may this one 0.66" 64*48 SSD1036
// I2C 0x3C - validate by running scanner



void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
