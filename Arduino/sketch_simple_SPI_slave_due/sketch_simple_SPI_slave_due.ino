
#include <SPI.h>

#define SS 53

void setup() 
{
   SPI.begin();
   pinMode(SS,INPUT);
   Serial.begin(115200);
}

void loop()
{
   char c = SPI.transfer(SS,0x45,SPI_CONTINUE);
   Serial.println(c);
   delay(1000);
}
