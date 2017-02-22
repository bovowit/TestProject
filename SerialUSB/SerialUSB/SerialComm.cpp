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
	m_sPort = portNum;	// ��Ʈ �ѹ� ����

	if (!m_SerialPort.OpenPort(m_sPort)) //��Ʈ�� �����ϰ� ���¿� �����Ͽ����� fail�� ��ȯ�Ѵ�.
		return RETURN_FAIL;

	m_SerialPort.ConfigurePort(m_lBaudRate, 8, FALSE, NOPARITY, ONESTOPBIT); //��Ʈ �⺻���� �����Ѵ�.
	m_SerialPort.SetCommunicationTimeouts(0, 0, 100, 0, 0); //Timeout�� ����, ����° ������ �ѹ��� ��Ŷ�� 100 msec ��ŭ ��ٸ�.

	return RETURN_SUCCESS;
}

int	CSerialComm::TryConnect(string _portNum)	// ����ɶ����� 10�� ���� �ݺ�.
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

int	CSerialComm::TryConnectSimple(string _portNum)	// ����ɶ����� 10�� ���� �ݺ�.
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

int CSerialComm::sendCommand(char pos) //�����͸� �����ϴ� �Լ�
{
	if (m_SerialPort.WriteByte(pos)) //WriteByte()�� ���� ������ ���ۿ� �����ϸ� SUCCESS, �����ϸ� FAIL�� ��ȯ�Ѵ�.
		return RETURN_SUCCESS;
	else
		return RETURN_FAIL;
}

void CSerialComm::disconnect() //��Ʈ�� �� ���� ���� �ݴ� �Լ�
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
	//for (int i = 2; i < 10; i++)	 //COM1~COM9 ���� ��Ʈ�� �����Ѵ�. 10�� ���� �ݺ� -> ���� �ݺ�?????
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

	// reading�� ��ӵ� �����ŭ �ݺ��б�.. ReadByte(BYTE* &resp, UINT size) ���.
	// disconnect event�� timeout �� �߻��� ��� ó����? 
	// timeout�� �޽��� ���� ����, disconnect event�� .. ������ ó�� �ʼ�!!!!


	while (1) //���¿� ������ ��� sendCommand()�� ���� ��������� �����͸� �����Ѵ�. ���ۿ� ���� �� ��� failed �޽����� ����Ѵ�.
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
		// ����� ������ ������ �����߽� (Ÿ�� ����)�� ����.
		bRet = oTE.HitPoint(hPoint);

		cout << "Hit Point : " << hPoint.x << ", " << hPoint.y << endl;

		oTE.ClearImpact();

		std::this_thread::sleep_for(std::chrono::seconds(1));

		//else
		//	boost::this_thread::sleep(boost::posix_time::milliseconds(1));

			//std::cout << "try read buff" << std::endl;
	}

}