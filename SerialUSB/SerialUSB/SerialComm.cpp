#include "stdafx.h"
#include "serialcomm.h"
#include <iostream>
#include <boost/thread/thread.hpp>
//#include <stdlib.h>
#include "..\\..\\BLAST_LOGIC1\\BLAST_LOGIC1\TdEye.h"

CSerialComm::CSerialComm()
{
	m_bConnected = false;
	m_lBaudRate = CBR_115200;
	m_sPort = "COM3";
}
CSerialComm::~CSerialComm() {}


int CSerialComm::connect(string portNum)
{
	m_sPort = portNum;	// 포트 넘버 저장

	if (!m_SerialPort.OpenPort(m_sPort)) //포트를 오픈하고 오픈에 실패하였으면 fail을 반환한다.
		return RETURN_FAIL;

	m_SerialPort.ConfigurePort(m_lBaudRate, 8, FALSE, NOPARITY, ONESTOPBIT); //포트 기본값을 설정한다.
	m_SerialPort.SetCommunicationTimeouts(0, 0, 100, 0, 0); //Timeout값 설정, 세번째 값으로 한번의 패킷을 100 msec 만큼 기다림.

	return RETURN_SUCCESS;
}

int	CSerialComm::TryConnect(string _portNum)	// 연결될때까지 10초 동안 반복.
{
	int iRet = RETURN_FAIL;
	int iRetryCnt = 0;

	iRet = connect(_portNum);
	if(!iRet)
		boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
	iRetryCnt++;

	if (iRet)
	{
		cout << _portNum << "  : try send command " << endl;
		iRet = sendCommand('N');
		if (iRet)
		{
			cout << _portNum << "  : success send command " << endl;
			boost::this_thread::sleep(boost::posix_time::milliseconds(100));

			char _rbuff[100] = { 0, };
			readCommand(_rbuff, 6);
			cout << _rbuff << "  : readed command... from arduino  !!" << endl;
			if (!strcmp("KAIFUN", _rbuff))
			{
				iRet = RETURN_SUCCESS;
				m_bConnected = true;
				cout << _portNum << "  : succeed connection" << endl;
			}
			else
			{
				iRet = RETURN_FAIL;
				cout << _portNum << "  : failed connection" << endl;
				disconnect();
			}
		}
		else
		{
			disconnect();
			cout << _portNum << "  : failed sendcommnad" << endl;
		}
	}
	return iRet;
}

int	CSerialComm::TryConnectSimple(string _portNum)	// 연결될때까지 10초 동안 반복.
{
	int iRet = RETURN_FAIL;
	int iRetryCnt = 0;

	for (int i = 0; i < 2; i++)
	{
		iRet = connect(_portNum);
		if (!iRet)
			boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
		else
			break;
	}
	return iRet;
}

int CSerialComm::sendCommand(char pos) //데이터를 전송하는 함수
{
	if (m_SerialPort.WriteByte(pos)) //WriteByte()를 통해 데이터 전송에 성공하면 SUCCESS, 실패하면 FAIL을 반환한다.
		return RETURN_SUCCESS;
	else
		return RETURN_FAIL;
}

void CSerialComm::disconnect() //포트를 다 쓰고 난뒤 닫는 함수
{
	m_SerialPort.ClosePort();
	m_bConnected = false;
}

bool CSerialComm::readCommand(char * pBuff, UINT length)
{
	BYTE * byte = new BYTE[100];
	if (m_SerialPort.ReadByte(byte, length))
	{

		memcpy(pBuff, byte, length);
	}
	else
		return false;

}

static TdEye oTE;
void CSerialComm::SerialCommRun()
{
	string sComPort = "";
	//for (int i = 2; i < 10; i++)	 //COM1~COM9 번의 포트를 오픈한다. 10초 동안 반복 -> 무한 반복?????
	for (int i = 0; i < 10; i++)
	{
		cout << "COM" << 8 << "  : try connection" << endl;
		sComPort = "COM" + to_string(8);
		//if (TryConnect(sComPort))
		if(TryConnectSimple(sComPort))
		{
			std::cout << "Connected [ " << sComPort << " ]"<< std::endl;
			break;
		}
	}

	// reading은 약속된 사이즈만큼 반복읽기.. ReadByte(BYTE* &resp, UINT size) 사용.
	// disconnect event나 timeout 이 발생할 경우 처리는? 
	// timeout은 메시지 버퍼 비우기, disconnect event는 .. 재접속 처리 필수!!!!


	while (1) //오픈에 성공한 경우 sendCommand()를 통해 계속적으로 데이터를 전송한다. 전송에 실패 할 경우 failed 메시지를 출력한다.
	{
		char pBuff[13] = { 0, };
		if (!readCommand(pBuff, 12))
		{
			boost::this_thread::sleep(boost::posix_time::milliseconds(1));
			continue;
		}

		std::cout << pBuff << " - " <<std::endl;

		oTE.SetImpact(pBuff);
		oTE.CalCrossCircleFix(0, 1);
		oTE.CalCrossCircleFix(0, 2);
		oTE.CalCrossCircleFix(1, 2);

		bool bRet = oTE.CheckCross();
		if (bRet == false)
		{
			cout << "not enough impact point.."<<std::endl;
			std::this_thread::sleep_for(std::chrono::seconds(1));
			continue;
		}

		CPoint hPoint;
		// 저장된 세개의 교점의 무게중심 (타격 지점)을 구함.
		bRet = oTE.HitPoint(hPoint);

		cout << "Hit Point : " << hPoint.x << ", " << hPoint.y << endl;

		oTE.ClearImpact();

		std::this_thread::sleep_for(std::chrono::seconds(1));

		//else
		//	boost::this_thread::sleep(boost::posix_time::milliseconds(1));

			//std::cout << "try read buff" << std::endl;
	}

}