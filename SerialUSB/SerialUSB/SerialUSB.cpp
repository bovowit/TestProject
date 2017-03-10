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

void func_serialthread(int iPort, boost::shared_ptr<CSerialComm> pSerialComm)
{
	//boost::unique_lock<boost::mutex> scoped_lock(io_mutex);
	std::cout << "Port == " << iPort << std::endl;
	pSerialComm->SerialCommRun(iPort);
}

void second_thread()
{
	while (1)
	{
		//boost::unique_lock<boost::mutex> scoped_lock(io_mutex);

		//std::cout << "second thread called == " << std::endl;
		boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
	}
}

int main()
{
	char buffer;
	CSerialComm serialComm; //SerialComm ��ü ����

	int iPort = 8;
	boost::shared_ptr<CSerialComm> pSComm(new CSerialComm);

	//boost::thread_group threads;
	////for (int i = 0; i < 1; ++i)
	//threads.create_thread(boost::bind(func_serialthread, boost::cref(icnt), boost::cref(pCount)));
	//threads.create_thread(boost::bind(second_thread));
	////thrds.interrupt_all();
	//threads.join_all();

	boost::thread thread1(boost::bind(func_serialthread, boost::cref(iPort), boost::cref(pSComm)));
	boost::thread thread2(second_thread);
	thread1.join();
	thread2.join();


	//thrds.create_thread(boost::bind(second_thread));
	
	//thrds.interrupt_all();
	//thrds.join_all();


	std::cout << "wainting..." << std::endl;
	boost::this_thread::sleep(boost::posix_time::milliseconds(1000));

	

	serialComm.disconnect(); //�۾��� ������ ��Ʈ�� �ݴ´�
	cout << "dusconnect and end application" << endl;
	boost::this_thread::sleep(boost::posix_time::seconds(10000));
	return 0;

}