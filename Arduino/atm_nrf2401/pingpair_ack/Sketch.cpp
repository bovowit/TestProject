
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

// 필수 데이터 셋팅
const int mymode = 1;  // 1 메인 컨트롤러, 2 센서 데이터 수집장치
const int myindex = 0; // 0~ 센서의 갯수만큼 위치에 따라 차례로 설정해야 함.

// translate
RF24 radio(7, 8);
const uint64_t pipes[2][4] = { (0xFF00FF00, 0xFF00FF01), (0xFF00FF10, 0xFF00FF11), (0xFF00FF20, 0xFF00FF21), (0xFF00FF30, 0xFF00FF31) };
typedef enum { role_sesor = 1, role_controler } role_e;
const char* role_friendly_name[] = { "invalid", "Main controller", "Sensor data" };
role_e role = role_controler;

// data
const int sensorcnt = 1;
const int sensorbuffsize = 32;
const int buffsize = 32;
byte counter = 1;
byte data[sensorbuffsize] = { 0, };    // 송신용
byte command[buffsize] = { 0, };  // 일반 수신용 (명령어 등)
byte recv[sensorcnt][sensorbuffsize];

int sensitivity = 700;
unsigned long synctime = 300000; // 30 msec
unsigned long stime = 0;
unsigned long sreadtime = 0;
unsigned long currenttime = 0;
bool bOneShot = false;
int bReadSensorCnt = 0;

void setup()
{
	Serial.begin(115200);
	printf_begin();

	Serial.print(F("\n\rRF24/examples/pingpair_ack/\n\rROLE: "));
	Serial.println(role_friendly_name[role]);
	if(mymode == 1)
	Serial.println(F("*** Selected MainController mode ***"));
	else
	{
		Serial.print(F("*** Selected Sensor mode ***  Sensor Index : "));
		Serial.print(myindex); Serial.println("");
	}
	// Setup and configure rf radio
	radio.begin();
	radio.setAutoAck(1);                    // Ensure autoACK is enabled
	radio.enableAckPayload();               // Allow optional ack payloads

	//uint8_t dpsize = radio.getDynamicPayloadSize();
	//printf("DynamicPayloadSize = %d \r\n", dpsize);
	//radio.enableDynamicPayloads()       //
	
	radio.setRetries(0, 15);                 // Smallest time between retries, max no. of retries

	//radio.setPayloadSize(1);                // Here we are sending 1-byte payloads to test the call-response speed
	radio.setPayloadSize(buffsize);

	BasePipeSetting(mymode);
	//radio.openWritingPipe(pipes[1]);        // Both radios listen on the same pipes by default, and switch when writing
	//radio.openReadingPipe(1, pipes[0]);
	//radio.startListening();                 // Start listening
	radio.printDetails();                   // Dump the configuration of the rf unit for debugging

	stime = micros();
}

void BasePipeSetting(int role)
{
	if(role == role_sesor)
	{
		radio.openReadingPipe(1, pipes[myindex][0]);
		radio.openWritingPipe(pipes[myindex][1]);
		radio.startListening();
	}
	else if(role == role_controler)
	{
		radio.openReadingPipe(1, pipes[0][0]);
		radio.openReadingPipe(2, pipes[1][0]);
		radio.openReadingPipe(3, pipes[2][0]);
		radio.openReadingPipe(4, pipes[3][0]);
		radio.startListening();
		//radio.openWritingPipe(pipes[myindex][1]); // 필요할때마다 open.
	}
}

void loop(void)
{
	if (role == role_sesor)
	{
		if (radio.available())
		{
			radio.read(&command, buffsize);
			printf("RECV command : %s from master \n\r", (char*)&command);
			String sCmd((char*)command);
			if (! sCmd.equals("_TIME_SYNC_")) // (sCmd == "_TIME_SYNC_") // sCmd.compareTo("_");
			{
				currenttime = micros();
				radio.write(&currenttime, 8);
				Serial.println("answer sync time ............");
			}
		}

		int _valuex = analogRead(A0);
		int _valuey = analogRead(A1);
		int _valuez = analogRead(A2);
		if ((_valuex > sensitivity || _valuey > sensitivity || _valuez > sensitivity) && bOneShot == false)
		{
			bOneShot = true;
			stime = micros();
			printf("Detected Impact - %d - %d - %d : time %d \n\r", _valuex, _valuey, _valuez, stime);
		}
		else
		{
			if (micros() - stime > synctime)   // 30 msec 마다 무효화, 노이즈 처리
			{
				stime = 0;
				bOneShot = false;
			}
			return;
		}

		memcpy(data, &stime, sizeof(stime));
		Serial.print("====send===");
		for(int i = 0; i < buffsize; i++)
		{
			Serial.print(data[i]); Serial.print(" | ");
		}
		Serial.println("");

		radio.stopListening();               // First, stop listening so we can talk.
		if (!radio.write(data, sensorbuffsize))
		{
			Serial.println(F("failed sending. clear data "));
		}
		memset(data, 0, sensorbuffsize);

		radio.startListening();       // 메인보드로부터 명령어를 받기 위해.
	}


	// Pong back role.  Receive each packet, dump it out, and send it back
	// 메인컨트롤러는 다수의 수집장치로부터 데이터를 수신해서 조합 후 PC로 전송.(최소 4개의 장치중 3개 이상 수집)
	// 첫번째 신호가 수집된후 10ms 이상이 지나면 데이터 무효화, 새로운 타격으로 처리.
	//
	if (role == role_controler)
	{
		byte pipeNo;
		byte gotByte;
		int sensorIndex = 0;
		while (radio.available(&pipeNo))
		{
			for(sensorIndex; sensorIndex < sensorcnt; sensorIndex++ )
			{
				if(pipeNo == pipes[sensorIndex][0])
				break;
			}
			if(sensorIndex >= sensorcnt)
			{
				Serial.println("receive from Invalid sensor index.... ");
				break;
			}
			if(bReadSensorCnt == 0)
			sreadtime = micros();
			bReadSensorCnt++;
			radio.read(recv[sensorIndex], sensorbuffsize);
			//radio.writeAckPayload(pipeNo, &rBuff, 1);
		}

		unsigned long _curtime =  micros();
		if(bReadSensorCnt >= sensorcnt || _curtime - sreadtime > synctime)
		{
			if(bReadSensorCnt < 3)    // 주어진 시간에 3개 이상의 센서 값을 읽지 못한 경우.
			{
				Serial.println("not enough gather sensor data.. so reset data");
				bReadSensorCnt = 0;
				sreadtime = 0;
				return;
			}

			bReadSensorCnt = 0;
			sreadtime = 0;

			// 적당히 모아서 PC로 전송.

			Serial.print("=========recv : ");
			for(int i = 0; i < buffsize; i++)
			{
				Serial.print(recv[i][0]); Serial.print(" : ");
			}
			Serial.println("");

			for(int i = 0; i < sensorcnt; i++)
			{
				memset(recv[i], 0, sensorbuffsize);
			}
		}

		//if(_curtime > ULONG_MAX - 100000 )    // 주기적인 calibration ...
		
	}
	
}
