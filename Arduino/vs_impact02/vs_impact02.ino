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
const char sCmdTimeSync[] = "_RS_";          // 리셋명령.

// 디지털핀 시그널을 통해 시간 동기화
int gTimeSyncPin = 9;   // 두에의 경우 모든 핀에 인터럽트 가능.
int gTimeSyncRecvPin[4] = {2, 3, 21, 20};  // 우노의 경우 2, 3번만 인터럽트 가능 => 메가로 변경 필요(2, 3, 21, 20, 19, 18)

SimpleTimer sync_timer;
int _timer_id = 0;

// data
const int sensorbuffsize = 16;
const int buffsize = 16;
byte data[sensorbuffsize] = { 0, };    // 송신용
byte command[buffsize] = { 0, };  // 일반 수신용 (명령어 등)
byte recv[sensorcnt][sensorbuffsize];

int sensitivity = 650;
unsigned long synctime = 1000000; // 300 msec
unsigned long stime = 0;
unsigned long startreadingtime = 0;
unsigned long base_sync_time = 0;
bool bOneShot = false;
volatile int iReadSensorCnt = 0;

void BaseSetting(role_e role)
{
  if (role == role_sensor)
  {
    Serial.println("Base Pipe Setting : role_sensor");

    pinMode(gTimeSyncPin, INPUT_PULLUP);  // 인터럽트 핀.
    attachInterrupt(digitalPinToInterrupt(gTimeSyncPin), TimeInterrupt, RISING);
    pinMode(gTimeSyncRecvPin[myindex], OUTPUT);
  }
  else if (role == role_controller)
  {
    Serial.println("Base Pipe Setting : role_controller");

    pinMode(gTimeSyncPin, OUTPUT);
    pinMode(gTimeSyncRecvPin[0], INPUT_PULLUP); // 인터럽트 핀으로 사용
    pinMode(gTimeSyncRecvPin[1], INPUT_PULLUP);
    pinMode(gTimeSyncRecvPin[2], INPUT_PULLUP);
    pinMode(gTimeSyncRecvPin[3], INPUT_PULLUP);
    // 두에는 핀 번호를 인터럽트로 사용
    attachInterrupt(digitalPinToInterrupt(gTimeSyncRecvPin[0]), TimeInterruptRecv0, RISING);
    attachInterrupt(digitalPinToInterrupt(gTimeSyncRecvPin[1]), TimeInterruptRecv1, RISING);
    attachInterrupt(digitalPinToInterrupt(gTimeSyncRecvPin[2]), TimeInterruptRecv2, RISING);
    attachInterrupt(digitalPinToInterrupt(gTimeSyncRecvPin[3]), TimeInterruptRecv3, RISING);

   _timer_id = sync_timer.setInterval(3000, SyncTimerCallback);
    Serial.print(_timer_id); Serial.println(" : Timer Started");

  }
}

void TimeInterrupt()  // 센스쪽에서 발생. 
{
}

volatile unsigned long recvtime[4] = {0, };
void TimeInterruptRecv0()
{
  iReadSensorCnt++;
  recvtime[0] = micros();
  Serial.print(recvtime[0]);  Serial.println("  interrupt pin2");
}
void TimeInterruptRecv1()
{
  iReadSensorCnt++;
  recvtime[1] = micros();
  Serial.print(recvtime[1]); Serial.println("  interrupt pin3");
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

// timer 이벤트가 발생하면 디지털 핀 High setting
int gTimeSyncRecvCnt = 0;
bool gFirstTimeSync = true;
void SyncTimerCallback()
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
  // 첫번째 신호가 수집된후 10ms 이상이 지나면 데이터 무효화, 새로운 타격으로 처리.
  if (g_role == role_controller)
  {

      if (iReadSensorCnt >= sensorcnt)
      {
        long _duetime = recvtime[0] - recvtime[1];
        Serial.print("************* ImpactTime : ");
        Serial.println(_duetime);
        iReadSensorCnt = 0;
               startreadingtime = 0; 
        recvtime[0] = recvtime[1] = recvtime[2] = recvtime[3] = 0;
      }
 
  }
}
