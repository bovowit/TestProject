/**********************************************
  MASTER
**********************************************/

#include <SPI.h>

#define SS 10

void setup() 
{
  Serial.begin(115200);
   SPI.begin();
   pinMode(SS,OUTPUT);
   digitalWrite(SS,LOW);
}

void loop()
{
   byte recv = SPI.transfer(SS, 'E');
   Serial.println(recv);
   //SPI.transfer(SS,(byte)'M', SPI_CONTINUE);
   delay(10);
}
