// 프로토콜 규칙
//PC에서 "[CMD]10;" 명령입력시, Amm_Supply_Off 명령이 수행됨
//       "[CMD]11;" 명령입력시 , Amm_Supply_ON 명령이 수행됨 
// 숫자의 앞은 핀 넘버, 뒷자리 0/1 은 off/On 을 의미함.
// 아두이노에서 PC로도 동일하게 명령어 전송됨.

// 개발 참고
// 세개의 thread (sub routine) 이 동작함. PC로부터 명령어 읽어 처리/센서 값 읽기/버튼의 점멸 신호 등.

#include <mthread.h>

// Reserved Command : PC -> Controller 
enum COMMAND {
  AMM_SUPPLY_OFF  = 10,
  AMM_SUPPLY_ON   = 11,
  TRIGGER_OFF     = 20,
  TRIGGER_ON      = 21,
  GUN_CAMSLEEP_OFF= 30,
  GUN_CAMSLEEP_ON = 31,
  BTN_START_OFF   = 70,
  BTN_START_ON    = 71,
  BTN_CANCEL_OFF  = 100,
  BTN_CANCEL_ON   = 101,
};

// Reserved Command : Sensor -> Controller => PC
#define CRD_ADD_MONEY   "41"
#define BTN_START       "51"
#define GUN_SHOT        "61"
#define BTN_GUN_F1      "81"
#define BTN_AMMSUPPLY   "91"
#define BTN_CANCEL      "111"


// define Pin (INPUT signal)
#define PIN_AMM_SUPPLY        1
#define PIN_TRIGGER           2
#define PIN_GUN_CAMSLEEP      3

#define PIN_CRD_ADD_MONEY     4
#define PIN_BTN_START         5
#define PIN_GUN_SHOT          6
#define PIN_BTN_GUN_F1        8
#define PIN_BTN_AMMSUPPLY     9
#define PIN_BTN_CANCEL        11

// define Pin (Output signal)
#define PIN_START_LIGHT       7
#define PIN_CANCEL_LIGHT      10

////////////////////////////////////////////////////////////
String sCmdBegin = "[CMD]";
int iCmdBegin = 5;
String sCmdEnd = ";";
int iCmdEnd = 1;
int serial_read_length = 0;         // 수신된 데이터 길이
int serial_max_packet = 90;
String sReadData;
bool g_bFlickerStartBtn = false;    // true 이면 스타트 버튼 점멸
bool g_bFlickerCancelBtn = false;   // true 이면 취소 버튼 점멸

void SetPinMode()
{
    pinMode(PIN_CRD_ADD_MONEY, INPUT);    // 지폐 투입
    pinMode(PIN_BTN_START, INPUT);        // 시작버튼
    pinMode(PIN_GUN_SHOT, INPUT);         // 총알의 발사 신호
    pinMode(PIN_BTN_GUN_F1, INPUT);       // 총기의 기능버튼
    pinMode(PIN_BTN_AMMSUPPLY, INPUT);    // 
    pinMode(PIN_BTN_CANCEL, INPUT);       // 취소버튼 

    pinMode(PIN_START_LIGHT, OUTPUT);
    pinMode(PIN_CANCEL_LIGHT, OUTPUT);
}

void RunCommand(String sCommand)    // PC에서 읽어온 명령어 처리.
{
  Serial.println(sCommand);
  switch(sCommand.toInt())
  {
    case 00:
      Serial.println("echo test : [CMD]00;");  
      break;
    case 10:   // 공급모터 OFF
      digitalWrite(PIN_AMM_SUPPLY, LOW);     
      break;
    case 11:   // 공급모터 ON
      digitalWrite(PIN_AMM_SUPPLY, HIGH);    
      break;
    case 20:  // 에어모터 = 방아쇠제어
      digitalWrite(PIN_TRIGGER, LOW);     
      break;
    case 21:
      digitalWrite(PIN_TRIGGER, HIGH);    
      break;
    case 30:  // 총구 카메라 
      digitalWrite(PIN_GUN_CAMSLEEP, LOW);     
      break;
    case 31:
      digitalWrite(PIN_GUN_CAMSLEEP, HIGH);    
      break;
    //case 40:  // 4번핀 = 지폐인식 입력신호
    //case 50:  // 5번핀 = 시작버튼 눌림 입력신호
    //case 60:  // 6번핀 = 총구의 광센서 입력신호
    case 70:  // 시작버튼 소등 
      digitalWrite(PIN_START_LIGHT, LOW);     
      break;
    case 71:  // 시작버튼 깜박임
      g_bFlickerStartBtn = true;  // true인 동안 깜박임 처리
      break;        
    //case 80:  // 8번핀 = 총의 기능1버튼  입력신호
    //case 90:  // 9번핀 = BB버튼 입력 신호
    case 100:   // 취소버튼 소등
      digitalWrite(PIN_CANCEL_LIGHT, LOW);     
      break;      
    case 101:  // 취소버튼 깜박임
      g_bFlickerCancelBtn = true;  // true인 동안 깜박임 처리
      break;
    //case 110:  //11번핀 = 취소버튼 눌림 입력신호   
  }
}

void SendCommandToPc(String sCommandID)
{
    String sCmd = sCmdBegin + sCommandID + sCmdEnd;
    Serial.println(sCmd);
}

void buff_reset()
{
    sReadData.remove(0); 
}

class SensorThread : public Thread
{
public:
    SensorThread(int id);
protected:
    bool loop();
private:
    int id;
};
 
SensorThread::SensorThread(int id)
{
    this->id = id;
}

