#pragma once

#include"pch.h"

#include <string> 
#include <windows.h>
#include <mutex>
#include <time.h>
#include "ThreadSafeQueue.cpp"

using namespace std;




class netManager {
private:
	mutex mut;
	string remoteIP;
	int remotePort;
	SOCKET* sock;
	void mySend();


public:
	bool state = false;//成功连接的标志
	__time64_t lastTime;
	ThreadSafeQueue<string> sendfileQueue;

	netManager(string ip, int port, SOCKET* s);

	~netManager();

	int init();

	void start();
    void sendState();

	void finishNet();
	void myRecv();
	void addOrder();

};

















