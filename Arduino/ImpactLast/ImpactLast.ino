const int gSensorCount = 2;
const int gImpactGap = 15;		// 충격판단값 - 기준값에서 +- 변화폭
const int gInvalidSensingValue = 5;	// 센서의 평균값으로부터 차이가 너무 작을 경우, 센싱 실패한 것으로 처리.

int gBaseValue[gSensorCount];	// 기준값 - 안정모드에서 각 센스 z 값 평균

unsigned long gImpactStartTime = 0;
int gAverageSensingTime = 0;	// 센서값을 읽는데 소요된 시간, 소요시간/센싱횟수 ===================> 오차 줄이기위한 부동소숫점 연산 체크
int gArrImpactSensingIndex[gSensorCount] = { 0, }; // 최대값을 기록한 인덱스 (시간 = index * gAverageSensingTime)

const int gOslioCount = 200;
int gSensorValue[gSensorCount][gOslioCount]; // 센서에서 측정한값. => 실제 모드에서는 필요하지 않음. 최대값만 필요.
enum { MODE_IMPACT, MODE_OSILLO};// 0 : first impact, 1 : osillo, 
int gRunMode = MODE_OSILLO;	

void setup() 
{
	//Serial.begin(115200);
	Serial.begin(250000);
	calibration();


}

void clear()
{
	gImpactStartTime = 0;
	for (int idx = 0; idx < gSensorCount; idx++)
	{
		for (int i = 0; i < gOslioCount; i++)
		{
			gSensorValue[idx][i] = 0;
		}
		gArrImpactSensingIndex[idx] = 0;
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

		if (gRunMode == MODE_IMPACT)
		{
			Serial.print(idx); Serial.print(" Sensor Caliburation value = "); Serial.println(gBaseValue[idx]);
		}
	}
}

void displayFlotter()
{
//	for (int i = 0; i < 20; i++)
//		Serial.println(gBaseValue[0]);
	for (int idx = 0; idx < gSensorCount; idx++)
	{
    for (int i = 0; i < 10; i++)
      Serial.println(gBaseValue[idx]); 
		for (int retry = 1; retry < gOslioCount; retry++)
		{
			Serial.println(gSensorValue[idx][retry]);
		}
		for (int i = 0; i < 10; i++)
			Serial.println(gBaseValue[idx]);
	}
}

void RunOsillo()
{
	if (gImpactStartTime == 0)
	{
		for (int i = 0; i < gSensorCount; i++)
		{
			gSensorValue[i][0] = analogRead(i);
			if (abs(gBaseValue[i] - gSensorValue[i][0]) > gImpactGap)
			{
				gImpactStartTime = micros();
			}
		}
	}
	if(gImpactStartTime > 0)
	{
		for (int retry = 1; retry < gOslioCount; retry++)
		{
			for (int idx = 0; idx < gSensorCount; idx++)
			{
				gSensorValue[idx][retry] = analogRead(idx);
			}
		}
	}
	
	gAverageSensingTime = (micros() - gImpactStartTime) / (gOslioCount * gSensorCount);  
}


// 각 센서에서 최고값을 기록한 인덱스 저장, 최소값 이하일 경우 실패
// 각 센서가 최고값을 기록한 시간 계산
bool CalculateMaxSensingTime()
{
	int _maxval = 0;
	int _delta = 0;
	int _base = 0;
	for (int idx = 0; idx < gSensorCount; idx++)
	{
		_base = gBaseValue[idx];
		for (int retry = 0; retry < gOslioCount; retry++)
		{
			_delta = abs(_base - gSensorValue[idx][retry]);
			if (_delta < gInvalidSensingValue)
				continue;
			if(_delta > _maxval)
			{
				_maxval = _delta;
				gArrImpactSensingIndex[idx] = retry;
			}
		}
		if (gArrImpactSensingIndex[idx] == 0)	// 최소기준값보다 큰 충격값이 없을 경우 => 무효화.
			return false;
		_delta = 0;
		_maxval = 0;
	}
	return true;
}

int _rcnt = 0;
void loop()
{
	if (gRunMode == MODE_IMPACT)
	{
		RunOsillo();
		if (gImpactStartTime == 0)
			return;
     
		if (CalculateMaxSensingTime() == false)
		{
			Serial.println("=== failed impact sensing ======");   
			clear();
			return;
		}

		_rcnt++;
		Serial.print(_rcnt); 
    Serial.print(" : ");
    Serial.print(gAverageSensingTime);
		Serial.print(" : IMP = ");
		for (int idx = 0; idx < gSensorCount; idx++)
		{
			Serial.print(gArrImpactSensingIndex[idx]);
			Serial.print(" - ");
		}
		Serial.println("");
		delay(100);

		//SendImpactTime();
		//CalculateImpactPointer();

		clear();

	}

	if (gRunMode == MODE_OSILLO)
	{
		RunOsillo();
		if (gImpactStartTime > 0)
		{
			displayFlotter();
			clear();
		}
	}
}
