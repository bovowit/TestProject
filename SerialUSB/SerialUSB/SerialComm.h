#pragma once

#include "serialport.h"

#define RETURN_SUCCESS 1
#define RETURN_FAIL 0


class CSerialComm
{
public:
	CSerialPort	m_SerialPort;
	bool m_bConnected;
	string m_sPort;

public:
	CSerialComm();
	~CSerialComm();

	int		connect(char* _portNum);
	int		TryConnect(char* _portNum);

	int		sendCommand(char pos);
	void	disconnect();
	bool	readCommand(char * pBuff);

	void SerialCommRun();
};
