#include <SPI.h>
#include <SimpleTimer.h>
#include <Limits.h>
#include "printf.h"

typedef enum { role_sensor = 1, role_controller } role_e;

// 필수 데이터 셋팅
role_e g_role = role_controller;
const int myindex = 0; // 0~ 센서의 갯수만큼 위치에 따라 차례로 설정해야 함.
const int sensorcnt = 2;

// 디지털핀 시그널을 통해 시간 동기화
int gTimeSyncPin = 9;   // 두에의 경우 모든 핀에 인터럽트 가능.
int gTimeSyncRecvPin[4] = {2, 3, 21, 20};  // 우노의 경우 2, 3번만 인터럽트 가능 => 메가로 변경 필요(2, 3, 21, 20, 19, 18)

SimpleTimer sync_timer;
int _timer_id = 0;

int sensitivity = 650;
unsigned long synctime = 1000000; // 300 msec
unsigned long stime = 0;
unsigned long startreadingtime = 0;
bool bOneShot = false;
volatile int iReadSensorCnt = 0;

void BaseSetting(role_e role)
{
  if (role == role_sensor)
  {
    Serial.println("Base Pipe Setting : role_sensor");
    pinMode(gTimeSyncPin, INPUT_PULLUP);  
    attachInterrupt(digitalPinToInterrupt(gTimeSyncPin), SensorInterrupt, RISING);
    pinMode(gTimeSyncRecvPin[myindex], OUTPUT);		
  }
  else if (role == role_controller)
  {
    Serial.println("Base Pipe Setting : role_controller");
    pinMode(gTimeSyncPin, OUTPUT);
	for (int i = 0; i < sensorcnt; i++)
		pinMode(gTimeSyncRecvPin[i], INPUT_PULLUP); // 인터럽트 핀으로 사용
	attachInterrupt(digitalPinToInterrupt(gTimeSyncRecvPin[0]), TimeInterruptRecv0, RISING);
	attachInterrupt(digitalPinToInterrupt(gTimeSyncRecvPin[1]), TimeInterruptRecv0, RISING);
	attachInterrupt(digitalPinToInterrupt(gTimeSyncRecvPin[2]), TimeInterruptRecv0, RISING);
	attachInterrupt(digitalPinToInterrupt(gTimeSyncRecvPin[3]), TimeInterruptRecv0, RISING);

   _timer_id = sync_timer.setInterval(60000, KeepAliveCallback);	// 1분 타이머
    Serial.print(_timer_id); Serial.println(" : keep alive Timer Started");
  }
}

void SensorInterrupt()  // 센스쪽에서 발생. 리셋등에 활용
{
}

volatile unsigned long recvtime[4] = {0, };
void TimeInterruptRecv0()
{
  recvtime[0] = micros();
  iReadSensorCnt++;
  //Serial.print(recvtime[0]);  Serial.println("  interrupt pin2");
}
void TimeInterruptRecv1()
{
  recvtime[1] = micros();
  iReadSensorCnt++;
  //Serial.print(recvtime[1]); Serial.println("  interrupt pin3");
}
void TimeInterruptRecv2()
{
  recvtime[2] = micros();
  iReadSensorCnt++;
}
void TimeInterruptRecv3()
{
  recvtime[3] = micros();
  iReadSensorCnt++;
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
  BaseSetting(g_role);
  stime = micros();
}

// Keep alive check
void KeepAliveCallback()
{
}

void loop(void)
{
  if (g_role == role_sensor)
  {
    int _valuex = analogRead(A0);
    int _valuey = analogRead(A1);
    int _valuez = analogRead(A2);     // 효율성을 위해 확인 후 제거.. Z축은 무의미한 값일듯.
    if ((_valuex > sensitivity || _valuey > sensitivity || _valuez > sensitivity) && bOneShot == false)
    {
       bOneShot = true;
       stime = micros();
       digitalWrite(gTimeSyncRecvPin[myindex], HIGH);
       digitalWrite(gTimeSyncRecvPin[myindex], LOW);
       Serial.print("imapct sensor : "); Serial.println(gTimeSyncRecvPin[myindex]);
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
  // 첫번째 신호가 수집된후 30ms 이상이 지나면 데이터 무효화, 새로운 타격으로 처리.
  if (g_role == role_controller)
  {

	  if (iReadSensorCnt > 0)
	  {
		  if (startreadingtime == 0)
			  startreadingtime = micros();

		  if (iReadSensorCnt == sensorcnt)
		  {
			  long _duetime = recvtime[0] - recvtime[1];
			  Serial.print("************* ImpactTime : ");
			  Serial.println(_duetime);
			  iReadSensorCnt = 0;
			  startreadingtime = 0;
			  recvtime[0] = recvtime[1] = recvtime[2] = recvtime[3] = 0;
		  }
		  if (startreadingtime != 0 && micros() - startreadingtime > 30000) // 하나의 값만 수신
		  {
			  iReadSensorCnt = 0;
			  startreadingtime = 0;
			  recvtime[0] = recvtime[1] = recvtime[2] = recvtime[3] = 0;
			  Serial.print("---------------- clear ImpactTime  ");
		  }
      }
 
  }
}
