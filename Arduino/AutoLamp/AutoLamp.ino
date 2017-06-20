// 동작감지&시간체크 -> 조도체크&OnOff스위치체크 -> 전원제어
// 동작감시 인터럽트
#include "MsTimer2.h"

// 기준값
const unsigned int g_illumBase = 500;				// 기준 조도값, 어두운 밤 기준으로 측정후 셋팅.
const unsigned int g_delayTimeSwitchOn = 10;		// 스위치 On일때, 움직임 없어 전원 Off 할때까지 지속시간
const unsigned int g_delayTimeSwitchOff = 3;		// 스위치 Off할때 Lamp Off 대기시간
const unsigned int g_delayTimeMotion = 6;			// 스위치 Off일때, 움직임 감지 후 전원 On 지속시간

// 핀 정의
int pinMotionInterrupt = 2;		// 동작감시
int pinSwitchInterrupt = 3;		// On/Off 스위치
int pinIlluminance = A0;		// 조도센서
int pinLamp = 7;				// 램프제어 릴레이

// 상태
volatile byte g_moveFlag = LOW;
volatile byte g_switchFlag = LOW;
volatile unsigned long g_timeCountSwitch = 0;
volatile unsigned long g_timeCountMotion = 0;
bool lampToggle = false;

// the setup function runs once when you press reset or power the board
void setup() 
{
	Serial.begin(9600);
	int illumValue = analogRead(pinIlluminance);
	Serial.print("Start Setup... "); Serial.println(illumValue);

	pinMode(pinMotionInterrupt, INPUT_PULLUP);
	attachInterrupt(digitalPinToInterrupt(pinMotionInterrupt), motion, RISING);// CHANGE);
	pinMode(pinSwitchInterrupt, INPUT_PULLUP);
	attachInterrupt(digitalPinToInterrupt(pinSwitchInterrupt), onoff, CHANGE);
	
	pinMode(pinLamp, OUTPUT);

	g_moveFlag = HIGH;
	g_switchFlag = !digitalRead(pinSwitchInterrupt);

	g_timeCountSwitch = 0;
	g_timeCountMotion = 0;

	MsTimer2::set(1000, timer); // flash함수 1000ms마다 호출
	MsTimer2::start();

}

// the loop function runs over and over again until power down or reset
void loop() 
{
	// 컨트롤 => sleep 모드 추가.. : 타이머 이벤트로만 이동해도 됨.
	// 스위치 On 상태 : 조도에 상관없이 일정시간동안 전원 On, 움직임이 감지되면 계속 On
	if (g_switchFlag == HIGH)	
	{
		if (g_timeCountSwitch > g_delayTimeSwitchOn && lampToggle == true)	// 켜진상태로 움직임이 계속 없을때.. : 10분.
		{
			digitalWrite(pinLamp, LOW);
			g_moveFlag = LOW;
			lampToggle = false;
		}
	}
	// 스위치 Off 일 경우 : 움직임이 감지되고 조도가 기준 이하면 전원 On, 1분 지나면 전원 Off 
	else
	{
		if (g_timeCountSwitch > g_delayTimeSwitchOff && lampToggle == true)	// 스위치 끄면 지정된 시간 지난후 일단 Lamp Off.
		{
			digitalWrite(pinLamp, LOW);
			g_moveFlag = LOW;
			lampToggle = false;
		}

		if (g_moveFlag == HIGH && g_timeCountSwitch > g_delayTimeSwitchOff)
		{
			unsigned int illumValue = 0;
			illumValue = analogRead(pinIlluminance);
			// Serial.print("Check illum... "); Serial.println(illumValue);      
			if (illumValue < g_illumBase)		// 어두우면 전원 On
			{
				if (lampToggle == false)
				{
					digitalWrite(pinLamp, HIGH);
					lampToggle = true;
					Serial.println("Lamp On.........");
				}
			}
			g_timeCountMotion = 0;
		}

		if (g_timeCountMotion > g_delayTimeMotion)	// 움직임이 없으면 1분 후 전원 off
		{
			digitalWrite(pinLamp, LOW);
			lampToggle = false;
			g_moveFlag = LOW;
			Serial.println("Lamp Off.........");
		}
	}
	
	delay(100);
}

void timer()
{
	g_timeCountSwitch++;
	g_timeCountMotion++;
	//if(g_timeCount % 60 == 1)
	//{
	//	Serial.print(g_timeCountSwitch);
	//	Serial.print(" : TIMER Switch : "); 
	//	Serial.print(g_timeCountMotion);
	//	Serial.println(" : TIMER Motion : ");
	//}
}

void motion()  // 움직임 감지 High 일때만 타이머 리셋
{
	g_moveFlag = digitalRead(pinMotionInterrupt);
	if (g_moveFlag == HIGH)
	{
		g_timeCountMotion = 0;
		//g_timeCountSwitch = 0;
	}
	Serial.print("MOTION : "); Serial.println(g_moveFlag);
}

void onoff()  // 스위치 : 스위치를 직접 On 할때 무조건 Lamp On, 끌때는 지연 처리. 스위치 Off는 타이머만 리셋.
{
	g_timeCountSwitch = 0;
	g_switchFlag = !digitalRead(pinSwitchInterrupt);
	if (g_switchFlag && lampToggle == false)			
	{
		digitalWrite(pinLamp, HIGH);
		lampToggle = true;
	}
	Serial.print("SWITCH : "); Serial.println(g_switchFlag);
	delay(1);
}
