
#include <SPI.h>

#define SS 53
int idx = 0;
char SendByte[100] = {'t', 'e', 's', 't', '-','s','l','a','v','e','r','!', };
char ReadByte[100] = {'0', };

void setup() 
{
   SPI.begin(SS);
   pinMode(SS, OUTPUT); // ????
   Serial.begin(115200);
   Serial.println("Slaver Started..");
}

void loop()
{
  //char s = SPI.transfer(SS, 0x00);
  //Serial.print("received start char "); Serial.println(s);
  //if(s == 'K')
  { 
      Serial.println("master data received... ");
      for(int i = 0; i < 1; i++)
      {
        char ret = SPI.transfer(SS, SendByte[i], SPI_CONTINUE);
         Serial.print(idx); Serial.print(" : "); Serial.print(SendByte[i]); Serial.print(" : "); Serial.println(ret);
      }
       SPI.transfer(SS, 0x00);    // continue mode 종료
       delay(1000);
       idx++;
      //memset(ReadByte, 0, 100);    
  }

}
