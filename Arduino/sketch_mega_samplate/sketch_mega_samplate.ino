#define MAX_ADC_RESOLUTION 10

const unsigned char PS_16 = (1 << ADPS2);
const unsigned char PS_32 = (1 << ADPS2) | (1 << ADPS0);
const unsigned char PS_64 = (1 << ADPS2) | (1 << ADPS1);
const unsigned char PS_128 = (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);


void setup() {
    Serial.begin(9600);
  
    // set up the ADC
    ADCSRA &= ~PS_128;  // remove bits set by Arduino library
  
    // you can choose a prescaler from above.
    // PS_16, PS_32, PS_64 or PS_128
    ADCSRA |= PS_16;    // set our own prescaler to 64 
 
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
   value = analogRead(4);
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

