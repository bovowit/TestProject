#include "stdafx.h"
#include "serialcomm.h"
#include <iostream>
#include <boost/thread/thread.hpp>

CSerialComm::CSerialComm() {}
CSerialComm::~CSerialComm() {}


int CSerialComm::connect(char* portNum)
{
	if (!serial.OpenPort(portNum)) //포트를 오픈하고 오픈에 실패하였으면 fail을 반환한다.
		return RETURN_FAIL;

	serial.ConfigurePort(CBR_9600, 8, FALSE, NOPARITY, ONESTOPBIT); //포트 기본값을 설정한다.
	serial.SetCommunicationTimeouts(0, 0, 0, 0, 0); //Timeout값 설정

	return RETURN_SUCCESS;
}


int CSerialComm::sendCommand(char pos) //데이터를 전송하는 함수
{
	if (serial.WriteByte(pos)) //WriteByte()를 통해 데이터 전송에 성공하면 SUCCESS, 실패하면 FAIL을 반환한다.
		return RETURN_SUCCESS;
	else
		return RETURN_FAIL;
}

void CSerialComm::disconnect() //포트를 다 쓰고 난뒤 닫는 함수
{
	serial.ClosePort();
}

bool CSerialComm::readCommand(char * pBuff)
{
	BYTE byte;
	if (serial.ReadByte(byte))
		memcpy(pBuff, &byte, 1);
	else
		return false;


}

void CSerialComm::SerialCommRun()
{
	if (!connect("COM3")) //COM25번의 포트를 오픈한다. 실패할 경우 -1을 반환한다.
	{
		std::cout << "connect faliled" << std::endl;
	}
	else
		std::cout << "connect successed" << std::endl;


	while (1) //오픈에 성공한 경우 sendCommand()를 통해 계속적으로 데이터를 전송한다. 전송에 실패 할 경우 failed 메시지를 출력한다.
	{
		char pBuff[10] = { 0, };
		if(readCommand(pBuff))
			std::cout << pBuff;
		else
			boost::this_thread::sleep(boost::posix_time::milliseconds(1));
	}

}