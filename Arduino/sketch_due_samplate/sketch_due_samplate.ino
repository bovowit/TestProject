//#define MAX_ADC_RESOLUTION 10 //changing resolution doesn't seem to affect sampling rate speed in this test
#define MAX_ADC_RESOLUTION 8
void setup() {
 Serial.begin(9600);
 analogReadResolution(MAX_ADC_RESOLUTION); //

 //adc_init(ADC, SystemCoreClock, ADC_FREQ_MAX*2, 1); //~313000 samples per second, A/D readings kaput
  //adc_init(ADC, SystemCoreClock, ADC_FREQ_MAX*2, 2); //~288 000 samples per second, A/D readings now have about 10% deviation
  ////adc_init(ADC, SystemCoreClock, ADC_FREQ_MAX*2, 3); //~267000 samples per second, A/D readings OK, the best compromise I got
  //adc_init(ADC, SystemCoreClock, ADC_FREQ_MAX*2, 4); //~220000 samples per second, A/D readings OK
  //adc_init(ADC, SystemCoreClock, ADC_FREQ_MAX*2, ADC_STARTUP_NORM ); //~65000 samples per second,A/D readings OK 
 //adc_init(ADC, SystemCoreClock, ADC_FREQ_MAX*2, ADC_STARTUP_FAST ); //~47000 samples per second, from the name, I was tempted to think this would perform better. But I know nothing.
 pinMode(ADC0, INPUT);
}

void loop() {  
 
 int start = millis();
 long num_samples = 0;
 
 //just to evaluate any possible accurancy degradation while messing around with this
 int value = 0;
 int max_value = 0;
 int min_value = 2^MAX_ADC_RESOLUTION; 
 
 //let's see how many samples we can read in 1 second
 while( millis() - start < 1000 )
 {
   value = analogRead(A0);//DC0);
  //value = analogRead(ADC4);
   if(value > max_value )
   max_value = value;
   
   if(value < min_value )
   min_value = value;
  
   num_samples++;
 }

 // print out the control results
 Serial.println("\nsamples read: "); 
 Serial.print(num_samples); 
 
 Serial.println("\nMax value: ");
 Serial.print( max_value ); 
 
 Serial.println("\nMin value: ");
 Serial.print( max_value ); 

 delay(1000);
}

