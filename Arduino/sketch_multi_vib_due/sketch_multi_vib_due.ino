#include <mthread.h>
int sensor_count = 2;
int sensitivity = 2;
long stime = 0;
long dtime = 0;
int sensor_value[] = {0, 0, 0, 0};
long sensing_time[] = {0, 0, 0, 0};
int retry = 0;

const unsigned char PS_16 = (1 << ADPS2);
const unsigned char PS_32 = (1 << ADPS2) | (1 << ADPS0);
const unsigned char PS_64 = (1 << ADPS2) | (1 << ADPS1);
const unsigned char PS_128 = (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

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
 
bool SensorThread::loop()  // sensor_count 만큼의 센서에서 값을 읽기만 함
{
    // Die if requested:
    if(kill_flag)
        return false;
    int tempvalue = analogRead(id);
    //if(sensor_value[id] == 0 && tempvalue > sensitivity) // 값이 들어오면 무조건 감지
    if(sensor_value[id] < tempvalue && tempvalue > sensitivity)
    {
      sensor_value[id] = tempvalue;
      sensing_time[id] = micros();
    }

    //sleep_micro(1000);
    return true;
}

class ManagerThread : public Thread
{
public:
    ManagerThread(int id);
protected:
    bool loop();
private:
    int id;
};
 
ManagerThread::ManagerThread(int id)
{
}
 
bool ManagerThread::loop()
{
    if((sensor_value[0] > 0 || sensor_value[1]) > 0 && stime == 0)
      stime = micros();
    if(stime > 0)
      dtime = micros() - stime;
    if(dtime > 1500 || dtime < 0)     // 측정값 리셋
    {
      for(int i = 0; i < sensor_count; i++)
      {
        sensor_value[i] = 0;
        sensing_time[i] = 0;
        stime = dtime = 0;
      }
    }

    if(sensor_value[0] > 0 && sensor_value[1] > 0)
    {
      retry++;  Serial.print(retry);
      Serial.print("  [V0] "); Serial.print(sensor_value[0]); 
      Serial.print("  [V1] "); Serial.println(sensor_value[1]); 
      long deltaTime = sensing_time[0] - sensing_time[1];
      Serial.print("Delta Time : "); Serial.println(deltaTime);
      
      for(int i = 0; i < sensor_count; i++)
      {
        sensor_value[i] = 0;
        sensing_time[i] = 0;
        stime = dtime = 0;
      }      

      delay(30);
      //sleep_milli(100);
    }
    //sleep_micro(1000000);
    return true;
} 
void setup()
{
    // Create five threads and add them to the main ThreadList:
    for(int i = 0; i < 2; i++)
        main_thread_list->add_thread(new SensorThread(i));

     main_thread_list->add_thread(new ManagerThread(1));

    // set up the ADC
    ADCSRA &= ~PS_128;  // remove bits set by Arduino library
  
    // you can choose a prescaler from above.
    // PS_16, PS_32, PS_64 or PS_128
    ADCSRA |= PS_16;    // set our own prescaler to 64 
 
    // Initialize the serial connection:
    Serial.begin(9600);
    //Serial.begin(250000);
    delay(1000);
}
