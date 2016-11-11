#include <SPI.h>
char received = 'K';

void setup (void)
{
  SPI.begin (); // SPI 통신 초기화
  digitalWrite(SS, HIGH);   // SS = 10 : 슬레이브가 선택되지 않은 상태로 유지
  //SPI.setClockDivider(SPI_CLOCK_DIV16);  // 안정적인 전송을 위해 분주비를 높여 전송 속도를 낮춤
  Serial.begin(115200);
}
void loop (void)
{
  if(Serial.available()){
    char data = Serial.read(); // 데이터 입력 확인
    if(data == 'F')
    {
      while(received == 'K')
      {
        digitalWrite(SS, LOW); // 슬레이브를 선택한다.
        char received = SPI.transfer(received + 1); // 1바이트 데이터 수신을 위해 의미 없는 1바이트 데이터를 전송한다.
        digitalWrite(SS, HIGH); // 슬레이브 선택을 해제한다.
        Serial.println(received);
      }
    }
  }
}

