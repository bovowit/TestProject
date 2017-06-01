// BlueTooth 연결, 통신모듈연결후 시리얼 입력 & 응답확인
// AT -> Ok
// AT+NAMENOVAARDU01 -> OKsetName
// AT+PIN3333 -> OKsetPin

#include <DS1302.h> // RTS 라이브러리
#include <SoftwareSerial.h> //시리얼 통신 라이브러리 호출

// RTS ..........................
#define SCK_PIN 10
#define IO_PIN 9
#define RST_PIN 8
DS1302 rtc(RST_PIN, IO_PIN, SCK_PIN); // Initialise the DS1302 library 
String g_curTime;
String g_curDate;

// buletooth....................
int blueTx=2;   //Tx (보내는핀 설정)
int blueRx=3;   //Rx (받는핀 설정)
SoftwareSerial mySerial(blueTx, blueRx);  //시리얼 통신을 위한 객체선언
String myString=""; //받는 문자열
 
void setup() {
  Serial.begin(9600);   //시리얼모니터 
  mySerial.begin(9600); //블루투스 시리얼 개방
 
  RTSSetup();           // 한번만 호출후 다시 업로드 할것. => bluetooth TIME Command 구현
}

void RTSSetup()
{
  /* Clear the 1302's halt flag */
  rtc.halt(false);
  /* And disable write protection */
  rtc.writeProtect(false);
  /* Initialise the serial port */
  Serial.begin(9600);
  
  /* Set the time and date */
  rtc.setDOW(THURSDAY);
  rtc.setTime(21,48,0);
  rtc.setDate(1,6,2017); 
}

void GetTime()
{
  String _week(rtc.getDOWStr());
  String _date(rtc.getDateStr());
  g_curDate = _week + _date;
  g_curTime = rtc.getTimeStr();
}

void loop() {
  while(mySerial.available())  //mySerial에 전송된 값이 있으면
  {
    char myChar = (char)mySerial.read();  //mySerial int 값을 char 형식으로 변환
    myString+=myChar;   //수신되는 문자를 myString에 모두 붙임 (1바이트씩 전송되는 것을 연결)
    delay(5);           //수신 문자열 끊김 방지
  }
  if(!myString.equals(""))  //myString 값이 있다면
  {
    Serial.println("input value: " + myString + "Time : " + g_curTime); //시리얼모니터에 myString값 출력
    myString="";  //myString 변수값 초기화
  }
}

