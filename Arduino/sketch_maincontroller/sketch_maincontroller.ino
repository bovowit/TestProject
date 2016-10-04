#include <mthread.h>

// Reserved Command : PC -> Controller
//  

// Reserved Command : Controller -> PC

// define Pin
#define PIN_GUNFIRE     0
#define PIN_GUNFIRE     1
#define PIN_GUNFIRE     2
#define PIN_GUNFIRE     3
#define PIN_GUNFIRE     4
#define PIN_GUNFIRE     5
#define PIN_GUNFIRE     6
#define PIN_GUNFIRE     7
#define PIN_GUNFIRE     8
#define PIN_GUNFIRE     9
#define PIN_GUNFIRE     10

////////////////////////////////////////////////////////////
String sToken = ";";
int serial_read_length = 0;         // 수신된 데이터 길이
int serial_max_packet = 100;
char temp_buff[100];                // 읽기 buff
int read_buff_pos = 0;              // 
String sReadData;

void SetPinMode()
{
    pinMode(PIN_GUNFIRE, INPUT);   // 총알의 발사 신호


    pinMode(5, OUTPUT);
  
}
void RunCommand(String sCommand)    // PC에서 읽어온 명령어 처리.
{
  if(!sCommand.compareTo("COMMND_START"))
      digitalWrite(1, HIGH);
  //else if(
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
    Serial.println("serial thread run");
    if (Serial.available() > 0)
    {
        if(!Serial.find(";"))
          return true;

        serial_read_length = Serial.readBytes(temp_buff, serial_max_packet);
        sReadData += temp_buff;       
        Serial.print("ReadData : "); Serial.println(temp_buff);
    }
    int iTockenIndex = 0;
    while(iTockenIndex = sReadData.indexOf(sToken))
    {
      if(iTockenIndex < 0)           // 명령이 완성되지 않았음.
        return true;
      
      String sCommand = sReadData.substring(0, iTockenIndex);
      RunCommand(sCommand);
      int iCommandLength = sCommand.length();
      sReadData.remove(0, iCommandLength);
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
    delay(1000);
}


