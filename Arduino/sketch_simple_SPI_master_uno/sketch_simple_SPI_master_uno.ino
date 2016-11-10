
#include <SPI.h>

#define SS 10

void setup() 
{
   Serial.begin(115200);
   SPI.begin();
   pinMode(SS,OUTPUT);
   digitalWrite(SS,HIGH);
}

void loop()
{
   byte recv = SPI.transfer((byte)'E');
   
   Serial.print("RECV : "); Serial.println(recv);
   delay(1000);
}
