// BLAST_LOGIC1.cpp : Defines the entry point for the console application.
// 세개의 기준점에서 감지된 충격값을 통해 충격 지점을 계산.
// 충격 시점의 시간을 정확히 알수 없으므로, 최소 4개 지점에서의 시간을 측정해서 3지점의 시간의 차이(델타값)을 계산.
// 시간의 델타값은 최초 도달 시점의 
// 센서값이 입력되기 시작하면 지정된시간(10000 micro seconds) 동안 하나의 충격으로 처리.

#include "stdafx.h"
#include <iostream>
//#include <math.h>
//#include <errno.h>
//#include <fenv.h>
#include <thread> 
//#include <chrono>
#include <string>

#include "TdEye.h"

using namespace std;

int main()
{
	TdEye oTE;

	//oTE.SetImpact(0, 5000, 2000, 1000, 550 / 6);
	//oTE.SetImpact(1, 5000, 2000, 1000, 550 / 6);
	//oTE.SetImpact(2, 5000, 2000, 1000, 700 / 6);
	//oTE.SetImpact(3, 5000, 2000, 1000, 700 / 6);
	//oTE.SetImpact(4, 5000, 2000, 1000, 0);
	//oTE.SetImpact(5, 5000, 2000, 1000, 400 / 6);

	oTE.SetImpact("152:124:24:");
	//oTE.SetImpact("120:32:112:");
	//oTE.SetImpact("376:356:24:");

	//string sSensorData = "0,5000,2000,1000,125:1,6000,3000,3000,89:2,5000,2000,1000,90";

	//SelectSensorPair();	// 나중에 구현.
	//  두개의 센서로부터 원의 교점을 잇는 직선을 구해서 저장
	oTE.CalCrossCircleFix(0, 1);	
	oTE.CalCrossCircleFix(0, 2);
	oTE.CalCrossCircleFix(1, 2);

	// 세개의 선으로부터 각 선의 교점 3개를 구해서 저장
	bool bRet = oTE.CheckCross();
	if (bRet == false)
	{
		cout << "not enough impact point..";
		std::this_thread::sleep_for(std::chrono::seconds(10000));
		return 0;
	}

	CPoint hPoint;
	// 저장된 세개의 교점의 무게중심 (타격 지점)을 구함.
	bRet = oTE.HitPoint(hPoint);

	cout << "Hit Point : " << hPoint.x << ", " << hPoint.y << endl;
	std::this_thread::sleep_for(std::chrono::seconds(10000));
    return 0;
}

