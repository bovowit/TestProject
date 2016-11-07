int gSensitivity = 800;
int a0 = 0, a1 = 0, a2 = 0, a3 = 0;
int aVal[4][100];
unsigned long firsttime = 0;

void setup() 
{
  empty();
  Serial.begin(9600);
  int t = analogRead(0); t = analogRead(1); t = analogRead(2); t = analogRead(3);

  ADC->ADC_MR |= 0x80; // these lines set free running mode on adc 7 and adc 6 (pin A0 and A1 - see Due Pinout Diagram thread)
  ADC->ADC_CR = 2;
  //ADC->ADC_CHER = 0xC0; // this is (1<<7) | (1<<6) for adc 7 and adc 6     
  ADC->ADC_CHER=0xF0; // this is (1<<7) | (1<<6) for adc 7 and adc 4                                     
}

void loop() 
{
  if (Serial.available() > 0)
  {
    char temp_buff[30] = { 0, };
    int length = Serial.readBytes(temp_buff, 30);
    String sReadData = temp_buff;
    gSensitivity = sReadData.toInt();
    Serial.print("Sensitivity value input : "); Serial.println(sReadData);
  }

    for (int i = 0; i<99; i++) {
      while ((ADC->ADC_ISR & 0x80) == 0);
      while ((ADC->ADC_ISR & 0x80) == 0);// wait for 2 conversions
      while ((ADC->ADC_ISR & 0x80) == 0);// wait for 3 conversions
      while ((ADC->ADC_ISR & 0x80) == 0);// wait for 4 conversions
      aVal[0][i] = ADC->ADC_CDR[7];              // read data on A0 pin
      aVal[1][i] = ADC->ADC_CDR[6];              // read data on A1 pin
      if((aVal[0][0] > gSensitivity || aVal[1][0] > gSensitivity) && firsttime == 0)
          firsttime = micros();
      else
          break;
      aVal[2][i] = ADC->ADC_CDR[5];              // read data on A2 pin
      aVal[3][i] = ADC->ADC_CDR[4];              // read data on A3 pin
    }
  unsigned long tbase = micros();
  if(aVal[0][0] > gSensitivity || aVal[1][0] > gSensitivity)
  {
        Serial.println("--------------- micros");
       Serial.print("A0 : "); 
     for(int i = 0; i < 100; i++)
    {
        Serial.print(aVal[0][i]); Serial.print("|");
     }
      Serial.println("+");
    Serial.print("A1 : ");
    for(int i = 0; i < 100; i++)
    {
       Serial.print(aVal[1][i]);  Serial.print("|");
    }
    delay(10);
    Serial.println("+");

   }

  if(abs(micros() - firsttime) > 300)
  {
       for(int i = 0; i < 100; i++)
     {
      aVal[0][i] = 0;
      aVal[1][i] = 0;
     }  
  }
}
