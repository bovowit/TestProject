﻿/*
 Name:		AutoLamp.ino
 Created:	2017-06-19 오후 4:17:59
 Author:	nova
*/
// 동작감지&시간체크 -> 조도체크&OnOff스위치체크 -> 전원제어
// 동작감시 인터럽트
#include "MsTimer2.h"

// 기준값
uint g_illumBase = 100;			// 기준 조도값, 어두운 밤 기준으로 측정후 셋팅.
uint g_onDelayTime = 600;		// 스위치 On일때, 움직임 없어 전원 Off 할때까지 지속시간
uint g_offDelayTime = 60;		// 스위치 Off일때, 움직임 감지 후 전원 On 지속시간

// 핀 정의
int pinMotionInterrupt = 2;		// 동작감시
int pinSwitchInterrupt = 3;		// On/Off 스위치
int pinIlluminance = A0;		// 조도센서
int pinLamp = 7;				// 램프제어 릴레이

// 상태
volatile byte g_moveFlag = LOW;
volatile byte g_switchFlag = LOW;
volatile unsigned long g_timeCount = 0;

// the setup function runs once when you press reset or power the board
void setup() 
{
	Serial.begin(9600);
	pinMode(pinMotionInterrupt, INPUT_PULLUP);
	attachInterrupt(digitalPinToInterrupt(pinMotionInterrupt), motion, RISING);// CHANGE);
	pinMode(pinSwitchInterrupt, INPUT_PULLUP);
	attachInterrupt(digitalPinToInterrupt(pinSwitchInterrupt), onoff, CHANGE);
	
	pinMode(pinLamp, OUTPUT);
	g_moveFlag = HIGH;
	g_timeCount = 0;

	MsTimer2::set(1000, timer); // flash함수 1000ms마다 호출
	MsTimer2::start();

}

// the loop function runs over and over again until power down or reset
void loop() 
{
	// 컨트롤 => sleep 모드 추가.. : 타이머 이벤트로만 이동해도 됨.
	// 스위치 On 상태 : 조도에 상관없이 일정시간동안 전원 On, 움직임이 감지되면 계속 On
	if (g_switchFlag == HIGH && g_moveFlag == HIGH)	
	{
		digitalWrite(pinLamp, HIGH);
		if (g_timeCount > g_onDelayTime)	// 켜진상태로 움직임이 계속 없을때.. : 10분.
		{
			digitalWrite(pinLamp, LOW);
			g_moveFlag = LOW;
			g_timeCount = 0;
		}
	}
	// 스위치 Off 일 경우 : 움직임이 감지되고 조도가 기준 이하면 전원 On, 1분 지나면 전원 Off 
	else
	{
		uint illumValue = 0;
		if (g_moveFlag == HIGH)
		{
			illumValue = analogRead(pinIlluminance);
			if (illumValue < g_illumBase)		// 어두우면 전원 On
			{
				digitalWrite(pinLamp, HIGH); 
				g_timeCount = 0;
			}

			if (g_timeCount > g_offDelayTime)	// 움직임이 없으면 1분 후 전원 off
			{
				digitalWrite(pinLamp, LOW);
				g_moveFlag = LOW;
			}
		}
	}
	
	delay(100);
}

void timer()
{
	g_moveFlag++;
	if(g_moveFlag % 60 == 0)
		Serial.println("TIMER Min : " + g_moveFlag/60);
}

void motion()  // 움직임 감지
{
	g_moveFlag = HIGH;
	Serial.println("MOTION : " + g_moveFlag);
}

void onoff()  // 스위치
{
	g_timeCount = 0;			// 타임이벤트 리셋
	g_moveFlag == HIGH;

	g_switchFlag = pinSwitchInterrupt;
	Serial.println("SWITCH : " + g_switchFlag);
}