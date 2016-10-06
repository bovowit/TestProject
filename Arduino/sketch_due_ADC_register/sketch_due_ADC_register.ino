unsigned long start_time;
unsigned long stop_time;
unsigned long values[1000];
unsigned long values2[1000];

void setup() {        
  Serial.begin(9600);  
 //analogReadResolution(12);
  
  ADC->ADC_MR |= 0x80;  //set free running mode on ADC
  ADC->ADC_CHER = 0x80; //enable ADC on pin A0
}

void loop() {
  unsigned int i;
    
  start_time = micros();
  for(i=0;i<1000;i++){
    while((ADC->ADC_ISR & 0x80)==0); // wait for conversion
    values[i]=ADC->ADC_CDR[7]; //get values
    values2[i]=ADC->ADC_CDR[4];
  }
  stop_time = micros();

  Serial.print("Total time: ");
  Serial.println(stop_time-start_time); 
  Serial.print("Average time per conversion: ");
  Serial.println((float)(stop_time-start_time)/1000);

  Serial.print("Values : ");
  for(i=0;i<10;i++) {
    Serial.print(values[i]);
    Serial.print(" : ");
    Serial.println(values2[i]);
  }
  
  delay(2000);
}
