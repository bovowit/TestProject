void setup() 
{
  Serial.begin(9600);
  int t = analogRead(0);

  ADC->ADC_MR |= 0x80; // these lines set free running mode on adc 7 and adc 6 (pin A0 and A1 - see Due Pinout Diagram thread)
  ADC->ADC_CR = 2;
  //ADC->ADC_CHER = 0xC0; // this is (1<<7) | (1<<6) for adc 7 and adc 6     
  ADC->ADC_CHER=0xF0; // this is (1<<7) | (1<<6) for adc 7 and adc 6                                     
                                
}

void loop() {
  int t = micros();
  int q0 = 0, q1 = 0, q2 = 0, q3 = 0;
  int a0, a1, a2, a3;
  for (int i = 0; i<1; i++) {
    while ((ADC->ADC_ISR & 0x80) == 0);
    while ((ADC->ADC_ISR & 0x80) == 0);// wait for 2 conversions
    while ((ADC->ADC_ISR & 0x80) == 0);// wait for 3 conversions
    while ((ADC->ADC_ISR & 0x80) == 0);// wait for 4 conversions
    a0 = ADC->ADC_CDR[7];              // read data on A0 pin
    a1 = ADC->ADC_CDR[6];              // read data on A1 pin
    a2 = ADC->ADC_CDR[5];              // read data on A2 pin
    a3 = ADC->ADC_CDR[4];              // read data on A3 pin
    q0 += a0;
    q1 += a1;
    q2 += a2;
    q3 += a3;
  }
  t = micros() - t;
  Serial.print("1000 pairs of conversions in "); Serial.print(t); Serial.println(" micros");
  Serial.print("A0 total:"); Serial.println(q0);
  Serial.print("A1 total:"); Serial.println(q1);
  Serial.print("A2 total:"); Serial.println(q2);
  Serial.print("A3 total:"); Serial.println(q3);
}
