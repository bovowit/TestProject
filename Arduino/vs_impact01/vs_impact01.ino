#include <RF24_config.h>
#include <SPI.h>
#include <SimpleTimer.h>
#include <Limits.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

typedef enum { role_sensor = 1, role_controller } role_e;

// 필수 데이터 셋팅
role_e g_role = role_controller;
const int myindex = 0; // 0~ 센서의 갯수만큼 위치에 따라 차례로 설정해야 함.
const int sensorcnt = 2;

// translate
RF24 radio(7, 8);
const uint64_t pipes[4][2] = { { 0xFF0A10, 0xFF0A11 },{ 0xFF0A20, 0xFF0A21 },{ 0xFF0A30, 0xFF0A31 },{ 0xFF0A40, 0xFF0A41 } };
const char sCmdTimeSync[] = "_RS_";					// 리셋명령.

// 디지털핀 시그널을 통해 시간 동기화
int gTimeSyncPin = 9;		// 두에의 경우 모든 핀에 인터럽트 가능.
int gTimeSyncRecvPin0 = 2;	// 우노의 경우 2, 3번만 인터럽트 가능 => 메가로 변경 필요(2, 3, 21, 20, 19, 18)
int gTimeSyncRecvPin1 = 3;
int gTimeSyncRecvPin2 = 4;
int gTimeSyncRecvPin3 = 5;

SimpleTimer sync_timer;
int _timer_id = 0;

// data
const int sensorbuffsize = 16;
const int buffsize = 16;
byte data[sensorbuffsize] = { 0, };    // 송신용
byte command[buffsize] = { 0, };  // 일반 수신용 (명령어 등)
byte recv[sensorcnt][sensorbuffsize];

int sensitivity = 650;
unsigned long synctime = 300000; // 300 msec
unsigned long stime = 0;
unsigned long startreadingtime = 0;
unsigned long base_sync_time = 0;
bool bOneShot = false;
int iReadSensorCnt = 0;

void BaseSetting(role_e role)
{
	if (role == role_sensor)
	{
		radio.openReadingPipe(1, pipes[1][0]);
		radio.openWritingPipe(pipes[myindex][1]);
		radio.startListening();
		Serial.println("Base Pipe Setting : role_sensor");

		pinMode(gTimeSyncPin, INPUT_PULLUP);	// 인터럽트 핀.
		attachInterrupt(digitalPinToInterrupt(gTimeSyncPin), TimeInterrupt, RISING);
		pinMode(gTimeSyncRecvPin0, OUTPUT);
	}
	else if (role == role_controller)
	{
		radio.openReadingPipe(0, pipes[0][1]);
		radio.openReadingPipe(1, pipes[1][1]);
		radio.openReadingPipe(2, pipes[2][1]);
		radio.openReadingPipe(3, pipes[3][1]);
		radio.openWritingPipe(pipes[0][0]); // broadcasting all sensor
		radio.startListening();
		Serial.println("Base Pipe Setting : role_controller");

		pinMode(gTimeSyncPin, OUTPUT);
		pinMode(gTimeSyncRecvPin0, INPUT_PULLUP);	// 인터럽트 핀으로 사용
		pinMode(gTimeSyncRecvPin1, INPUT_PULLUP);
		pinMode(gTimeSyncRecvPin2, INPUT_PULLUP);
		pinMode(gTimeSyncRecvPin3, INPUT_PULLUP);
		attachInterrupt(digitalPinToInterrupt(gTimeSyncRecvPin0), TimeInterruptRecv0, RISING);
		attachInterrupt(digitalPinToInterrupt(gTimeSyncRecvPin1), TimeInterruptRecv1, RISING);
		attachInterrupt(digitalPinToInterrupt(gTimeSyncRecvPin2), TimeInterruptRecv2, RISING);
		attachInterrupt(digitalPinToInterrupt(gTimeSyncRecvPin3), TimeInterruptRecv3, RISING);

		_timer_id = sync_timer.setInterval(3000, SyncTimerCallback);
		Serial.print(_timer_id); Serial.println(" : Timer Started");

	}
}

void TimeInterrupt()	// 센스쪽에서 발생. 
{
	base_sync_time = micros();
}
void TimeInterruptRecv0()
{
	gTimeSyncRecvCnt++;
}
void TimeInterruptRecv1()
{
	gTimeSyncRecvCnt++;
}
void TimeInterruptRecv2()
{
	gTimeSyncRecvCnt++;
}
void TimeInterruptRecv3()
{
	gTimeSyncRecvCnt++;
}

void setup()
{
	Serial.begin(115200);
	printf_begin();
	if (g_role == role_controller)
		Serial.println(F("*** Selected MainController mode ***"));
	else
	{
		Serial.print(F("*** Selected Sensor mode ***  Sensor Index : "));
		Serial.print(myindex); Serial.println("");
	}
	// Setup and configure rf radio
	radio.begin();
	radio.powerUp();
	radio.setAutoAck(1);                    // Ensure autoACK is enabled
	radio.enableAckPayload();               // Allow optional ack payloads

	//uint8_t dpsize = radio.getDynamicPayloadSize();
	//printf("DynamicPayloadSize = %d \r\n", dpsize);
	//radio.enableDynamicPayloads() ;      //

	radio.setRetries(0, 10);                 // Smallest time between retries, max no. of retries
	radio.setPayloadSize(buffsize);

	BaseSetting(g_role);
	radio.printDetails();                   // Dump the configuration of the rf unit for debugging

	stime = micros();
}

