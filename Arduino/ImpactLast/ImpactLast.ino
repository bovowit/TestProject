
const int gSensorCount = 2;
const int gImpactGap = 20;		// 충격판단값 - 기준값에서 +- 변화폭

int gBaseValue[gSensorCount];	// 기준값 - 안정모드에서 각 센스 z 값 평균
bool gFlagImpactStart = false;
unsigned long gImpactTime[gSensorCount];

const int gOslioCount = 300;
int gSensorValue[gSensorCount][gOslioCount]; // 센서에서 측정한값. => 실제 모드에서는 필요하지 않음. 최대값만 필요.
enum { EFASTIMPACT, EOSILLO};// 0 : first impact, 1 : osillo, 
int gRunMode = EFASTIMPACT;	

void setup() 
{
	Serial.begin(115200);
	calibration();


}

void clear()
{
	for (int j = 0; j < gSensorCount; j++)
	{
		gImpactTime[j] = 0;
		for (int i = 0; i < gOslioCount; i++)
		{
			gSensorValue[j][i] = 0;
		}
	}
	gFlagImpactStart = false;

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

void displayFlotter()
{
	for (int i = 0; i < 20; i++)
		Serial.println(gBaseValue[0]);
	for (int idx = 0; idx < gSensorCount; idx++)
	{
		for (int retry = 1; retry < gOslioCount; retry++)
		{
			Serial.println(gSensorValue[idx][retry]);
		}
		for (int i = 0; i < 20; i++)
			Serial.println(gBaseValue[idx]);
	}
}

void RunOsillo()
{
	if (gFlagImpactStart == false)
	{
		for (int i = 0; i < gSensorCount; i++)
		{
			gSensorValue[i][0] = analogRead(i);
			if (abs(gBaseValue[i] - gSensorValue[i][0]) > gImpactGap)
			{
				gFlagImpactStart = true;
			}
		}
	}
	else
	{
		for (int retry = 1; retry < gOslioCount; retry++)
		{
			for (int idx = 0; idx < gSensorCount; idx++)
			{
				gSensorValue[idx][retry] = analogRead(idx);
			}
		}

		displayFlotter();
		clear();
	}
}

void loop()
{
	if (gRunMode == EFASTIMPACT)
	{

	}

	if (gRunMode == EOSILLO)
	{
		RunOsillo();
	}
}
