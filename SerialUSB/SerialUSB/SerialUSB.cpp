// SerialUSB.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <stdio.h>
#include <iostream>
#include <string>
#include "serialcomm.h"

#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>

boost::mutex io_mutex; // The iostreams are not guaranteed to be thread-safe!
using namespace std;

void func_serialthread(int a, boost::shared_ptr<CSerialComm> pSerialComm)
{

	boost::unique_lock<boost::mutex> scoped_lock(io_mutex);

	std::cout << "count == " << a << std::endl;

	pSerialComm->SerialCommRun();
}


int main()
{
	char buffer;
	CSerialComm serialComm; //SerialComm 객체 생성

	int icnt = 8;
	boost::shared_ptr<CSerialComm> pCount(new CSerialComm);

	boost::thread_group thrds;
	//for (int i = 0; i < 1; ++i)
	thrds.create_thread(boost::bind(func_serialthread, boost::cref(icnt), boost::cref(pCount)));
	thrds.join_all();

	std::cout << "wainting..." << std::endl;
	boost::this_thread::sleep(boost::posix_time::seconds(1000));




	//if (!serialComm.connect("COM3")) //COM25번의 포트를 오픈한다. 실패할 경우 -1을 반환한다.
	//{
	//	cout << "connect faliled" << endl;
	//	return -1;
	//}
	//else
	//	cout << "connect successed" << endl;


	//while (1) //오픈에 성공한 경우 sendCommand()를 통해 계속적으로 데이터를 전송한다. 전송에 실패 할 경우 failed 메시지를 출력한다.
	//{ 
	//	//cin >> buffer;
	//	//if (!serialComm.sendCommand(buffer))
	//	//{
	//	//	cout << "send command failed" << endl;
	//	//}
	//	//else
	//	//	cout << "send Command success" << endl;

	//	char pBuff[10] = { 0, };
	//	serialComm.readCommand(pBuff);
	//	cout << pBuff;
	//}


	serialComm.disconnect(); //작업이 끝나면 포트를 닫는다
	cout << "end connect" << endl;

	return 0;

}