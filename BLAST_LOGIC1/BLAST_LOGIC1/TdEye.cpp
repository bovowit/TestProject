#include "stdafx.h"
#include "TdEye.h"


TdEye::TdEye()
{
}

TdEye::~TdEye()
{
}

void TdEye::SetImpact(string sSerialData)
{
	string sTime = "";
	int findedidx = 0;
	int retry = 0;
	auto itr = sSerialData.begin();
	while(true)
	{
		int idx = sSerialData.find(":", findedidx);
		if (idx < 0)
			break;
		sTime = sSerialData.substr(findedidx, idx-findedidx);
		findedidx = idx + 1;

		int _time = atoi(sTime.c_str());
		CImpact imp(retry, 10, 10, 10, _time);
		m_vImpact.push_back(imp);
		retry++;
	}
}

bool TdEye::SetImpact(int iIndex, int iX, int iY, int iZ, int iDeltaTime)
{
	int iLimit = m_Config.impact_limit;
	if (iX < iLimit || iY < iLimit || iZ < iLimit)		// 노이즈 충격값 필터링
		return false;
	if (iDeltaTime < 0 || iDeltaTime > 1000)
		return false;

	CImpact imp(iIndex, iX, iY, iZ, iDeltaTime);
	m_vImpact.push_back(imp);

	return true;
}

void TdEye::ClearImpact()
{
	m_vImpact.clear();
	m_vLines.clear();
}

// 두 원의 교점 찾기 기본 알고리즘
bool TdEye::_CalCrossCircle(int sensor_index1, int sensor_index2, int iExpDist, CPoint & IP1, CPoint & IP2)
{
	CPoint AP1 = m_Config.vBasePos[sensor_index1];
	CPoint AP2 = m_Config.vBasePos[sensor_index2];
	float r1 = m_vImpact[sensor_index1].t * m_Config.micro_speed + iExpDist;		// 반지름 구하기 (시간 + 보정거리)
	float r2 = m_vImpact[sensor_index2].t * m_Config.micro_speed + iExpDist;

	float e = AP2.x - AP1.x;
	float f = AP2.y - AP1.y;
	//if (e * e + f * f < 0)
	//	return false;
	float p = sqrt(e * e + f * f);
	float k = (p * p + r1 * r1 - r2 * r2) / (2.0f * p);
	//if (r1 * r1 - k * k < 0)
	//	return false;	
	float temp = sqrt(r1 * r1 - k * k);
	if (math_errhandling & MATH_ERRNO)		// sqrt의 값이 '-'일 경우.. 두 원은 교차점이 없음.
		if (errno == EDOM)
			return false;
	IP1.x = AP1.x + (e * k) / p + (f / p) * temp;
	IP1.y = AP1.y + (f * k) / p - (e / p) * temp;
	IP2.x = AP1.x + (e * k) / p - (f / p) * temp;
	IP2.y = AP1.y + (f * k) / p + (e / p) * temp;
	return true;
}

bool TdEye::CalCrossCircle(int sensor_index1, int sensor_index2)
{
	CPoint IP1, IP2;
	int i = 0;
	for (i; i < 10; i++)
	{
		int iExpDist = m_Config.half_distance/2 + m_Config.half_distance * i / 10;
		if (_CalCrossCircle(sensor_index1, sensor_index2, iExpDist, IP1, IP2))
		{
			// IP1, IP2간 거리가 너무 짧은 경우 .. 한번더 확장해서 계산 
			if (sqrt(abs(IP2.x - IP1.x)*abs(IP2.x - IP1.x) + abs(IP2.y - IP1.y)*abs(IP2.y - IP1.y)) < 1)
				continue;
			m_vLines.push_back(make_pair(IP1, IP2));
			break;
		}
	}
	if (i == 9)
		return false;
	return true;
}

bool TdEye::CalCrossCircleFix(int sensor_index1, int sensor_index2)
{
	CPoint IP1, IP2;
	int i = 0;
	int iExpDist = m_Config.half_distance + 50;
	if (_CalCrossCircle(sensor_index1, sensor_index2, iExpDist, IP1, IP2))
	{
		// IP1, IP2간 거리가 너무 짧은 경우 .. 한번더 확장해서 계산 
		if (sqrt(abs(IP2.x - IP1.x)*abs(IP2.x - IP1.x) + abs(IP2.y - IP1.y)*abs(IP2.y - IP1.y)) < 1)
			return false;
		m_vLines.push_back(make_pair(IP1, IP2));
	}
	return true;
}

