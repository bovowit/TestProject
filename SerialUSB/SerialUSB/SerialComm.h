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
	long m_lBaudRate; // Baud rates at which the communication device operates

public:
	CSerialComm();
	~CSerialComm();

	int		connect(string _portNum);
	int		TryConnect(string _portNum);
	int		TryConnectSimple(string _portNum);

	int		sendCommand(char pos);
	void	disconnect();
	bool	readCommand(char * pBuff, UINT length);

	void SerialCommRun(int iPortNo);
};
