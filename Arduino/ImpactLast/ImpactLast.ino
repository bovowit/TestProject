const int gSensorCount = 3;
const int gImpactGap = 80;		        // 충격판단값 - 기준값에서 +- 변화폭
<<<<<<< .mine
const int gInvalidSensingValue = 15;	// 센서의 평균값으로부터 차이가 너무 작을 경우, 센싱 실패한 것으로 처리.
=======
const int gInvalidSensingValue = 10;	// 센서의 평균값으로부터 차이가 너무 작을 경우, 센싱 실패한 것으로 처리.
>>>>>>> .theirs
//const int gImpactGap = 30;           // 자이로센서의 경우 적용
//const int gInvalidSensingValue = 20;  // 자이로센서의 경우 적용

int gBaseValue[gSensorCount];	// 기준값 - 안정모드에서 각 센스 z 값 평균

unsigned long gImpactStartTime = 0;
int gAverageSensingTime = 0;	// 센서값을 읽는데 소요된 시간, 소요시간/센싱횟수 ===================> 오차 줄이기위한 부동소숫점 연산 체크
int gArrImpactSensingIndex[gSensorCount] = { 0, }; // 최대값을 기록한 인덱스 (시간 = index * gAverageSensingTime)

const int gOslioCount = 200;
int gSensorValue[gSensorCount][gOslioCount]; // 센서에서 측정한값. => 실제 모드에서는 필요하지 않음. 최대값만 필요.
enum { MODE_IMPACT, MODE_OSILLO, MODE_STREAM};// 0 : first impact, 1 : osillo, 2 : stream
int gRunMode = MODE_IMPACT;

void setup() 
{
	Serial.begin(115200);
	//Serial.begin(250000);
	//calibration();      
	empty();            // 잔류 전류 제거 목적
	calibration();      // 실제 평균값 측정
  
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
void empty()
{
	int idx = 0;
	for (int retry = 0; retry < 100; retry++)
	{
		for (idx = 0; idx < gSensorCount-1; idx++)
		{
		  gBaseValue[idx] = analogRead(idx);
		  Serial.print(gBaseValue[idx]);
		  Serial.print(" ");
		}
		gBaseValue[idx] = analogRead(idx);
		Serial.println(gBaseValue[idx]);
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
			//delay(1);
		}
		gBaseValue[idx] = int(_totalvalue / 100);
		//gBaseValue[idx] = 80;
    
		if (gRunMode == MODE_IMPACT)
		{
			//Serial.print(idx); Serial.print(" Sensor Caliburation value = "); Serial.println(gBaseValue[idx]);
		}
   else 
   {
    
   }
	}
}

void displayFlotter()
{
    int idx = 0;
	for (int retry = 0; retry < 20; retry++)
	{
    for (idx = 0; idx < gSensorCount-1; idx++)
		{
			Serial.print(gBaseValue[idx]);
			Serial.print(" ");
		}
		Serial.println(gBaseValue[idx]);
	}

	for (int retry = 0; retry < gOslioCount; retry++)
	{
		for (idx = 0; idx < gSensorCount-1; idx++)
		{
			Serial.print(gSensorValue[idx][retry]);
			Serial.print(" ");
		}
		Serial.println(gSensorValue[idx][retry]);
	}

	for (int retry = 0; retry < 20; retry++)
	{
		for (idx = 0; idx < gSensorCount-1; idx++)
		{
			Serial.print(gBaseValue[idx]);
			Serial.print(" ");
		}
		Serial.println(gBaseValue[idx]);
	}
}

