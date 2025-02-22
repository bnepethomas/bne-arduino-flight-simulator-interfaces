// Time to write characters and place screen in deep sleep is 820mS
// 680mS doing a character write alone

#include <SPI.h>

#include "Display_EPD_W21_spi.h"
#include "Display_EPD_W21.h"
#include "Ap_29demo.h"

#define Ethernet_In_Use 1
#define DCSBIOS_In_Use 1
#define Reflector_In_Use 1

#define DCSBIOS_IRQ_SERIAL
#include "DcsBios.h"

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


unsigned int LastAltitude = 0;
unsigned int LastHundredAltitude = 0;
unsigned int LastThousandAltitude = 0;
unsigned int LastTenThousandAltitude = 0;

void SendDebug(String MessageToSend) {
  if ((Reflector_In_Use == 1) && (Ethernet_In_Use == 1)) {
    udp.beginPacket(reflectorIP, reflectorport);
    udp.println(MessageToSend);
    udp.endPacket();
  }
}


void setup() {


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

  pinMode(4, INPUT);   //BUSY
  pinMode(5, OUTPUT);  //RES
  pinMode(6, OUTPUT);  //DC
  pinMode(7, OUTPUT);  //CS

  //SPI
  SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE0));
  SPI.begin();

  unsigned long timetoprocess = 0;

#if 1                                       //Partial refresh demostration. \
                                            //Partial refresh demo support displaying a clock at 5 locations with 00:00.  If you need to perform partial refresh more than 5 locations, please use the feature of using partial refresh at the full screen demo. \
                                            //After 5 partial refreshes, implement a full screen refresh to clear the ghosting caused by partial refreshes. \
                                            //////////////////////Partial refresh time demo/////////////////////////////////////
  EPD_Init();                               //Electronic paper initialization.
  EPD_SetRAMValue_BaseMap(gImage_basemap);  //Please do not delete the background color function, otherwise it will cause unstable display during partial refresh.
                                            // for (unsigned char i = 0; i <= 3; i++) {
                                            //   // EPD_Dis_Part_Time(15, 120, Num[i], Num[0], 1, 32, 48);  //x,y,DATA-A~E,number,Resolution 32*32
                                            //   timetoprocess = millis();

  //   EPD_Dis_Part_Time(15, 60, agImage_48480, agImage_48480 , 2, 48, 48);
  //   //EPD_Dis_Part_Time(15, 60, gImage_040480, gImage_04048, 2, 40, 48);
  //   EPD_DeepSleep();  //Enter the sleep mode and please do not delete it, otherwise it will reduce the lifespan of the screen.
  //   SendDebug("Time to Write To Screen :" + String((millis() - timetoprocess) ));
  //   //EPD_Dis_Part_Time(15, 60, Num[i], Num[0], 1, 32, 48);  //x,y,DATA-A~E,number,Resolution 32*32
  //   // 40 48
  // }
  timetoprocess = millis();

  EPD_Dis_Part_Time(15, 60, agImage48480, agImage48480, 1, 48, 48);
  //EPD_Dis_Part_Time(15, 60, gImage_040480, gImage_04048, 2, 40, 48);
  EPD_DeepSleep();  //Enter the sleep mode and please do not delete it, otherwise it will reduce the lifespan of the screen.
  SendDebug("Time to Write To Screen :" + String((millis() - timetoprocess)));
  EPD_DeepSleep();  //Enter the sleep mode and please do not delete it, otherwise it will reduce the lifespan of the screen.
  delay(2000);      //Delay for 2s.


#endif


  if (DCSBIOS_In_Use == 1) DcsBios::setup();

  SendDebug("Setup complete");
}



//Tips//
/*
1.Flickering is normal when EPD is performing a full screen update to clear ghosting from the previous image so to ensure better clarity and legibility for the new image.
2.There will be no flicker when EPD performs a partial refresh.
3.Please make sue that EPD enters sleep mode when refresh is completed and always leave the sleep mode command. Otherwise, this may result in a reduced lifespan of EPD.
4.Please refrain from inserting EPD to the FPC socket or unplugging it when the MCU is being powered to prevent potential damage.)
5.Re-initialization is required for every full screen update.
6.When porting the program, set the BUSY pin to input mode and other pins to output mode.
*/

void onAltMslFtChange(unsigned int newValue) {

  // unsigned int LastAltitude = 0;
  // unsigned int LastHundredAltitude = 0;
  // unsigned int LastThousandAltitude = 0;
  // unsigned int LastTenThousandAltitude = 0;

  SendDebug("Altitude :" + String(newValue));


  if (newValue != LastAltitude) {
    int ThousandsAltitude = 0;
    LastAltitude = newValue;
    ThousandsAltitude = int(newValue / 1000);
    if (ThousandsAltitude != LastThousandAltitude) {
      // We have a change in Thousands - send to display
      LastThousandAltitude = ThousandsAltitude;
      SendDebug("ThousandsAltitude: " + String(ThousandsAltitude));

      if (newValue <= 9999) {
        unsigned char zerovalue = 0;
        EPD_Dis_Part_Time(0, 15, Num[zerovalue], '0', 2, 32, 48);
        EPD_DeepSleep();
      } else {
        SendDebug("results of modules :" + String(ThousandsAltitude % 10));
        SendDebug("results of division :" + String(int(ThousandsAltitude / 10)));
        unsigned char wrkThousands = int(ThousandsAltitude % 10);
        unsigned char wrkTenThousands = int(ThousandsAltitude / 10);
        //unsigned char wrkTenThousands = 2;
        EPD_Dis_Part_Time(0, 15, Num[wrkThousands], Num[wrkTenThousands], 2, 32, 48);
        EPD_DeepSleep();
      }
    }
  }
}
DcsBios::IntegerBuffer altMslFtBuffer(0x0434, 0xffff, 0, onAltMslFtChange);

void loop() {

  if (millis() >= NEXT_STATUS_TOGGLE_TIMER) {
    RED_LED_STATE = !RED_LED_STATE;
    digitalWrite(STATUS_LED_PORT, RED_LED_STATE);
    NEXT_STATUS_TOGGLE_TIMER = millis() + FLASH_TIME;


    if (DCSBIOS_In_Use == 1) DcsBios::loop();
  }
}
