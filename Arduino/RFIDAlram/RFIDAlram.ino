/*
 Name:		RFIDAlram.ino
 Created:	2017-06-22 오후 2:47:19
 Author:	nova
*/

#include "AddicoreRFID.h"
#include <SPI.h>

#define uchar   unsigned char
#define uint    unsigned int

//4 bytes tag serial number, the first 5 bytes for the checksum byte
uchar serNumA[5];

uchar fifobytes;
uchar fifoValue;

AddicoreRFID myRFID; // create AddicoreRFID object to control the RFID module

					 /////////////////////////////////////////////////////////////////////
					 //set the pins
					 /////////////////////////////////////////////////////////////////////
const int chipSelectPin = 10;
const int NRSTPD = 9;

//Maximum length of the array
#define MAX_LEN 16

void setup() {
	Serial.begin(9600);                        // RFID reader SOUT pin connected to Serial RX pin at 9600bps

											   // start the SPI library:
	SPI.begin();

	pinMode(chipSelectPin, OUTPUT);              // Set digital pin 10 as OUTPUT to connect it to the RFID /ENABLE pin
	digitalWrite(chipSelectPin, LOW);         // Activate the RFID reader
	pinMode(NRSTPD, OUTPUT);                     // Set digital pin 10 , Not Reset and Power-down
	digitalWrite(NRSTPD, HIGH);

	myRFID.AddicoreRFID_Init();
}

void loop()
{
	uchar i, tmp, checksum1;
	uchar status;
	uchar str[MAX_LEN];
	uchar RC_size;
	uchar blockAddr;  //Selection operation block address 0 to 63
	String mynum = "";
	str[1] = 0x4400;

	//Find tags, return tag type
	status = myRFID.AddicoreRFID_Request(PICC_REQIDL, str);
	if (status == MI_OK)
	{
		Serial.println("RFID tag detected");
		Serial.print(str[0], BIN);
		Serial.print(" , ");
		Serial.print(str[1], BIN);
		Serial.println(" ");
	}

	//Anti-collision, return tag serial number 4 bytes
	status = myRFID.AddicoreRFID_Anticoll(str);
	if (status == MI_OK)
	{
		checksum1 = str[0] ^ str[1] ^ str[2] ^ str[3];
		Serial.println("The tag's number is  : ");
		//Serial.print(2);
		Serial.print(str[0]);
		Serial.print(" , ");
		Serial.print(str[1], BIN);
		Serial.print(" , ");
		Serial.print(str[2], BIN);
		Serial.print(" , ");
		Serial.print(str[3], BIN);
		Serial.print(" , ");
		Serial.print(str[4], BIN);
		Serial.print(" , ");
		Serial.println(checksum1, BIN);

		//이 아래 코드를 수정하여 자기 카드인지 아닌지 확인합니다.
		if (str[0] == 156)  //현재 코드에서는 가장 첫번째(str[0])만으로 비교하는데 늘리셔도 됩니다.                    
		{                  //또한 코드에는 '156'으로 되어있지만 자신의 카드를 확인하여 변경하시면 됩니다.
			Serial.print("Hello Craig!\n");
		}
		else if (str[0] == 244) {
			Serial.print("Hello Erin!\n");
		}
		Serial.println();
		delay(1000);
	}

	myRFID.AddicoreRFID_Halt();        //Command tag into hibernation             

}

