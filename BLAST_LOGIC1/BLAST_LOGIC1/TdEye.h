#pragma once

#include <iostream>
#include <math.h>
#include <errno.h>
#include <fenv.h>
#include <thread> 
#include <chrono>

#include <vector>
#include <algorithm>
using namespace std;

class CPoint
{
public:
	int x;			// x ��ǥ
	int y;			// y ��ǥ

	CPoint() { x = 0; y = 0; }
	CPoint(int a, int b) : x(a), y(b) {};

	const CPoint operator *(const float f) const
	{
		return CPoint(x + x * f / 10, y + y * f / 10);
	}
};

class CImpact
{
public:
	int idx;		// sensor index
	int x;			// x�� ��ݰ�
	int y;			// y�� ��ݰ�
	int z;			// z�� ��ݰ�
	int t;			// delta time (micro second)
	CImpact() { idx = 0; x = 0; y = 0; z = 0; t = 0; }
	CImpact(int i, int a, int b, int c, int d) : idx(i), x(a), y(b), z(c), t(d) {};
};


class Config
{
public:
	int speed;		// ������ ���� �ӵ� : m/s
	int width;		// �浹���� width : mm
	int height;		// �浹���� height : mm
	int cycle_time;		// �ϳ��� ������� ó���� �ð�
	int impact_limit;	// ��ݰ� ���͸� ����, ������ ���͸� 

	int micro_speed;	// mm/microsecond
	int half_distance;
	//svCP 6�� : ���͸� �߽����� �� ��и��� ��ǥ 4���� �߽��·� 2��.
	vector<CPoint> vBasePos;

	Config() { speed = 2500; width = 400; height = 400; cycle_time = 10000; impact_limit = 1000; _init(); }
	// Config() { speed = 5710(����), 3430(Ⱦ��); width = 1439.0; width = 809.4; }	// ��ȭ����, 65��ġ TVũ��

	void _init()
	{
		vBasePos.push_back(CPoint(-width / 2, -height / 2));
		vBasePos.push_back(CPoint(width / 2, -height / 2));
		vBasePos.push_back(CPoint(width / 2, height / 2));
		vBasePos.push_back(CPoint(-width / 2, height / 2));
		vBasePos.push_back(CPoint(0, -height / 2));
		vBasePos.push_back(CPoint(0, height / 2));

		micro_speed = speed / 1000;		
		half_distance = sqrt(width * width + height * height) / 2;
	}
};


class TdEye
{
private:
	Config m_Config;
	vector<CImpact> m_vImpact;					// 6���� ���� ������ ����
	typedef pair<CPoint, CPoint> LineT;			// �� ���� ���������� �̷���� ����
	vector<LineT> m_vLines;						// �������� ����
	CPoint m_Triangle[3];						// �� ���� ���������� ã�� �ﰢ��

public:
	TdEye();
	~TdEye();

	void SetImpact(string sSerialData);
	bool SetImpact(int iIndex, int iX, int iY, int iZ, int iDeltaTime);
	void ClearImpact();
	bool _CalCrossCircle(int sensor1, int sensor2, int iExpTime, CPoint & IP1, CPoint & IP2);		// ������ 2�� ���������κ��� ���� ������ ����
	bool CalCrossCircle(int sensor1, int sensor2);
	bool CalCrossCircleFix(int sensor1, int sensor2);

	bool TdEye::SelectSensorPair();				// 1 ~ 6 �� ���� �� 3���� ���� (1~4 �켱)

	bool _CheckCross(const CPoint& AP1, const CPoint& AP2, const CPoint& BP1, const CPoint& BP2, CPoint* IP);
	bool _CheckCrossExt(CPoint& AP1, CPoint& AP2, CPoint& BP1, CPoint& BP2, CPoint &IP);
	bool CheckCross();

	bool HitPoint(CPoint& hPoint);
};

