#define MAX_ADC_RESOLUTION 10

#include <mthread.h>
int sensor_count = 2;
int sensitivity = 90;
long stime = 0;
long dtime = 0;
int sensor_value[4] = {0, };
long sensing_time[4] = {0, };
int retry = 0;


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

    if(sensor_value[0] > sensitivity && sensor_value[1] > sensitivity)
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

      delay(1000);
      //sleep_milli(100);
    }
    //sleep_micro(1000000);
    return true;
} 
void setup()
{
     analogReadResolution(MAX_ADC_RESOLUTION); //
    
     //adc_init(ADC, SystemCoreClock, ADC_FREQ_MAX*2, 1); //~313000 samples per second, A/D readings kaput
      //adc_init(ADC, SystemCoreClock, ADC_FREQ_MAX*2, 2); //~288 000 samples per second, A/D readings now have about 10% deviation
      ////adc_init(ADC, SystemCoreClock, ADC_FREQ_MAX*2, 3); //~267000 samples per second, A/D readings OK, the best compromise I got
      //adc_init(ADC, SystemCoreClock, ADC_FREQ_MAX*2, 4); //~220000 samples per second, A/D readings OK
      //adc_init(ADC, SystemCoreClock, ADC_FREQ_MAX*2, ADC_STARTUP_NORM ); //~65000 samples per second,A/D readings OK 
      //adc_init(ADC, SystemCoreClock, ADC_FREQ_MAX*2, ADC_STARTUP_FAST ); //~47000 samples per second, from the name, I was tempted to think this would perform better. But I know nothing.
     //pinMode(ADC0, INPUT);
     //pinMode(1, INPUT);
    // pinMode(2, INPUT);
     
    // Create five threads and add them to the main ThreadList:
    for(int i = 0; i < 2; i++)
        main_thread_list->add_thread(new SensorThread(i));

     main_thread_list->add_thread(new ManagerThread(1));

     // Initialize the serial connection:
    Serial.begin(9600);
    //Serial.begin(250000);
    Serial.println("Due multi vibration program start !!!!");
    delay(100);
}
