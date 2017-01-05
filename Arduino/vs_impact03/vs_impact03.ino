
// defaule setting 
const int gSensorCnt = 2;
int gSensitivity = 650;
int validation_time = 1000000;	// 1초

bool gArrSensorFlag[gSensorCnt] = { false, };
unsigned long gArrImpactTime[gSensorCnt] = { 0, };
//int gArrSensorValue[gSensorCnt] = { 0, };
unsigned long uiImpactStartTime = 0;
int recevied_count = 0;


int iRetryCnt = 0;

void ClearData()
{
	for (int i = 0; i < gSensorCnt; i++)
	{
		gArrSensorFlag[i] = false;
		gArrImpactTime[i] = 0;
		//gArrSensorValue[i] = 0;
		uiImpactStartTime = 0;
		recevied_count = 0;
	}
}

void setup()
{
	Serial.begin(115200);
	int t = analogRead(0); t = analogRead(1);		// 잔류값 클리어

	ADC->ADC_MR |= 0x80; // these lines set free running mode on adc 7 and adc 6 (pin A0 and A1 - see Due Pinout Diagram thread)
	ADC->ADC_CR = 2;
	ADC->ADC_CHER = 0xC0; // this is (1<<7) | (1<<6) for adc 7 and adc 6     
	//ADC->ADC_CHER = 0xF0; // this is (1<<7) | (1<<6) for adc 7 and adc 4                                     
}

void loop()
{
	if (Serial.available() > 0)
	{
		char temp_buff[8] = { 0, };
		int length = Serial.readBytes(temp_buff, 30);
		String sReadData = temp_buff;
		gSensitivity = sReadData.toInt();
		Serial.print("Sensitivity value input : "); Serial.println(sReadData);
	}

	//for (int i = 0; i < gSensorCnt; i++) 
	{
		//while ((ADC->ADC_ISR & 0x80) == 0);
		//while ((ADC->ADC_ISR & 0xC0) == 0);// wait for 2 conversions
		//while ((ADC->ADC_ISR & 0xE0) == 0);// wait for 3 conversions
		//while ((ADC->ADC_ISR & 0xF0) == 0);// wait for 4 conversions
		//gArrSensorValue[0] = ADC->ADC_CDR[7];              // read data on A0 pin
		//gArrSensorValue[1] = ADC->ADC_CDR[6];              // read data on A1 pin
		//gArrSensor[2] = ADC->ADC_CDR[5];              // read data on A2 pin
		//gArrSensor[3] = ADC->ADC_CDR[4];              // read data on A3 pin
	}
	/*
	if ((ADC->ADC_ISR & 0x80) == 1)
	a0 = ADC->ADC_CDR[7];
	if ((ADC->ADC_ISR & 0x40) == 1)
	a1 = ADC->ADC_CDR[6];
	if ((ADC->ADC_ISR & 0x20) == 1)
	a2 = ADC->ADC_CDR[5];
	if ((ADC->ADC_ISR & 0x10) == 1)
	a3 = ADC->ADC_CDR[4];              // read data on A3 pin
	*/

	//int _value = 0;
	for (int i = 0; i < gSensorCnt; i++)
	{
		if (gArrSensorFlag[i] == true)
			continue;
		if (analogRead(i) > gSensitivity)		// 값이 도착하지 않은 것만 읽게하여 효율적으로 처리.
		{
			gArrSensorFlag[i] = true;
			gArrImpactTime[i] = micros();
			recevied_count++;

			if (uiImpactStartTime == 0)
				uiImpactStartTime = micros();
		}
	}
	if (recevied_count == gSensorCnt)
	{
		Serial.print(iRetryCnt); Serial.println("  --- Seccesed Impact Index ---");
		Serial.print("RX : ");
		int _due_time = 0;
		//for (int i = 0; i < gSensorCnt; i++)
		{
			_due_time = gArrImpactTime[0] - gArrImpactTime[1];
			Serial.print(_due_time);
		}

		ClearData();
	}
	else
	{
		if (uiImpactStartTime > 0 && micros() - uiImpactStartTime > validation_time)	// 타임의 롤오버 처리 필요.
		{
			ClearData();
			Serial.println(" failed receve all sensor .. so clear data");
		}
	}


	//int recevied_count = 0;
	//for (int i = 0; i < gSensorCnt; i++)
	//{
	//	if (gArrSensorValue[i] > gSensitivity)
	//	{
	//		gArrImpactTime[i] = micros();
	//		gArrSensorFlag[i] = true;
	//		recevied_count++;

	//		if (uiImpactStartTime == 0)
	//			uiImpactStartTime = micros();
	//	}
	//}



}