bool TdEye::SelectSensorPair()	// 3개 이상의 센서로부터 값이 전달되었을 경우, 적절한것 3개를 선택. 또는 모든 라인을 검출.
{
	vector<CImpact> vecTemp = m_vImpact;
	//sort(vecTemp.begin(), vecTemp.begin() + 4, Compare);

	return true;
}


// 두 직선의 교차점 구하기 : AP와 BP의 선이 교차하지 않는 경우 false return.
bool TdEye::_CheckCross(const CPoint& AP1, const CPoint& AP2, const CPoint& BP1, const CPoint& BP2, CPoint* IP)
{
	float t, s;
	float under = (BP2.y - BP1.y)*(AP2.x - AP1.x) - (BP2.x - BP1.x)*(AP2.y - AP1.y);
	if (under == 0)
		return false;
	float _t = (BP2.x - BP1.x)*(AP1.y - BP1.y) - (BP2.y - BP1.y)*(AP1.x - BP1.x);
	float _s = (AP2.x - AP1.x)*(AP1.y - BP1.y) - (AP2.y - AP1.y)*(AP1.x - BP1.x);
	t = _t / under;
	s = _s / under;

	if (t<0.0 || t>1.0 || s<0.0 || s>1.0)
		return false;
	if (_t == 0 && _s == 0)
		return false;

	IP->x = AP1.x + t * (float)(AP2.x - AP1.x);
	IP->y = AP1.y + t * (float)(AP2.y - AP1.y);
	return true;
}

bool TdEye::_CheckCrossExt(CPoint& AP1, CPoint& AP2, CPoint& BP1, CPoint& BP2, CPoint& IP)
{
	bool bRet = _CheckCross(AP1, AP2, BP1, BP2, &IP);
	if (bRet)
		return true;
	else	// 접점을 바로 찾지 못했을 경우 선을 충분히 확장한 후 다시 찾기
	{
		CPoint ap1, ap2, bp1, bp2;
		if (AP1.x == AP2.x)
		{
			ap1.x = AP1.x;
			ap2.x = AP2.x;
			ap1.y = AP1.y + 10000;
			ap2.y = AP2.y - 10000;
		}
		else
		{
			ap1.x = 10000;
			ap2.x = -10000;
			ap1.y = (((AP2.y - AP1.y) / (AP2.x - AP1.x)) * (ap1.x - AP1.x)) + AP1.y;
			ap2.y = (((AP2.y - AP1.y) / (AP2.x - AP1.x)) * (ap2.x - AP1.x)) + AP2.y;
		}

		if (BP1.x == BP2.x)
		{
			bp1.x = BP1.x;
			bp2.x = BP2.x;
			bp1.y = BP1.y + 10000;
			bp2.y = BP2.y - 10000;
		}
		else
		{
			bp1.x = 10000;
			bp2.x = -10000;
			bp1.y = (((BP2.y - BP1.y) / (BP2.x - BP1.x)) * (bp1.x - BP1.x)) + BP1.y;
			bp2.y = (((BP2.y - BP1.y) / (BP2.x - BP1.x)) * (bp2.x - BP1.x)) + BP2.y;
		}

		bRet = _CheckCross(ap1, ap2, bp1, bp2, &IP);
		return bRet;
	}
}

bool TdEye::CheckCross()
{
	if (m_vLines.size() < 3)	// 최소값이 필요
	{
		m_vLines.clear();
		return false;
	}
	bool bRet = _CheckCrossExt(m_vLines[0].first, m_vLines[0].second, m_vLines[1].first, m_vLines[1].second, m_Triangle[0]);
	bRet = _CheckCrossExt(m_vLines[0].first, m_vLines[0].second, m_vLines[2].first, m_vLines[2].second, m_Triangle[1]);
	bRet = _CheckCrossExt(m_vLines[1].first, m_vLines[1].second, m_vLines[2].first, m_vLines[2].second, m_Triangle[2]);

	//int iCrossPointCnt = 0;
	//while (iCrossPointCnt < 3)
	//{
	//	int iFirst = iCrossPointCnt;
	//	int iSecond = iCrossPointCnt + 1;
	//	bool bRet = _CheckCross(m_vLines[iFirst].first, m_vLines[iFirst].second, m_vLines[iSecond].first, m_vLines[iSecond].second, &m_Triangle[iCrossPointCnt]);
	//	if (bRet)
	//		iCrossPointCnt++;
	//	else
	//	{

	//	}
	//}
	return true;
}


bool TdEye::HitPoint(CPoint& hPoint)
{
	hPoint.x = (m_Triangle[0].x + m_Triangle[1].x + m_Triangle[2].x) / 3;
	hPoint.y = (m_Triangle[0].y + m_Triangle[1].y + m_Triangle[2].y) / 3;
	return true;
}