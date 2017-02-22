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
	int x;			// x 좌표
	int y;			// y 좌표

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
	int x;			// x축 충격값
	int y;			// y축 충격값
	int z;			// z축 충격값
	int t;			// delta time (micro second)
	CImpact() { idx = 0; x = 0; y = 0; z = 0; t = 0; }
	CImpact(int i, int a, int b, int c, int d) : idx(i), x(a), y(b), z(c), t(d) {};
};


class Config
{
public:
	int speed;		// 매질의 전파 속도 : m/s
	int width;		// 충돌판의 width : mm
	int height;		// 충돌판의 height : mm
	int cycle_time;		// 하나의 충격으로 처리할 시간
	int impact_limit;	// 충격값 필터링 기준, 노이즈 필터링 

	int micro_speed;	// mm/microsecond
	int half_distance;
	//svCP 6개 : 센터를 중심으로 각 사분면의 좌표 4개와 중심좌료 2개.
	vector<CPoint> vBasePos;

	Config() { speed = 2500; width = 400; height = 400; cycle_time = 10000; impact_limit = 1000; _init(); }
	// Config() { speed = 5710(종파), 3430(횡파); width = 1439.0; width = 809.4; }	// 강화유리, 65인치 TV크기

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
	vector<CImpact> m_vImpact;					// 6개의 센서 측정값 저장
	typedef pair<CPoint, CPoint> LineT;			// 두 원의 교차점으로 이루어진 직선
	vector<LineT> m_vLines;						// 직선들의 벡터
	CPoint m_Triangle[3];						// 세 선의 교차점으로 찾은 삼각형

public:
	TdEye();
	~TdEye();

	void SetImpact(string sSerialData);
	bool SetImpact(int iIndex, int iX, int iY, int iZ, int iDeltaTime);
	void ClearImpact();
	bool _CalCrossCircle(int sensor1, int sensor2, int iExpTime, CPoint & IP1, CPoint & IP2);		// 임의의 2개 센서값으로부터 계산된 교점을 리턴
	bool CalCrossCircle(int sensor1, int sensor2);
	bool CalCrossCircleFix(int sensor1, int sensor2);

	bool TdEye::SelectSensorPair();				// 1 ~ 6 의 센서 중 3개를 선택 (1~4 우선)

	bool _CheckCross(const CPoint& AP1, const CPoint& AP2, const CPoint& BP1, const CPoint& BP2, CPoint* IP);
	bool _CheckCrossExt(CPoint& AP1, CPoint& AP2, CPoint& BP1, CPoint& BP2, CPoint &IP);
	bool CheckCross();

	bool HitPoint(CPoint& hPoint);
};

