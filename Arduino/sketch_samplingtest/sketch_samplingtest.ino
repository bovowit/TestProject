#define measurecnt 100
int sensitivity = 820;
int ax = 0, ay = 0, az = 0;
int arrx[measurecnt];
int arry[measurecnt];
int arrz[measurecnt];
bool bstarted = false;

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
  
 
  bstarted = false;
  ax = analogRead(A2);
  ay = 0;//analogRead(A4);
  az = analogRead(A5);
  arrx[0] = ax;
  arry[0] = ay;
  arrz[0] = az;
  if(ax > sensitivity || ay > sensitivity || az > sensitivity)
  {
    bstarted = true;
    for(int i = 1; i < measurecnt; i++)
    {
      ax = analogRead(A2);
      ay = 0;//analogRead(A4);
      az = analogRead(A5);
      arrx[i] = ax;
      arry[i] = ay;
      arrz[i] = az;
    }
    
  }

  if(bstarted == true)
  {
    Serial.print("Measured Count ======================== : "); Serial.println("");
    for(int i = 0; i < measurecnt ; i++)
    {
      Serial.print(arrx[i]);
      Serial.print(" | ");
    }
    Serial.println(" ");
    
    for(int i = 0; i < measurecnt ; i++)
    {
      Serial.print(arry[i]);
      Serial.print(" | ");
    }
    Serial.println(" ");
                                                                 
    for(int i = 0; i < measurecnt ; i++)
    {
      Serial.print(arrz[i]);
      Serial.print(" | ");
    }
    Serial.println(" ");     

    for(int i = 0; i < measurecnt ; i++)
    {
      arrx[i] = 0;
      arry[i] = 0;
      arrz[i] = 0;
    }
    delay(1000); 
    analogRead(A0);
    analogRead(A1);
    analogRead(A2);
    analogRead(A3);
    analogRead(A4);
    analogRead(A5);
  }
    

}
