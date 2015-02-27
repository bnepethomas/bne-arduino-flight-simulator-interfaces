/*

Change log
20141214a - Right Fuel Min Max ok. Removed initialisation to zero points from setup.
20141214b - All Fuel and Hyd gauges have correct startup sequence
20141214d - Left and Right Fuel receivng data, now working on smoothing
20141214e - Receiving : spearated values for all guages, now to smooth timing
20141215a - Cleaning up used vairable names


Working sanely with individual pointers moving on dual fuel guage
added init zero position
still unsure if issues are electrocial or physical associated with hitting binding point

    
command bytes description: 
one 595 connected to one VID6606, 2 bytes will be send to each 595 to control 4 stepper motor.
VID6606 requires a raise edge to move the stepper, that's the reason that we need  2 bytes for 595
please refer to the KiCAD PCB schema.


the bit 7,5,3,1 controls the 4 stepper motor's direction, 0 is CW, 1 is CCW,
bit 6,4,2,0 controls the 4 stepper's movement, 0 is no move, 1 is one step.


for example:
command byte 0: 
01xxxxxx  // StepperA CCW
command byte 1:
11xxxxxx  // StepperA step 1

xx00xxxx // StepperB CW
xx10xxxx // StepperB step 1

xxxx00xx  // StepperC CW but has no effect
xxxx00xx  // StepperC no move

APU EGT - 595_1 -- 76
APU RPM - 595_1 -- 54
Engine Oil Pressure Right - 595_1 -- 32
Engine Oil Pressure Left  - 595_1 -- 10

Engine Fule Flow Right - 595_2 -- 76
Engine Fule Flow Left - 595_2 -- 54
Engine Core Speed Right - 595_2 -- 32
Engine Core Speed Left - 595_2 -- 10

Engine Fan Speed Right - 595_3 -- 76
Engine Fan Speed Left - 595_3 -- 54
Engine Interstage Temperature Right - 595_3 -- 32
Engine Interstage Temperature Left - 595_3 -- 10

*/

#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

// 21041115 Adding block to save zero position for needles
#include <EEPROM.h>

byte mac[] = { 
	0xA9,0xE7,0x3E,0xCA,0x34,0x1E};
IPAddress ip(192,168,1,105);
const unsigned int localport = 13135;

EthernetUDP udp;
char packetBuffer[200]; //buffer to store the incoming data

const int MaxSteps = 3780;
const int DelayBetweenSteps = 250;

const int StartUpDelayBetweenSteps = 200;            // Time between different cycles on startup
const int MaxFuelPosition = 157;                     // Maximum number of degrees movement for both fuel gauges
const int StepsPerDegree = 12;                       // Number of steps per degree, normally 12 for Vid29 series of steppers

int testposition = 0;


void setup()
{

	// 2014115
        int i = EEPROM.read(0);
        Ethernet.begin( mac, ip);
	udp.begin( localport );

	Serial.begin(115200);    

	for( int i = 22; i <= 42; i++)
	{
		pinMode( i, OUTPUT );
	}

        //HardZeroPort1(); // Left Fuel
        //HardZeroPort3(); //Right Fuel
 	for( int i = 1; i <= 1; i++)
	{
//		GotoZero();
//                delay(200);
//                GotoMax();
//                delay(200);
//  

                // Port 1 is Lower Needle for Left Fuel
                Serial.println("Pausing before cycling 1");
                delay(StartUpDelayBetweenSteps);
                Serial.println("Start 1 Max");
                GotoMaxPort1();
                Serial.println("Finished 1 Max");
                Serial.println("Pausing");
                delay(StartUpDelayBetweenSteps);
                Serial.println("Start 1 Min");
                GotoMinPort1();
                Serial.println("Finished 1 Min");
                Serial.println("Finished 1 cycle");
                //GotoMaxPort1();
                //delay(200);
  		//GotoMinPort1();
                //delay(200);

                
                // Port 3 is Upper Needle for Right Fuel
                // Upper needle selected for right as it avoids contact with bolt heads
                //Serial.println("Hard Zero 3");
                //HardZeroPort3();
                Serial.println("Pausing before cycling 3");
                delay(StartUpDelayBetweenSteps);
                Serial.println("Start 3 Max");
                GotoMaxPort3();
                Serial.println("Finished 3 Max");
                Serial.println("Pausing");
                delay(StartUpDelayBetweenSteps);
                Serial.println("Start 3 Min");
                GotoMinPort3();
                Serial.println("Finished 3 Min");
                Serial.println("Finished 3 cycle");
                
                
                Serial.println("Pausing before cycling 0");  
                delay(StartUpDelayBetweenSteps);
                Serial.println("Start 0 Max");
                GotoMaxPort0();
                Serial.println("Finished 0 Max");
                delay(StartUpDelayBetweenSteps);
                Serial.println("Start 0 Min");
                GotoMinPort0();
                Serial.println("Finished 0 Min");

                Serial.println("Pausing before cycling 2");  
                delay(StartUpDelayBetweenSteps);
                Serial.println("Start 2 Max");
                GotoMaxPort2();
                Serial.println("Finished 2 Max");
                delay(StartUpDelayBetweenSteps);
                Serial.println("Start 2 Min");
                GotoMinPort2();
                Serial.println("Finished 2 Min");
                
	}       

        

}


