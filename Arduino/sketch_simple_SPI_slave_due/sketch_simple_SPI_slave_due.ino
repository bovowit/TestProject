
#include <SPI.h>

#define SS 53
int idx = 0;
byte SendByte[100] = {'t', 'e', 's', 't', '-','s','l','a','v','e','r','!', };
byte ReadByte[100] = {'0', };

void setup() 
{
   SPI.begin(SS);
   //pinMode(SS,INPUT); // ????
   Serial.begin(115200);
}

void loop()
{
  byte s = SPI.transfer(SS, 0x00);
  if(s == 'K')
  { 
      Serial.println("master data received... ");
      for(int i = 0; i < 15; i++)
      {
        ReadByte[i] = SPI.transfer(SS, SendByte[i], SPI_CONTINUE);
         Serial.print(idx); Serial.print(" : "); Serial.println(ReadByte[i]);
      }
       SPI.transfer(SS, 0x00);    // continue mode 종료
       delay(1000);
       idx++;
  }
   memset(ReadByte, 0, 100);
}
