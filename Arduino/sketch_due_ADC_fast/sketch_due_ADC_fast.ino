int gSensitivity = 800;
unsigned long firsttime = 0;
int q0 = 0, q1 = 0, q2 = 0, q3 = 0;
int a0 = 0, a1 = 0, a2 = 0, a3 = 0;
unsigned long  t0 = 0, t1 = 0, t2 = 0, t3 = 0;

void setup() 
{
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

    for (int i = 0; i<1; i++) {
    while ((ADC->ADC_ISR & 0x80) == 0);
    while ((ADC->ADC_ISR & 0x80) == 0);// wait for 2 conversions
    while ((ADC->ADC_ISR & 0x80) == 0);// wait for 3 conversions
    while ((ADC->ADC_ISR & 0x80) == 0);// wait for 4 conversions
    a0 = ADC->ADC_CDR[7];              // read data on A0 pin
    a1 = ADC->ADC_CDR[6];              // read data on A1 pin
    a2 = ADC->ADC_CDR[5];              // read data on A2 pin
    a3 = ADC->ADC_CDR[4];              // read data on A3 pin
  }
  unsigned long tbase = micros();

  if(a0 > gSensitivity && t0 == 0)
  {
      q0 = a0;
      t0 = tbase;
      if(firsttime == 0)
        firsttime = tbase;
  }
  if(a1 > gSensitivity && t1 == 0)
  {
      q1 = a1;
      t1 = tbase;
      if(firsttime == 0)
        firsttime = tbase;
  }
  if(a2 > gSensitivity && t2 == 0)
  {
      q2 = a2;
      t2 = tbase;
  }
  if(a3 > gSensitivity && t3 == 0)
  {
      q3 = a3;
      t3 = tbase;
  }  

  if(q0 > 0 && q1 > 0 && q2 > 0 && q3 > 0)
  {
    //Serial.print("1000 pairs of conversions in "); Serial.print(t); Serial.println(" micros");
    Serial.print(firsttime); Serial.print(" : "); Serial.println(t1);
    Serial.print("A0 total:"); Serial.print(q0); Serial.print(" time : "); Serial.println(abs(t0 - firsttime));
    Serial.print("A1 total:"); Serial.print(q1); Serial.print(" time : "); Serial.println(abs(t1 - firsttime));
    Serial.print("A2 total:"); Serial.print(q2); Serial.print(" time : "); Serial.println(t2 - firsttime);
    Serial.print("A3 total:"); Serial.print(q3); Serial.print(" time : "); Serial.println(t3 - firsttime);
    delay(10);
    firsttime = 0;
   }

  if(abs(micros() - firsttime) > 300)
  {
    firsttime = 0;
    t0 = t1 = t2 = t3 = 0;
    q0 = q1 = q2 = q3 = 0;
    a0 = a1 = a2 = a3 = 0;
  }
}