// timer 이벤트가 발생하면 디지털 핀 High setting
int gTimeSyncRecvCnt = 0;
bool gFirstTimeSync = true;
void SyncTimerCallback()
{
	gTimeSyncRecvCnt = 0;
	if (gFirstTimeSync == false)
	{
		Serial.println("LongTimer -- Digital pin HIGH for Time Sync Interrupt");
		sync_timer.disable(_timer_id);
		_timer_id = sync_timer.setInterval(3000, SyncTimerCallback);
		sync_timer.restartTimer(_timer_id);
		gFirstTimeSync = true;
	}
	// 타이머 
	Serial.println("InitTimer -- Digital pin HIGH for Time Sync Interrupt");
	digitalWrite(gTimeSyncPin, HIGH);
	delayMicroseconds(10);
	digitalWrite(gTimeSyncPin, LOW);
}

void loop(void)
{
	if (g_role == role_sensor)
	{
		if (radio.available())				// 반응속도 저하 요인이 될수 있음. 체크 후 제거 검토.
		{
			radio.read(command, buffsize);
			printf("RECV command : %s from master \n\r", (char*)&command);
			String sCmd((char*)command);
			if (!sCmd.equals("_RS_"))		// 리셋처리..
			{
				// reset....
				Serial.println("recv Reset command........");
			}
			memset(command, 0, buffsize);
		}

		int _valuex = analogRead(A0);
		int _valuey = analogRead(A1);
		int _valuez = analogRead(A2);			// 효율성을 위해 확인 후 제거.. Z축은 무의미한 값일듯.
		if ((_valuex > sensitivity || _valuey > sensitivity || _valuez > sensitivity) && bOneShot == false)
		{
			bOneShot = true;
			stime = micros();

			// delta time & rollover 처리
			unsigned long _delta_time = stime - base_sync_time;
			if (_delta_time < 0)
				_delta_time += ULONG_MAX;

			radio.stopListening();
			delayMicroseconds(1);
			if (!radio.write(&_delta_time, sizeof(_delta_time)))
			{
				Serial.println(F("failed sending. clear data "));
			}
			radio.startListening();       // 메인보드로부터 명령어를 받기 위해.
			printf("Detected Impact - %d - %d - %d : time %d \n\r", _valuex, _valuey, _valuez, stime);
			delayMicroseconds(1000);
		}

		if (bOneShot)
		{
			if (micros() - stime > synctime)   // 300 msec 마다 무효화, 노이즈 처리
			{
				stime = 0;
				bOneShot = false;
			}
		}
	}


	// Pong back role.  Receive each packet, dump it out, and send it back
	// 메인컨트롤러는 다수의 수집장치로부터 데이터를 수신해서 조합 후 PC로 전송.(최소 4개의 장치중 3개 이상 수집)
	// 첫번째 신호가 수집된후 10ms 이상이 지나면 데이터 무효화, 새로운 타격으로 처리.
	if (g_role == role_controller)
	{
		if (gTimeSyncRecvCnt == sensorcnt)
		{
			sync_timer.disable(_timer_id);
			Serial.println(F("success time sync.. so reset long timer setting"));
			_timer_id = sync_timer.setInterval(3600000, SyncTimerCallback);
			sync_timer.restartTimer(_timer_id);
			gFirstTimeSync = false;
		}
		else
		{
			sync_timer.run();
		}


		uint8_t  sensorIndex = -1;
		while (radio.available(&sensorIndex))
		{
			Serial.print((unsigned long)sensorIndex);   Serial.println(" : Pipe... recv availabled");
			if (sensorIndex > sensorcnt || sensorIndex < 0)
			{
				Serial.print((unsigned long)sensorIndex);
				Serial.println(" : receive from Invalid sensor index.... ");
				break;
			}

			if (iReadSensorCnt == 0)
				startreadingtime = micros();
			iReadSensorCnt++;
			radio.read(recv[sensorIndex], sensorbuffsize);

		}

		unsigned long _curtime = micros();
		if ((iReadSensorCnt >= sensorcnt || _curtime - startreadingtime > synctime) && iReadSensorCnt > 0)
		{
			if (iReadSensorCnt < sensorcnt)    // 주어진 시간에 3개 이상의 센서 값을 읽지 못한 경우.
			{
				Serial.print(iReadSensorCnt);
				Serial.println(" : not enough gather sensor data.. so reset data");
				iReadSensorCnt = 0;
				startreadingtime = 0;
				return;
			}

			iReadSensorCnt = 0;
			startreadingtime = 0;

			// 적당히 모아서 PC로 전송.
			unsigned long _time1;
			unsigned long _time2;
			Serial.print("/");
			for (int i = 0; i < sensorcnt; i++)
			{
				unsigned long _time;
				memcpy(&_time, recv[i], sizeof(unsigned long));
				if (i == 0)
					_time1 = _time;
				if (i == 1)
					_time2 = _time;
				Serial.print(_time);
				Serial.print("|");
			}
			Serial.println("");
			Serial.print("Interval : ");
			int _interval = _time1 - _time2;
			Serial.println(_interval);

			for (int i = 0; i < sensorcnt; i++)
			{
				memset(recv[i], 0, sensorbuffsize);
			}
		}


	}

}
