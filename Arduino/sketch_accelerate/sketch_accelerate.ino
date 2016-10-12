// 
// 데이터 수집에 집중, 모든 처리는 PC에서..
// 첫번째 측정 시간만을 표시하고 나머지는 6micro second로 계산? 

#define MAX_ADC_RESOLUTION 10

// config value
const int gSensorCnt = 4;
const int gMeasureCnt = 100;
int gSensitivity = 300;
const int gCalibrationValue = 6;      // 6micro seconds : 보정은 불가능
const int gBlastCycle = 1000;       // 400 microseconds X 6mm = 2400mm : 최대 대각선 길이보다 커야 함  +  파장길이고려해야 신호 섞이지 않음

// data
struct base
{
  int value;
  unsigned long time;
};
base arrAccel[gSensorCnt][gMeasureCnt];
int valueIndex[gSensorCnt];

void setup()
{
  Serial.begin(9600);
  analogReadResolution(MAX_ADC_RESOLUTION);
  
  Serial.print("Start analize Blast Accelerate !!  : ");
}

int iMeasuredCnt = 0;
void loop()
{
  // for test 
  if (Serial.available() > 0)
  {
    char temp_buff[30] = { 0, };
    int length = Serial.readBytes(temp_buff, 30);
    String sReadData = temp_buff;
    gSensitivity = sReadData.toInt();
    Serial.print("Sensitivity value input : "); Serial.println(sReadData);
  }
  iMeasuredCnt++;


  bool bBlast = false;
  int tempValue = 0;
  int tempPin = 0;
  unsigned long sTime = 0;
  unsigned long dueTime = 0;

  for (int i = 0; i < gSensorCnt; i++)      // 초기화
  {
    valueIndex[tempPin] = 0;
    for (int j = 0; j < gMeasureCnt; j++)
    {
      arrAccel[i][j].time = 0;
      arrAccel[i][j].value = 0;
    }
  }

  for (tempPin = 0; tempPin < gSensorCnt; tempPin++)    // 첫번째 신호 감지 후 값을 저장, bBlast 플랫그 셋팅.
  {
    tempValue = abs(analogRead(tempPin));
    if (tempValue > gSensitivity) // && abs(tempValue < gSensitivity * 10)) // 최대값 설정은 하지 않음.
    {
      bBlast = true;
      sTime = micros();
      arrAccel[tempPin][valueIndex[tempPin]].value = tempValue;
      arrAccel[tempPin][valueIndex[tempPin]].time = sTime;
      valueIndex[tempPin]++;
      break;
    }
  }
  
  int iRetryCnt = 0;
  while(bBlast && dueTime < gBlastCycle)      // 첫번째 신호가 감지된 후 일정 시간 동안 각 센서의 값을 gMeasureCnt 번씩 저장.
  {
    if (iRetryCnt >= gMeasureCnt * 4)     // 센서의 숫자 x 측정횟수 만큼만 반복해서 측정하면, 4개의 센서 데이터 배열을 모두 채움.
      break;
    for (tempPin ; tempPin < 4; tempPin++)    // 첫번째 신호가 감지된 핀 다음부터 계속
    {
      if (valueIndex[tempPin] >= gMeasureCnt) // arrAccel를 gMeasureCnt 만큼 채우면 더이상 해당 핀의 데이터 수집 안함. 데이터 수집이 더 효율적이 됨.
        continue;
      tempValue = analogRead(tempPin);
      if (abs(tempValue) > gSensitivity)
      {
        arrAccel[tempPin][valueIndex[tempPin]].value = tempValue;
        arrAccel[tempPin][valueIndex[tempPin]].time = micros();
        valueIndex[tempPin]++;
      }
    }
    dueTime = sTime - micros();
    tempPin = 0;                // 첫번째 센서로 셋팅
    iRetryCnt++;
  }

  Serial.print("Measured Accleration : "); Serial.println(iMeasuredCnt);
  int i = 0;
  int j = 0;
  for (i = 0; i < gSensorCnt; i++)
  {
    Serial.print("Sensor : "); Serial.print(i); Serial.print("  First time : "); Serial.println(arrAccel[i][0].time);
    for (j = 0; j < gMeasureCnt; j++)
    {
      Serial.print(arrAccel[i][j].value); Serial.print("|");
      if (arrAccel[i][j].value < gSensitivity)
        break;
    }
    Serial.println("[E]");
    unsigned long _dueTime = arrAccel[i][j-1].time - arrAccel[i][0].time;
    Serial.print("Due Time : "); Serial.println(_dueTime);
  }
  Serial.println("---------------------------------------------");
}
