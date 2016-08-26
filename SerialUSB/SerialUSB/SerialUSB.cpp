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

void second_thread()
{
	while (1)
	{
		boost::unique_lock<boost::mutex> scoped_lock(io_mutex);
		std::cout << "second thread called == " << std::endl;
		boost::this_thread::sleep(boost::posix_time::milliseconds(100));
	}
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

	thrds.create_thread(boost::bind(second_thread));
	
	thrds.interrupt_all();
	thrds.join_all();

	std::cout << "wainting..." << std::endl;
	boost::this_thread::sleep(boost::posix_time::milliseconds(10));

	

	serialComm.disconnect(); //작업이 끝나면 포트를 닫는다
	cout << "dusconnect and end application" << endl;
	boost::this_thread::sleep(boost::posix_time::seconds(10000));
	return 0;

}