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
      Serial.println("echo test : [CMD]00;");  break;
    case 10:   // 공급모터 OFF
      digitalWrite(1, LOW);     break;
    case 11:   // 공급모터 ON
      digitalWrite(1, HIGH);    break;
    case 20:  // 에어모터 = 방아쇠제어
      digitalWrite(2, LOW);     break;
    case 21:
      digitalWrite(2, HIGH);    break;
    case 30:  // 총구 카메라 
      digitalWrite(3, LOW);     break;
    case 31:
      digitalWrite(3, HIGH);    break;
    //case 40:  // 4번핀 = 지폐인식 입력신호
    //case 50:  // 5번핀 = 시작버튼 눌림 입력신호
    //case 60:  // 6번핀 = 총구의 광센서 입력신호
    case 70:  // 시작버튼 소등 
      digitalWrite(7, LOW);     break;
    case 71:  // 시작버튼 깜박임
      g_bFlickerStartBtn = true;  // true인 동안 깜박임 처리
      break;        
    //case 80:  // 8번핀 = 총의 기능1버튼  입력신호
    //case 90:  // 9번핀 = BB버튼 입력 신호
    case 100:   // 취소버튼 소등
      digitalWrite(10, LOW);     break;      
    case 101:  // 취소버튼 깜박임
      g_bFlickerCancelBtn = true;  // true인 동안 깜박임 처리
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
 
bool SensorThread::loop()  // 센서로부터 데이터 수집 
{
    //int itemp = digitalRead(PIN_CRD_ADD_MONEY);
    //Serial.print("4Pin Input Val : "); Serial.println(itemp);
    
    if(digitalRead(PIN_CRD_ADD_MONEY) == HIGH)
        SendCommandToPc(CRD_ADD_MONEY);
    if(digitalRead(PIN_BTN_START) == HIGH)
    {
        SendCommandToPc(BTN_START);
        g_bFlickerStartBtn = false;
    }
    if(digitalRead(PIN_GUN_SHOT) == HIGH)
        SendCommandToPc(GUN_SHOT);
    if(digitalRead(PIN_BTN_GUN_F1) == HIGH)
        SendCommandToPc(BTN_GUN_F1);
    if(digitalRead(PIN_BTN_AMMSUPPLY) == HIGH)
        SendCommandToPc(BTN_AMMSUPPLY);
    if(digitalRead(PIN_BTN_CANCEL) == HIGH)
     {
        SendCommandToPc(BTN_CANCEL);
        g_bFlickerCancelBtn = false;
    }
    sleep_milli(10);        
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


