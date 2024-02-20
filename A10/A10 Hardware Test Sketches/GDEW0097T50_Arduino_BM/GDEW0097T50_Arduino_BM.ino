#include <SPI.h>
//EPD
#include "Display_EPD_W21_spi.h"
#include "Display_EPD_W21.h"
#include "Ap_29demo.h"

#include "ozhornet_epaper_font_data.h"
#include "ozhornet_epaper_hash_data.h"


#define Ethernet_In_Use 1
#define DCSBIOS_In_Use 0
#define Reflector_In_Use 1

#include <Ethernet.h>
#include <EthernetUdp.h>


// These local Mac and IP Address will be reassigned early in startup based on
// the device ID as set by address pins
byte mac[] = { 0x00, 0xDD, 0x3E, 0xCA, 0x36, 0x99 };
IPAddress ip(172, 16, 1, 100);
String strMyIP = "X.X.X.X";

// Raspberry Pi is Target
IPAddress targetIP(172, 16, 1, 2);
String strTargetIP = "X.X.X.X";

// Reflector
IPAddress reflectorIP(172, 16, 1, 10);
String strReflectorIP = "X.X.X.X";

const unsigned int localport = 7788;
const unsigned int remoteport = 26027;
const unsigned int reflectorport = 27000;

EthernetUDP udp;
char packetBuffer[1000];     //buffer to store the incoming data
char outpacketBuffer[1000];  //buffer to store the outgoing data


#define STATUS_LED_PORT LED_BUILTIN
#define FLASH_TIME 500

unsigned long NEXT_STATUS_TOGGLE_TIMER = 0;
bool RED_LED_STATE = false;
unsigned long timeSinceRedLedChanged = 0;

void SendDebug(String MessageToSend) {
  if ((Reflector_In_Use == 1) && (Ethernet_In_Use == 1)) {
    udp.beginPacket(reflectorIP, reflectorport);
    udp.println(MessageToSend);
    udp.endPacket();
  }
}

void setup() {

  pinMode(4, INPUT);   //BUSY
  pinMode(5, OUTPUT);  //RES
  pinMode(6, OUTPUT);  //DC
  pinMode(7, OUTPUT);  //CS

  //SPI
  SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE0));
  SPI.begin();


  pinMode(STATUS_LED_PORT, OUTPUT);

  digitalWrite(STATUS_LED_PORT, true);
  delay(FLASH_TIME);
  digitalWrite(STATUS_LED_PORT, false);

  delay(FLASH_TIME);

  if (Ethernet_In_Use == 1) {
    Ethernet.begin(mac, ip);
    udp.begin(localport);
    SendDebug("Init UDP - " + strMyIP + " " + String(millis()) + "mS since reset.");
  }


  EPD_Init();  //Full screen refresh initialization.
               //EPD_WhiteScreen_White(); //Clear screen function.
  // EPD_WhiteScreen_Black();  //Clear screen function.
  EPD_WhiteScreen_White();  //Clear screen function.
  EPD_Dis_Part_Time(15, 4, black_num1, black_num1 , 2, 48, 48);
}




int lastThousandsValue = 0;
int lastTenThousandsValue = 0;
int lastCharacterOffset = 0;

