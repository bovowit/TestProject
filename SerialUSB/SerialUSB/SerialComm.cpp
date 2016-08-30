#include "stdafx.h"
#include "serialcomm.h"
#include <iostream>
#include <boost/thread/thread.hpp>

CSerialComm::CSerialComm()
{
	m_bConnected = false;

}
CSerialComm::~CSerialComm() {}


int CSerialComm::connect(string portNum)
{
	m_sPort = portNum;	// ��Ʈ �ѹ� ����

	if (!m_SerialPort.OpenPort(m_sPort)) //��Ʈ�� �����ϰ� ���¿� �����Ͽ����� fail�� ��ȯ�Ѵ�.
		return RETURN_FAIL;

	m_SerialPort.ConfigurePort(CBR_9600, 8, FALSE, NOPARITY, ONESTOPBIT); //��Ʈ �⺻���� �����Ѵ�.
	m_SerialPort.SetCommunicationTimeouts(0, 0, 100, 0, 0); //Timeout�� ����, ����° ������ �ѹ��� ��Ŷ�� 100 msec ��ŭ ��ٸ�.

	m_bConnected = true;
	return RETURN_SUCCESS;
}

int	CSerialComm::TryConnect(string _portNum)	// ����ɶ����� 10�� ���� �ݺ�.
{
	int iRet = RETURN_FAIL;
	int iRetryCnt = 0;
	while (!m_bConnected && iRetryCnt < 3)
	{
		iRet = connect(_portNum);
		boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
		iRetryCnt++;
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

bool CSerialComm::readCommand(char * pBuff)
{
	BYTE byte;
	if (m_SerialPort.ReadByte(byte))
		memcpy(pBuff, &byte, 1);
	else
		return false;


}

void CSerialComm::SerialCommRun()
{
	string sComPort = "COM4";
	if (!TryConnect(sComPort)) //COM3 ���� ��Ʈ�� �����Ѵ�. 10�� ���� �ݺ� -> ���� �ݺ�?????
	{
		std::cout << "connect faliled" << std::endl;
	}
	else
		std::cout << "connect successed" << std::endl;

	// reading�� ��ӵ� �����ŭ �ݺ��б�.. ReadByte(BYTE* &resp, UINT size) ���.
	// disconnect event�� timeout �� �߻��� ��� ó����? 
	// timeout�� �޽��� ���� ����, disconnect event�� .. ������ ó�� �ʼ�!!!!


	while (1) //���¿� ������ ��� sendCommand()�� ���� ��������� �����͸� �����Ѵ�. ���ۿ� ���� �� ��� failed �޽����� ����Ѵ�.
	{
		char pBuff[10] = { 0, };
		if(readCommand(pBuff))
			std::cout << pBuff;
		else
			boost::this_thread::sleep(boost::posix_time::milliseconds(1));

			//std::cout << "try read buff" << std::endl;
	}

}