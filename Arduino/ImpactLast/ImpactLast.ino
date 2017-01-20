const int gSensorCount = 2;
const int gImpactGap = 10;		// 충격판단값 - 기준값에서 +- 변화폭
int gBaseValue[gSensorCount];	// 기준값 - 안정모드에서 각 센스 z 값 평균
bool gFlagImpactStart = false;


const int gOslioCount = 300;
int gSensorValue[gSensorCount][gOslioCount]; // 센서에서 측정한값. => 실제 모드에서는 필요하지 않음. 최대값만 필요.

void setup() 
{
	Serial.begin(115200);
	calibration();


}

void clear()
{
	for (int j = 0; j < gSensorCount; j++)
	{
		for (int i = 0; i < gOslioCount; i++)
		{
			gSensorValue[j][i] = 0;
		}
	}
}

void calibration()
{
	for (int idx = 0; idx < gSensorCount; idx++)
	{
		int _totalvalue = 0;
		for (int i = 0; i < 100; i++)
		{
			_totalvalue += analogRead(idx);
			delay(1);
		}
		gBaseValue[idx] = int(_totalvalue / 100);
		Serial.print(idx); Serial.print(" Sensor Caliburation value = "); Serial.println(gBaseValue[idx]);
	}
}

void loop() 
{
  delay(3000);
  calibration();
}