void RunOsillo()
{
	if (gImpactStartTime == 0)
	{
		for (int idx = 0; idx < gSensorCount; idx++)
		{
			gSensorValue[idx][0] = analogRead(idx);
			if (abs(gSensorValue[idx][0] - gBaseValue[idx]) > gImpactGap)
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

bool bTrig = false;
void RunStream()
{
	int idx = 0;
	int _trig = 0;
	for (idx = 0; idx < gSensorCount-1; idx++)
	{
		Serial.print(analogRead(idx));
		Serial.print(" ");
	}
	_trig = analogRead(idx);
	Serial.println(_trig);

	if (abs(_trig - gBaseValue[0]) > 30)
	{
		if(bTrig)
			delay(1000);
    !bTrig;
  }
 //delay(1);
}

// 각 센서에서 최고값을 기록한 인덱스 저장, 최소값 이하일 경우 실패
// 각 센서가 최고값을 기록한 시간 계산
bool CalculateMaxSensingTime()
{
	int _maxval = 0;
	int _delta = 0;
	int _base = 0;
	int _decreasecnt = 0;
	for (int idx = 0; idx < gSensorCount; idx++)
	{
		_base = gBaseValue[idx];
		for (int retry = 0; retry < gOslioCount; retry++)
		{
			//_delta = abs(_base - gSensorValue[idx][retry]);
			_delta = gSensorValue[idx][retry] - _base;
			if (_delta < gInvalidSensingValue)
			{
				_decreasecnt++;				// 피크 이후 하강 체크
				continue;
			}
			if (_delta > _maxval)
			{
				_decreasecnt = 0;			// 최대값 갱신할때 하강체크 리셋
				_maxval = _delta;
				gArrImpactSensingIndex[idx] = retry;
			}
			else
				_decreasecnt++;

			if (_decreasecnt > 20)
			{
				break;
			}
		}

		if (gArrImpactSensingIndex[idx] == 0)	// 하나의 센서에서라도 최소기준값보다 큰 충격값이 없을 경우 => 무효화.
			return false;
		_delta = 0;
		_maxval = 0;
	}
	return true;
}

// 빠른 종파로 충격 시작을 감지하고, 횡파가 도달한 시간 즉 최대 충격값을 기준으로 시간 측정
bool CalculateFirstPickTime()
{
	int iPickCount = 0;
	for (int idx = 0; idx < gSensorCount; idx++)
	{
<<<<<<< .mine
		//int iFirstMinVal = gBaseValue[idx] - 50;
		int iFirstMaxVal = gBaseValue[idx] + 80;


=======
		//int iFirstMinVal = 0;
		//if(gBaseValue[idx] - 50 > 0)
		//	iFirstMinVal = gBaseValue[idx] - 50;
		int iFirstMaxVal = gBaseValue[idx];// +80;
>>>>>>> .theirs
		int iLowCount = 0;
		int iHighCount = 0;
		for (int retry = 0; retry < gOslioCount; retry++)
		{
      /*// 압전센서는 하강이 없음. (zero base)
			if (iFirstMinVal > gSensorValue[idx][retry])	// 하강
			{
				iFirstMinVal = gSensorValue[idx][retry];
				iHighCount = 0;
			}
			else if(iFirstMinVal + 100 > gSensorValue[idx][retry])
			{
				iHighCount++;
				if (iHighCount > 5)	// 하강중에 상승이 연속해서 10회 이상 나타날때.. 피크로 인식
				{
					gArrImpactSensingIndex[idx] = retry;
					iPickCount++;
					break;
				}
			}
			*/
			
			if (iFirstMaxVal < gSensorValue[idx][retry])	// 상승
			{
				iFirstMaxVal = gSensorValue[idx][retry];
				iLowCount = 1;								// 최대값이 갱신되면 리셋
				continue;
			}
			
			if(iLowCount > 0)
			{
				if (iFirstMaxVal - gSensorValue[idx][retry] < gInvalidSensingValue)		// 노이즈 제거
					continue;
				iLowCount++;
<<<<<<< .mine
				if (iLowCount > 3) // 상승중에 하강이 연속해서 5회 이상 나타날때.. 피크로 인식
=======
				if (iLowCount > 3) // 상승중에 하강이 연속해서 3회 이상 나타날때.. 피크로 인식
>>>>>>> .theirs
				{
					gArrImpactSensingIndex[idx] = retry;
					iPickCount++;    
					break;
				}
			}
		}
	}

	if (iPickCount < gSensorCount)    // 발견된 pick의 수.
		return false;

	return true;
}

int _rcnt = 0;
void loop()
{
	switch (gRunMode)
	{
		case MODE_IMPACT:
		{
			RunOsillo();
			if (gImpactStartTime == 0)
				return;

			//if(CalculateMaxSensingTime() == false)
			if(CalculateFirstPickTime() == false)
			{
				//Serial.println("=== failed impact sensing ======");
				clear();
				return;
			}
		  char buff[30] = {0, };
		  sprintf(buff, "%3d:%3d:%3d:", gArrImpactSensingIndex[0]*4, gArrImpactSensingIndex[1]*4, gArrImpactSensingIndex[2]*4);
		  Serial.write(buff, 12);
		  //Serial.println(buff);
		  delay(100);
			/*_rcnt++;
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
       */
       
			//SendImpactTime();
			//CalculateImpactPointer();

			clear();
			break;
		}
		case MODE_OSILLO:
		{
			RunOsillo();
			if (gImpactStartTime > 0)
			{
				displayFlotter();
				clear();
				calibration();
			}
			break;
		}
		case MODE_STREAM:
		{
			RunStream();
      _rcnt++;
      if(_rcnt > gOslioCount)
      {
        _rcnt = 0;
       // delay(3000);
      }
		}
	}

}