void UpdateAltimeterDigits(long height) {

  // SendDebug("Altimeter : " + String(height));

  String strnewValue = String(height);
  int itensAboveDigit = 0;
  int itensBelowDigit = 0;


  int iThousandsAboveDigit = 0;
  int iThousandsBelowDigit = 0;
  int iThousandsValue = ((height % 10000) / 1000);
  String sThousandValue = String(iThousandsValue);
  // SendDebug(String(i) + " : " + sThousandValue);
  if (iThousandsValue == 9) {
    iThousandsAboveDigit = 0;
  } else {
    iThousandsAboveDigit = iThousandsValue + 1;
  }
  if (iThousandsValue == 0) {
    iThousandsBelowDigit = 9;
  } else {
    iThousandsBelowDigit = iThousandsValue - 1;
  }

  String sThousandsAboveDigit = String(iThousandsAboveDigit);
  String sThousandsBelowDigit = String(iThousandsBelowDigit);
  const char* cThousandsValue = sThousandValue.c_str();
  const char* cThousandsaboveValue = sThousandsAboveDigit.c_str();
  const char* cThousandsbelowValue = sThousandsBelowDigit.c_str();


  int iTenThousandsValue = (height / 10000);
  String sTenThousandsDigit = String(iTenThousandsValue);
  //SendDebug("TenThousandsDigit : " + sTenThousandsDigit);
  const char* cTenThousandsValue = sTenThousandsDigit.c_str();

  unsigned long TimeToProcess = millis();


  int iCharacterOffset = ((height % 1000) / 130);
  // SendDebug("Character Offset : " + String(iCharacterOffset));


  // Only attempt to draw of something has changed that will impact display
  long iThousandsCharacterOffset = iThousandsValue * 8;
  long iTenThousandsCharacterOffset = iTenThousandsValue * 8;


  if ((iThousandsValue != lastThousandsValue) || (iTenThousandsValue != lastTenThousandsValue) || (iCharacterOffset != lastCharacterOffset)) {


    lastThousandsValue = iThousandsValue;
    lastTenThousandsValue = iTenThousandsValue;
    lastCharacterOffset = iCharacterOffset;
    SendDebug("Altimeter : " + String(height));
    SendDebug(" Processing " + String(iTenThousandsCharacterOffset) + " - " + String(iThousandsCharacterOffset) + " - " + String(iCharacterOffset));

    // Due to performance and memory constraints there are
    if (sTenThousandsDigit != "0") {
      EPD_Dis_Part_Time(15, 4, petetest[iThousandsCharacterOffset + iCharacterOffset], petetest[iTenThousandsCharacterOffset], 2, 48, 48);  //x,y,DATA-A~E,number,Resolution 32*32
    } else {
      EPD_Dis_Part_Time(14, 4, petetest[iThousandsCharacterOffset + iCharacterOffset], hashtest[iTenThousandsCharacterOffset], 2, 48, 48);  //x,y,DATA-A~E,number,Resolution 32*32
    }
    EPD_DeepSleep();  //Enter the sleep mode and please do not delete it, otherwise it will reduce the lifespan of the screen.
    delay(10);
  }
}
/*
void SavedUpdateAltimeterDigits(long height) {
  // Works with high between 0 and 80
  // Due to performance and memory constraints there are
  if (height <= 7) {
    EPD_Dis_Part_Time(15, 4, petetest[height], hashtest[height], petetest[height], petetest[height], petetest[height], 2, 48, 48);  //x,y,DATA-A~E,number,Resolution 32*32
  } else {
    EPD_Dis_Part_Time(15, 4, petetest[height], petetest[height], petetest[height], petetest[height], petetest[height], 2, 48, 48);  //x,y,DATA-A~E,number,Resolution 32*32
  }
  EPD_DeepSleep();  //Enter the sleep mode and please do not delete it, otherwise it will reduce the lifespan of the screen.
  delay(1);
}*/
//Tips//
/*
1.Flickering is normal when EPD is performing a full screen update to clear ghosting from the previous image so to ensure better clarity and legibility for the new image.
2.There will be no flicker when EPD performs a partial refresh.
3.Please make sue that EPD enters sleep mode when refresh is completed and always leave the sleep mode command. Otherwise, this may result in a reduced lifespan of EPD.
4.Please refrain from inserting EPD to the FPC socket or unplugging it when the MCU is being powered to prevent potential damage.)
5.Re-initialization is required for every full screen update.
6.When porting the program, set the BUSY pin to input mode and other pins to output mode.
*/


void loop() {

  for (long i = 64000; i >= 0; i--) {
    UpdateAltimeterDigits(i);
  }

  for (long i = 0; i <= 64000; i++) {
    UpdateAltimeterDigits(i);
  }

  // UpdateAltimeterDigits(9998);
  // delay(2000);
  // UpdateAltimeterDigits(9999);
  // delay(2000);
  // UpdateAltimeterDigits(10000);
  // delay(2000);
  // UpdateAltimeterDigits(10001);
  // delay(2000);

  // for (long i = 79; i >= 0; i--) {
  //   SavedUpdateAltimeterDigits(i);
  // }
  // for (long i = 0; i <= 78; i++) {
  //   SavedUpdateAltimeterDigits(i);
  // }
}




//////////////////////////////////END//////////////////////////////////////////////////