//pin 22 23 24 - PA0 PA1 PA2
void sendCmdTo595_1( const byte cmd )
{
	byte bitMask = B00000001;
	for( int i = 0; i < 8; i++)
	{
		/*
		WARNING!!!  don't use digitalWrite here because it's speed is too slow, 
		use avr c style to control the IO pin HIGH / LOW
		http://arduino.cc/en/Hacking/PinMapping2560
		http://www.instructables.com/id/Arduino-is-Slow-and-how-to-fix-it/?ALLSTEPS
		*/
		PORTA = (bitMask & cmd) > 0 ? (PORTA | B00000001) : (PORTA & B11111110);
		//give a raise pulse to 595 input clock pin
		PORTA &= B11111101;
		PORTA |= B00000010;

		bitMask = bitMask << 1;
	}
	//send data from shift register to storage register
	PORTA &= B11111011;
	PORTA |= B00000100;
}

//pin 25 26 27 - PA3 PA4 PA5
void sendCmdTo595_2(const byte cmd )
{
	byte bitMask = B00000001;
	for( int i = 0; i < 8; i++)
	{
		PORTA = (bitMask & cmd) > 0 ? (PORTA | B00001000) : (PORTA & B11110111);

		PORTA &= B11101111;
		PORTA |= B00010000;

		bitMask = bitMask << 1;
	}

	PORTA &= B11011111;
	PORTA |= B00100000;
}

//pin 28 29 30 - PA6 PA7 PC7
void sendCmdTo595_3(const byte cmd )
{
	byte bitMask = B00000001;
	for( int i = 0; i < 8; i++)
	{
		PORTA = (bitMask & cmd) > 0 ? (PORTA | B01000000) : (PORTA & B10111111);

		PORTA &= B01111111;
		PORTA |= B10000000;

		bitMask = bitMask << 1;
	}

	PORTC &= B01111111;
	PORTC |= B10000000;
}



double EMINumbers[12];

/*
Hydraulic Left
Hydraulic Right  
Fuel Left
Fuel Right

AOA
G
flap
Variometer  

Altimeter Pointer 
Airspeed Pointer
Airspeed Dial
Maximum Airspeed Pointer

*/

double Left_Hyd_Position = 0, Left_Fuel_Position = 0, Right_Hyd_Position = 0, Right_Fuel_Position = 0,
	EMI_FFlowRight_Position = 0, EMI_FFlowLeft_Position = 0, EMI_CoreSpdRightPosition = 0, EMI_CoreSpdLeftPosition = 0,
	EMI_FanSpdRight_Position = 0, EMI_FanSpdLeft_Position = 0, EMI_ITTRight_Position = 0, EMI_ITTLeft_Position = 0;

double target_Left_Hyd_Position, target_Left_Fuel_Position, target_Right_Hyd_Position, target_Right_Fuel_Position,
	target_FFlowRight_Position, target_FFlowLeft_Position, target_CoreSpdRight_Position, target_CoreSpdLeft_Position,
	target_FanSpdRight_Position, target_FanSpdLeft_Position, target_ITTRight_Position, target_ITTLeft_Position;

