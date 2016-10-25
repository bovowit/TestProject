int sensitivity = 815;
int noise = sensitivity - 10;
int invalidtime = 400;
unsigned long stime = 0;
unsigned long temptime = 0;
long dtime = 0;
#define measurecnt 30
int sensor_value0[measurecnt] = {0, };
unsigned long sensing_time0[measurecnt] = {0, };
int sensor_value1[measurecnt] = {0, };
unsigned long sensing_time1[measurecnt] = {0, };
int r0 = 0;
int r1 = 0;
int _index = 0;
bool bStarted = false;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  
  int _t = analogRead(A2);  // 
  _t = analogRead(A5);
  
   Serial.println("Due multi vibration program start !!!!");
}

void clear()
{
  r0 = 0; r1 = 0;
  for(int i = 0; i < measurecnt; i++)
  {
    sensor_value0[i] = 0;
    sensing_time0[i] = 0;
    sensor_value1[i] = 0;
    sensing_time1[i] = 0;
  }
  bStarted = false;
}

void loop() {
   if (Serial.available() > 0)
    {
        char temp_buff[30] = {0, };          
        int length = Serial.readBytes(temp_buff, 30);
        String sReadData = temp_buff;
        sensitivity = sReadData.toInt();
        Serial.print("Tot Cmd Data : "); Serial.println(sReadData);
    }
  
  // put your main code here, to run repeatedly:
  int a0 = analogRead(A2);
  int a1 = analogRead(A5);
  temptime = micros();
  if(a0 > sensitivity || a1 > sensitivity)
  {
    clear();
    bStarted = true;

    if(a0 < noise)
      a0 = 0;
    sensor_value0[0] = a0;
    sensing_time0[0] = temptime;
    if(a1 < noise)
      a1 = 0;
    sensor_value1[0] = a1;
    sensing_time1[0] = temptime;
  }
  unsigned long deltatime = 0;
  while(bStarted)
  {
    r0++;
    if(r0 >= measurecnt)
    {
      break;
    }
    a0 = analogRead(A2);
    a1 = analogRead(A5);
    deltatime = micros();
    
    if(a0 < noise)
      a0 = 0;
    sensor_value0[r0] = a0;
    sensing_time0[r0] = deltatime - temptime;
    if(a1 < noise)
      a1 = 0;
    sensor_value1[r0] = a1;
    sensing_time1[r0] = deltatime - temptime;
  }
  
  if(bStarted == true)
  {
    bStarted = false;
    _index++;
    Serial.print("Measured Count ======= : "); Serial.println(_index);
    //long due_time = sensing_time0[0] - sensing_time1[0];
    //Serial.print("due time : "); Serial.println(due_time);

    //Serial.print("SEN0 : "); Serial.print(sensor_value0[0]); Serial.print(" - "); Serial.println(sensing_time0[0]);
    for(int i = 0; i < measurecnt ; i++)
    {
      Serial.print(sensor_value0[i]);
      if(sensor_value0[i] > noise)
      {
        Serial.print("-"); 
        Serial.print(sensing_time0[i]);
      }
      Serial.print(" | ");
    }
    Serial.println(" ");
    //Serial.print("SEN1 : "); Serial.print(sensor_value1[0]); Serial.print(" - "); Serial.println(sensing_time1[0]);
    for(int i = 0; i < measurecnt ; i++)
    {
     Serial.print(sensor_value1[i]); 
     if(sensor_value1[i] > noise)
      {
        Serial.print("-"); 
        Serial.print(sensing_time1[i]);
      }
     Serial.print(" | ");
    }
    Serial.println(" ");
    clear();
    delayMicroseconds(1000000);
  }
  if(temptime - sensing_time0[0] > invalidtime || temptime - sensing_time1[0] > invalidtime)
  {
    clear();
  }

}
