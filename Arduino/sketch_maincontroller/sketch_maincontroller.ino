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
// define Pin (INPUT signal)
#define PIN_CRD_ADD_MONEY     4
#define PIN_BTN_START         5
#define PIN_GUN_SHOT          6
#define PIN_BTN_GUN_F1        8
#define PIN_BTN_AMMSUPPLY     9
#define PIN_BTN_CANCEL        11


////////////////////////////////////////////////////////////
String sCmdBegin = "[CMD]";
int iCmdBegin = 5;
String sCmdEnd = ";";
int iCmdEnd = 1;
int serial_read_length = 0;         // 수신된 데이터 길이
int serial_max_packet = 90;
String sReadData;

void SetPinMode()
{
    pinMode(PIN_CRD_ADD_MONEY, INPUT);   // 총알의 발사 신호


    pinMode(5, OUTPUT);
  
}
void RunCommand(String sCommand)    // PC에서 읽어온 명령어 처리.
{
  Serial.println(sCommand);
  switch(sCommand.toInt())
  {
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
      digitalWrite(7, HIGH);     break;         //====================> 깜박임 처리 필요
    //case 80:  // 8번핀 = 총의 기능1버튼  입력신호
    //case 90:  // 9번핀 = BB버튼 입력 신호
    case 100:   // 취소버튼 소등
      digitalWrite(10, LOW);     break;      
    case 101:  // 취소버튼 깜박임
      digitalWrite(10, HIGH);     break;      //====================> 깜박임 처리 필요
    //case 110:  //11번핀 = 취소버튼 눌림 입력신호   
  }
     
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
 
bool SensorThread::loop()  // 
{
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
 
bool SerialReadThread::loop()
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
    for(int i = 0; i < 1; i++)
        main_thread_list->add_thread(new SensorThread(i));

    main_thread_list->add_thread(new SerialReadThread(1));

    Serial.println("Running Main Controller...");
    Serial.setTimeout(100);
    delay(1000);
}


