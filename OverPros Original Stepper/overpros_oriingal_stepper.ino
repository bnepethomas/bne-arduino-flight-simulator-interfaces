/*
    
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

byte mac[] = { 
	0xA9,0xE7,0x3E,0xCA,0x34,0x1E};
IPAddress ip(192,168,1,105);
const unsigned int localport = 13135;

EthernetUDP udp;
char packetBuffer[200]; //buffer to store the incoming data

const int MaxSteps = 3780;

void setup()
{

	Ethernet.begin( mac, ip);
	udp.begin( localport );

	Serial.begin(115200);    

	for( int i = 22; i <= 42; i++)
	{
		pinMode( i, OUTPUT );
	}
	GotoZero();
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

double EMI_APUEGT_Position = 0, EMI_APURPM_Position = 0, EMI_EngOPRightPosition = 0, EMI_EngOPLeftPosition = 0,
	EMI_FFlowRight_Position = 0, EMI_FFlowLeft_Position = 0, EMI_CoreSpdRightPosition = 0, EMI_CoreSpdLeftPosition = 0,
	EMI_FanSpdRight_Position = 0, EMI_FanSpdLeft_Position = 0, EMI_ITTRight_Position = 0, EMI_ITTLeft_Position = 0;

double target_APUEGT_Position, target_APURPM_Position, target_EngOPRight_Position, target_EngOPLeft_Position,
	target_FFlowRight_Position, target_FFlowLeft_Position, target_CoreSpdRight_Position, target_CoreSpdLeft_Position,
	target_FanSpdRight_Position, target_FanSpdLeft_Position, target_ITTRight_Position, target_ITTLeft_Position;

void GotoZero()
{
	for( int i = 0; i < 360 * 12; i++)
	{
		sendCmdTo595_1( B00000000 );
		sendCmdTo595_2( B00000000 );
		sendCmdTo595_3( B00000000 );

		sendCmdTo595_1( B01010101 );
		sendCmdTo595_2( B01010101 );
		sendCmdTo595_3( B01010101 );

		delayMicroseconds(480);
	}
}

void loop()
{
	int packetSize = udp.parsePacket();

	if( packetSize > 0)
	{
		udp.read( packetBuffer, packetSize);
		//terminate the buffer manually
		packetBuffer[packetSize] = '\0';
		if (packetBuffer[0] == 1 )
		{    //check if this udp message is type 3 - EMI 
			char* endPos;
			//            Serial.println( packetBuffer );
			EMINumbers[0] = strtod( packetBuffer + 2, &endPos );
			//            Serial.println( EMINumbers[0] );
			for( int i = 1; i < 12; i++)
			{
				char *startPos = endPos;
				EMINumbers[i] = strtod( endPos, &endPos );
				//Serial.print( i );
				//Serial.print( '-' );
				//Serial.println( EMINumbers[i], 5 );
			}
			target_APUEGT_Position = EMINumbers[0];			//260 Deg
			target_APURPM_Position =  EMINumbers[1];		//240 Deg
			target_EngOPRight_Position =  EMINumbers[2];	//270 Deg
			target_EngOPLeft_Position =  EMINumbers[3];		//270 Deg

			target_FFlowRight_Position =  EMINumbers[4];	// 300 Deg
			target_FFlowLeft_Position =  EMINumbers[5];    	// 300 Deg
			target_CoreSpdRight_Position =  EMINumbers[6]; 	// 270 
			target_CoreSpdLeft_Position =  EMINumbers[7];  	// 270

			target_FanSpdRight_Position =  EMINumbers[8];   //315
			target_FanSpdLeft_Position =  EMINumbers[9];    //315
			target_ITTRight_Position =  EMINumbers[10];		//260
			target_ITTLeft_Position = EMINumbers[11];		//260
		}
		
	}




	//we need to control 3 vid6606, by using 3 74hc595, the cmdGroup1~3 
	//are the bytes need to send to each 595, to make a stepper move 1 step
	// we need to send 2 bytes to 595 in order to make a pulse, so there are 2 bytes in each cmdGroupX
	byte cmdGroup1[2];
	byte cmdGroup2[2];
	byte cmdGroup3[2];

	
	cmdGroup1[1] = B00000000;
	if (target_APUEGT_Position > EMI_APUEGT_Position)
	{
		cmdGroup1[1] |= B11000000;
		EMI_APUEGT_Position++;
	}
	else if ( target_APUEGT_Position < EMI_APUEGT_Position)
	{
		cmdGroup1[1] |= B01000000;
		EMI_APUEGT_Position--;
	}

	if ( target_APURPM_Position > EMI_APURPM_Position )
	{
		cmdGroup1[1] |= B00110000;
		EMI_APURPM_Position ++;
	}
	else if ( target_APURPM_Position < EMI_APURPM_Position )
	{
		cmdGroup1[1] |= B00010000;
		EMI_APURPM_Position --;
	}

	if ( target_EngOPRight_Position > EMI_EngOPRightPosition)
	{
		cmdGroup1[1] |= B00001100;
		EMI_EngOPRightPosition++;
	}
	else if ( target_EngOPRight_Position < EMI_EngOPRightPosition)
	{
		cmdGroup1[1] |= B00000100;
		EMI_EngOPRightPosition --;
	}

	if ( target_EngOPLeft_Position > EMI_EngOPLeftPosition )
	{
		cmdGroup1[1] |= B00000011;
		EMI_EngOPLeftPosition ++;
	}
	else if ( target_EngOPLeft_Position < EMI_EngOPLeftPosition )
	{
		cmdGroup1[1] |= B00000001;
		EMI_EngOPLeftPosition --;
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

}



