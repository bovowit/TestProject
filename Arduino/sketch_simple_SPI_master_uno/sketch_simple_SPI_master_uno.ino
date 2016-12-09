#include <SPI.h>
byte empty = 'a';
#define SS 10
int idx = 0;

void setup (void)
{
  SPI.begin (); // SPI 통신 초기화
     SPI.beginTransaction(SPISettings(14000000, MSBFIRST, SPI_MODE0));
  pinMode(SS, OUTPUT);
  digitalWrite(SS, HIGH);   // SS = 10 : 슬레이브가 선택되지 않은 상태로 유지
  //SPI.setClockDivider(SPI_CLOCK_DIV16);  // 안정적인 전송을 위해 분주비를 높여 전송 속도를 낮춤
  Serial.begin(115200);
  Serial.println("SPI master start... Press f key ");
}
void loop (void)
{
  if(Serial.available())
  {
     digitalWrite(SS, HIGH);
    byte data = Serial.read(); // 데이터 입력 확인
    idx = 0;
    if(data == 'f')
    {
      digitalWrite(SS, LOW); // 슬레이브를 선택한다.
      delay(10);
      while(idx < 100)
      {
        char recv = SPI.transfer(empty); // 1바이트 데이터 수신을 위해 의미 없는 1바이트 데이터를 전송한다.
        //digitalWrite(SS, HIGH); // 슬레이브 선택을 해제한다.
        if(recv > 0x00)//0x21) // !이상의 아스키코드 부터 처리
        {

          Serial.print(idx); Serial.print(" M Recv : "); Serial.print(recv); Serial.print(" : "); Serial.println((byte)recv);
          delay(10);
        }
        else
        {
          delay(10);
          //Serial.print(recv);
        } 
        idx++;
      }
      digitalWrite(SS, HIGH); // 슬레이브 선택을 해제한다.
      Serial.println(" - Invalid recv data so break loop ... press f key..");
    }
  }
}