void GotoZero()
{
	for( int i = 0; i < 166 * 12; i++)
	{
		sendCmdTo595_1( B00000000 ); //Working returning to zero
		sendCmdTo595_1( B01010101 );

//		sendCmdTo595_1( B00100000 ); //Working returning Fuel to desired zero
//		sendCmdTo595_1( B01110101 );

		delayMicroseconds(280);
	}

       
}


void HardZeroPort1()
// Counter Clockwise
{
	for( int i = 0; i < 180 * 12; i++)
	{
		sendCmdTo595_1( B00000000 ); //Working returning to zero
		sendCmdTo595_1( B00010000 );
		delayMicroseconds(200);
	}

       
}


void GotoMinPort0()
// Port 0 L Hydraulic
// Turning Counter Clockwise
{
	for( int i = 0; i < 320 * StepsPerDegree; i++)
	{
		sendCmdTo595_1( B00000000 ); //Working returning to zero
		sendCmdTo595_1( B01000000 );
		delayMicroseconds(250);
	}
       
}

void GotoMaxPort0()
// Port 0 L Hydraulic
// Turning Clockwise
{
	for( int i = 0; i < 320 * StepsPerDegree; i++)
	{		

                sendCmdTo595_1( B10101010 );
 		sendCmdTo595_1( B11000000 );
        	delayMicroseconds(250);
	}

}


void GotoMinPort2()
// Port 2 R Hydraulic
// Turning Counter Clockwise
{
	for( int i = 0; i < 320 * StepsPerDegree; i++)
	{
		sendCmdTo595_1( B00000000 ); //Working returning to zero
		sendCmdTo595_1( B00000100 );
		delayMicroseconds(250);
	}
       
}

void GotoMaxPort2()
// Port 2 R Hydraulic
// Turning Clockwise
{
	for( int i = 0; i < 320 * StepsPerDegree; i++)
	{		

                sendCmdTo595_1( B10101010 );
 		sendCmdTo595_1( B00001100 );
        	delayMicroseconds(250);
	}

}

void GotoMinPort1()
// Port 1 is Lower Needle for Left Fuel Quantity
// Turning Counter Clockwise
{
	for( int i = 0; i < MaxFuelPosition * StepsPerDegree; i++)
	{
		sendCmdTo595_1( B00000000 ); //Working returning to zero
		sendCmdTo595_1( B00010000 );
		delayMicroseconds(250);
	}

       
}



void GotoMaxPort1()
// Port 1 is Lower Needle for Left Fuel Quantity
// Turning Clockwise
{
	for( int i = 0; i < MaxFuelPosition * StepsPerDegree; i++)
	{		

                sendCmdTo595_1( B10101010 );
 		sendCmdTo595_1( B00110000 );
        	delayMicroseconds(250);
	}

}

void GotoMinPort3()
// Port 3 is Upper Needle for Right Fuel Quantity
// Turning Counter clockwise
{
	for( int i = 0; i < MaxFuelPosition * StepsPerDegree; i++)
	{
		sendCmdTo595_1( B00000000 ); //Working returning to zero
		sendCmdTo595_1( B00000001 );
		delayMicroseconds(250);
	}      
}



void GotoMaxPort3()
 // Port 3 is Upper Needle for Right Fuel
 // Turning Clockwise
 // 
{
	for( int i = 0; i < MaxFuelPosition * StepsPerDegree; i++)
	{		

                sendCmdTo595_1( B10101010 );
 		sendCmdTo595_1( B00000011 );
        	delayMicroseconds(250);
	}
}

void HardZeroPort3()
 // Port 3 is Upper Needle for Right Fuel
 // Turning Clockwise
{
	for( int i = 0; i < 90 * 12; i++)
	{		

                sendCmdTo595_1( B10101010 );
 		sendCmdTo595_1( B00000011 );
        	delayMicroseconds(200);
	}

}

