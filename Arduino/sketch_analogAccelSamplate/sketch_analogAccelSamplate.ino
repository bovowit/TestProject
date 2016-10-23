int azv[500];
int sensitivity = 63;
int maxval = 500;
unsigned long stime = 0; 

void setup() {
  // put your setup code here, to run once:
    Serial.begin(115200);
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
  int zv = analogRead(A2);
  //int zv = map(z, 0, 4096, 0, 100);
  if(zv > sensitivity)
  {
    stime = micros();    
    azv[0] = zv;
    for(int i = 1 ; i < maxval; i++)
    {
      zv = analogRead(A1);
      zv = analogRead(A2);
      //zv = map(z, 0, 1023, 0, 100);
      azv[i] = zv;
      
    }
  }

  if( (azv[0]) > sensitivity)
  {
    unsigned long dtime = micros() - stime;
    Serial.print("Z Axis : "); Serial.println(dtime);
    for(int i = 0; i < maxval; i++)
    {
      Serial.print(azv[i]);
      Serial.print(" | ");
      if(i%50 == 0)
        Serial.println("");
    }
    Serial.println("");
    for(int i = 0; i < maxval; i++)
      azv[i] = 0;
  }
}