int g_iCrdAddMoney = 0; 
int g_iBtnStart = 0;
int g_iGunShot = 0;
int g_iBtnGunF1 = 0;
int g_iBtnAmmSupply = 0;
int g_iBtnCalcel = 0;
int g_BaseDelayCount = 200; // 한번 신호 수신 후 milli second 동안 신호 무시
bool SensorThread::loop()   // 센서로부터 데이터 수집, 수집주기가 빨라서 계속 신호 감지될 가능성 처리.
{
    if(digitalRead(PIN_CRD_ADD_MONEY) == HIGH || g_iCrdAddMoney > 0)
    {
      if(g_iCrdAddMoney == 0)
        SendCommandToPc(CRD_ADD_MONEY);
      g_iCrdAddMoney++;
      if(g_iCrdAddMoney > g_BaseDelayCount)
        g_iCrdAddMoney = 0;
    }
    if(digitalRead(PIN_BTN_START) == HIGH || g_iBtnStart > 0)
    {
        g_bFlickerStartBtn = false;
        if(g_iBtnStart == 0)
          SendCommandToPc(BTN_START);
        g_iBtnStart++;
        if(g_iBtnStart > g_BaseDelayCount)
          g_iBtnStart = 0;
    }
    if(digitalRead(PIN_GUN_SHOT) == HIGH || g_iGunShot > 0)
    {
        if(g_iGunShot == 0)
          SendCommandToPc(GUN_SHOT);
        g_iGunShot++;
        if(g_iGunShot > g_BaseDelayCount)
          g_iGunShot = 0;
    }
    if(digitalRead(PIN_BTN_GUN_F1) == HIGH || g_iBtnGunF1 > 0)
    {
        if(g_iBtnGunF1 == 0)
          SendCommandToPc(BTN_GUN_F1);
        g_iBtnGunF1++;
        if(g_iBtnGunF1 > g_BaseDelayCount)
          g_iBtnGunF1 = 0;
    }
    if(digitalRead(PIN_BTN_AMMSUPPLY) == HIGH || g_iBtnAmmSupply > 0)
    {
        if(g_iBtnAmmSupply == 0)
          SendCommandToPc(BTN_AMMSUPPLY);
        g_iBtnAmmSupply++;  
        if(g_iBtnAmmSupply > g_BaseDelayCount)
          g_iBtnAmmSupply = 0;
    }
    if(digitalRead(PIN_BTN_CANCEL) == HIGH || g_iBtnCalcel > 0)
     {
        g_bFlickerCancelBtn = false;
        if(g_iBtnCalcel == 0)
          SendCommandToPc(BTN_CANCEL);
        g_iBtnCalcel++;
        if(g_iBtnCalcel > g_BaseDelayCount)
          g_iBtnCalcel = 0;
    }
    sleep_milli(1);
    return true;
}

class FlikerThread : public Thread
{
public:
    FlikerThread(int id);
protected:
    bool loop();
private:
    int id;
};
 
FlikerThread::FlikerThread(int id)
{
    this->id = id;
}
 
bool FlikerThread::loop()  // 버튼 깜박임 처리
{
  int iSleepTime = 1000;
  
  if(g_bFlickerStartBtn)
  {
      digitalWrite(PIN_START_LIGHT, HIGH);
      sleep_milli(iSleepTime);
      digitalWrite(PIN_START_LIGHT, LOW);
  }
   if(g_bFlickerCancelBtn)
  {
      digitalWrite(PIN_CANCEL_LIGHT, HIGH);
      sleep_milli(iSleepTime);
      digitalWrite(PIN_CANCEL_LIGHT, LOW);   
  } 
  sleep_milli(iSleepTime);
  return true;
}

class SerialReadThread : public Thread
{
public:
    SerialReadThread(int id);
protected:
    bool loop();
private:
    int id;
};
 
SerialReadThread::SerialReadThread(int id)
{
}
 
bool SerialReadThread::loop() // PC의 명령 수신 처리
{
    if (Serial.available() > 0)
    {
        //if(!Serial.find(sCmdBegin[0]))
        //  return false;
        char temp_buff[30] = {0, };          
        serial_read_length = Serial.readBytes(temp_buff, serial_max_packet);
        sReadData += temp_buff;
        if(sReadData.length() > 100)    // 100byte 이상 명령어 처리가 지연되면 모두 삭제.
            sReadData.remove(0);       
        Serial.print("Tot Cmd Data : "); Serial.println(sReadData);
    }
    
    int iCmdEndIndex = sReadData.indexOf(sCmdEnd);
    while(iCmdEndIndex > 0)
    {
      if(iCmdEndIndex < 7 || sCmdBegin.compareTo(sReadData.substring(0, iCmdBegin)) != 0)    // 5보다 작은것은 비 정상 명령어로 처리
      {
        sReadData.remove(0, iCmdEndIndex + iCmdEnd);
        return true;
      }
      String sCommand = sReadData.substring(iCmdBegin, iCmdEndIndex);
      RunCommand(sCommand);
      int iCommandLength = sCommand.length();
      sReadData.remove(0, iCmdBegin + iCommandLength + iCmdEnd);
      
      iCmdEndIndex = sReadData.indexOf(sCmdEnd);
    }
    return true;
} 
void setup()
{
    Serial.begin(9600);
    SetPinMode();   
    main_thread_list->add_thread(new SerialReadThread(1));
    main_thread_list->add_thread(new FlikerThread(2));   
    main_thread_list->add_thread(new SensorThread(3));

    Serial.println("Running Main Controller...");
    Serial.setTimeout(100);
    delay(1000);
}