void GotoMax()
{
	for( int i = 0; i < 150 * 12; i++)
	{		
  
//                01xxxxxx  // StepperA CCW
//                command byte 1:
//                11xxxxxx  // StepperA step 1
//                
//                xx00xxxx // StepperB CW
//                xx10xxxx // StepperB step 1

 		//sendCmdTo595_1( B11111111 );
                sendCmdTo595_1( B10101010 );
 		sendCmdTo595_1( B11111111 );


	delayMicroseconds(180);
	}

}
void loop()
{
	int packetSize = udp.parsePacket();
        


        //GotoZero();

	if( packetSize > 0)
	{
		udp.read( packetBuffer, packetSize);
                //Serial.println("Received a packet");
		//terminate the buffer manually
		packetBuffer[packetSize] = '\0';
//		if (packetBuffer[0] == 1 )
                Serial.println( packetBuffer );
                //Serial.println( packetBuffer[0]);
                if ( packetBuffer[0] == 'S' )
		{    //check if this udp message is type 3 - EMI 
			char* endPos;
//		        Serial.println( packetBuffer );
                       // strod string to double precision
                       // At end pos is set by the function

			EMINumbers[1] = strtod( packetBuffer + 1, &endPos );
                        EMINumbers[3] = strtod( packetBuffer + 1, &endPos );

			Serial.println( EMINumbers[1] );
                        Serial.print( "2nd " );
                        Serial.println(  EMINumbers[3] );

                       
                       
                       // Split the string up
                       int i = 0;
                       char *p = packetBuffer + 1;
                       char *str;
                       while ((str = strtok_r(p, ":", &p)) != NULL) 
                       {// delimiter is the semicolon
                         Serial.print( i);
                         Serial.print("-");
                         Serial.println(str);
                         EMINumbers[i] = strtod(str, &endPos);
                         i++;
                         
                      }
                                            

                        
			target_Left_Fuel_Position =  EMINumbers[0];		
                        if (target_Left_Fuel_Position > 2040)  target_Left_Fuel_Position = 2040;  // Stop over rotating
                        target_Right_Fuel_Position =  EMINumbers[1];	
                        if (target_Right_Fuel_Position > 2040)  target_Right_Fuel_Position = 2040;  // Stop over rotating
                        
			target_Right_Fuel_Position =  EMINumbers[1];
                        target_Left_Hyd_Position = EMINumbers[2];		
                        target_Right_Hyd_Position =  EMINumbers[3];;			

		}
		
	}




	//we need to control 3 vid6606, by using 3 74hc595, the cmdGroup1~3 
	//are the bytes need to send to each 595, to make a stepper move 1 step
	// we need to send 2 bytes to 595 in order to make a pulse, so there are 2 bytes in each cmdGroupX
	byte cmdGroup1[2];
	byte cmdGroup2[2];
	byte cmdGroup3[2];

	boolean ValueHasChanged = false;

	cmdGroup1[1] = B00000000;
	if (target_Left_Hyd_Position > Left_Hyd_Position)
	{
		cmdGroup1[1] |= B11000000;
		Left_Hyd_Position++;
	}
	else if ( target_Left_Hyd_Position < Left_Hyd_Position)
	{
		cmdGroup1[1] |= B01000000;
		Left_Hyd_Position--;
	}

	if ( target_Left_Fuel_Position > Left_Fuel_Position )
	{

                cmdGroup1[1] |= B00110000;
		Left_Fuel_Position ++;
                ValueHasChanged = true;

	}
	else if ( target_Left_Fuel_Position < Left_Fuel_Position )
	{

                cmdGroup1[1] |= B00010000;
		Left_Fuel_Position --;
                ValueHasChanged = true;
	}

	if ( target_Right_Hyd_Position > Right_Hyd_Position)
	{
		cmdGroup1[1] |= B00001100;
		Right_Hyd_Position++;
	}
	else if ( target_Right_Hyd_Position < Right_Hyd_Position)
	{
		cmdGroup1[1] |= B00000100;
		Right_Hyd_Position --;
	}

	if ( target_Right_Fuel_Position > Right_Fuel_Position )
	{

                cmdGroup1[1] |= B00000011; 
		Right_Fuel_Position ++;
                ValueHasChanged = true;

	}
	else if ( target_Right_Fuel_Position < Right_Fuel_Position )
	{

                cmdGroup1[1] |= B00000001; 
		Right_Fuel_Position --;
                ValueHasChanged = true;
 
	}

	//generate the cmdGroup1[0], this is the pulse 
	cmdGroup1[0] = cmdGroup1[1] & B10101010;




	//build cmdGroup2
	cmdGroup2[1] = B00000000;
	if ( target_FFlowRight_Position > EMI_FFlowRight_Position )
	{
		cmdGroup2[1] |= B11000000;
		EMI_FFlowRight_Position++;
	}
	else if ( target_FFlowRight_Position < EMI_FFlowRight_Position )
	{
		cmdGroup2[1] |= B01000000;
		EMI_FFlowRight_Position--;
	}

	if ( target_FFlowLeft_Position > EMI_FFlowLeft_Position )
	{
		cmdGroup2[1] |= B00110000;
		EMI_FFlowLeft_Position++;
	}
	else if ( target_FFlowLeft_Position < EMI_FFlowLeft_Position )
	{
		cmdGroup2[1] |= B00010000;
		EMI_FFlowLeft_Position--;
	}

	if ( target_CoreSpdRight_Position > EMI_CoreSpdRightPosition )
	{
		cmdGroup2[1] |= B00001100;
		EMI_CoreSpdRightPosition++;
	}
	else if ( target_CoreSpdRight_Position < EMI_CoreSpdRightPosition )
	{
		cmdGroup2[1] |= B00000100;
		EMI_CoreSpdRightPosition--;
	}

	if ( target_CoreSpdLeft_Position > EMI_CoreSpdLeftPosition )
	{
		cmdGroup2[1] |= B00000011;
		EMI_CoreSpdLeftPosition++;
	}
	else if ( target_CoreSpdLeft_Position < EMI_CoreSpdLeftPosition )
	{
		cmdGroup2[1] |= B00000001;
		EMI_CoreSpdLeftPosition--;
	}
	cmdGroup2[0] = cmdGroup2[1] & B10101010;



	cmdGroup3[1] = B00000000;
	if ( target_FanSpdRight_Position > EMI_FanSpdRight_Position )
	{
		cmdGroup3[1] |= B11000000;
		EMI_FanSpdRight_Position++;
	}
	else if ( target_FanSpdRight_Position < EMI_FanSpdRight_Position )
	{
		cmdGroup3[1] |= B01000000;
		EMI_FanSpdRight_Position--;
	}

	if ( target_FanSpdLeft_Position > EMI_FanSpdLeft_Position )
	{
		cmdGroup3[1] |= B00110000;
		EMI_FanSpdLeft_Position++;
	}
	else if ( target_FanSpdLeft_Position < EMI_FanSpdLeft_Position )
	{
		cmdGroup3[1] |= B00010000;
		EMI_FanSpdLeft_Position--;
	}

	if ( target_ITTRight_Position > EMI_ITTRight_Position )
	{
		cmdGroup3[1] |= B00001100;
		EMI_ITTRight_Position++;
	}
	else if ( target_ITTRight_Position < EMI_ITTRight_Position )
	{
		cmdGroup3[1] |= B00000100;
		EMI_ITTRight_Position--;
	}

	if ( target_ITTLeft_Position > EMI_ITTLeft_Position )
	{
		cmdGroup3[1] |= B00000011;
		EMI_ITTLeft_Position++;
	}
	else if ( target_ITTLeft_Position < EMI_ITTLeft_Position )
	{
		cmdGroup3[1] |= B00000001;
		EMI_ITTLeft_Position--;
	}
	cmdGroup3[0] = cmdGroup3[1] & B10101010;

	//move the stepper as required
	sendCmdTo595_1(  cmdGroup1[0] );
	sendCmdTo595_2(  cmdGroup2[0] );
	sendCmdTo595_3(  cmdGroup3[0] );


	sendCmdTo595_1(  cmdGroup1[1] );
	sendCmdTo595_2(  cmdGroup2[1] );
	sendCmdTo595_3(  cmdGroup3[1] );

        if (ValueHasChanged = true)
        {
          delayMicroseconds(780);
        }


}



