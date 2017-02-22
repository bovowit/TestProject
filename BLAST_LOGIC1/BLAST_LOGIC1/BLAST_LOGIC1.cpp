// BLAST_LOGIC1.cpp : Defines the entry point for the console application.
// ������ ���������� ������ ��ݰ��� ���� ��� ������ ���.
// ��� ������ �ð��� ��Ȯ�� �˼� �����Ƿ�, �ּ� 4�� ���������� �ð��� �����ؼ� 3������ �ð��� ����(��Ÿ��)�� ���.
// �ð��� ��Ÿ���� ���� ���� ������ 
// �������� �ԷµǱ� �����ϸ� �����Ƚð�(10000 micro seconds) ���� �ϳ��� ������� ó��.

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

	//SelectSensorPair();	// ���߿� ����.
	//  �ΰ��� �����κ��� ���� ������ �մ� ������ ���ؼ� ����
	oTE.CalCrossCircleFix(0, 1);	
	oTE.CalCrossCircleFix(0, 2);
	oTE.CalCrossCircleFix(1, 2);

	// ������ �����κ��� �� ���� ���� 3���� ���ؼ� ����
	bool bRet = oTE.CheckCross();
	if (bRet == false)
	{
		cout << "not enough impact point..";
		std::this_thread::sleep_for(std::chrono::seconds(10000));
		return 0;
	}

	CPoint hPoint;
	// ����� ������ ������ �����߽� (Ÿ�� ����)�� ����.
	bRet = oTE.HitPoint(hPoint);

	cout << "Hit Point : " << hPoint.x << ", " << hPoint.y << endl;
	std::this_thread::sleep_for(std::chrono::seconds(10000));
    return 0;
}

