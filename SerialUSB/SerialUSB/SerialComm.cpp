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
	m_sPort = portNum;	// 포트 넘버 저장

	if (!m_SerialPort.OpenPort(m_sPort)) //포트를 오픈하고 오픈에 실패하였으면 fail을 반환한다.
		return RETURN_FAIL;

	m_SerialPort.ConfigurePort(CBR_9600, 8, FALSE, NOPARITY, ONESTOPBIT); //포트 기본값을 설정한다.
	m_SerialPort.SetCommunicationTimeouts(0, 0, 100, 0, 0); //Timeout값 설정, 세번째 값으로 한번의 패킷을 100 msec 만큼 기다림.

	m_bConnected = true;
	return RETURN_SUCCESS;
}

int	CSerialComm::TryConnect(string _portNum)	// 연결될때까지 10초 동안 반복.
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
	if (!TryConnect(sComPort)) //COM3 번의 포트를 오픈한다. 10초 동안 반복 -> 무한 반복?????
	{
		std::cout << "connect faliled" << std::endl;
	}
	else
		std::cout << "connect successed" << std::endl;

	// reading은 약속된 사이즈만큼 반복읽기.. ReadByte(BYTE* &resp, UINT size) 사용.
	// disconnect event나 timeout 이 발생할 경우 처리는? 
	// timeout은 메시지 버퍼 비우기, disconnect event는 .. 재접속 처리 필수!!!!


	while (1) //오픈에 성공한 경우 sendCommand()를 통해 계속적으로 데이터를 전송한다. 전송에 실패 할 경우 failed 메시지를 출력한다.
	{
		char pBuff[10] = { 0, };
		if(readCommand(pBuff))
			std::cout << pBuff;
		else
			boost::this_thread::sleep(boost::posix_time::milliseconds(1));

			//std::cout << "try read buff" << std::endl;
	}

}