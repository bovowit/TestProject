int value = 0;
int highretrycnt = 0;
int lowretrycnt = 0;
int arrValue[10000] = { 0, };
unsigned long sTime = 0;
long dTime = 0;
bool bBlast = false;

int sensitivity = 50;

void setup() 
{
  Serial.begin(9600);

  Serial.print("Start analize Blast Wave !!  : ");
}

void loop()
{
   if (Serial.available() > 0)
    {
        char temp_buff[30] = {0, };          
        int length = Serial.readBytes(temp_buff, 30);
        String sReadData = temp_buff;
        sensitivity = sReadData.toInt();
        Serial.print("Tot Cmd Data : "); Serial.println(sReadData);
    }

  
  highretrycnt = 0;
  sTime = 0;
  //bBlast = false;
  for (int i = 0; i < 1000; i++)
    arrValue[i] = 0;

  int tempVal = analogRead(A0);
  if (tempVal >= sensitivity)
  {
    //bBlast = true;
    sTime = micros();
    lowretrycnt = 0;
    while (highretrycnt < 1000 -1)
    {
      tempVal = analogRead(A0);
      arrValue[highretrycnt] = tempVal;
      highretrycnt++;
      if (tempVal < sensitivity)
        lowretrycnt++;
      else
        lowretrycnt = 0;

      if (lowretrycnt >= 10)
      {
        //bBlast = false;
        dTime = micros() - sTime;
        break;
      }
    }

    if (lowretrycnt < 10)
      Serial.println("fail anlized wave.......................");
    Serial.print(micros()); Serial.println("===micros===");
    Serial.print("WAVE in count : "); Serial.println(highretrycnt);
    Serial.print("due time : "); Serial.println(dTime);
    for (int i = 0; i < highretrycnt; i++)
    {
      if (i % 20 == 19)
      {
        Serial.println(arrValue[i]);
        Serial.print("|");
      }
      Serial.print("|");
      Serial.print(arrValue[i]);
    }
    Serial.println("|");
  }
  //delay(3000);
}
