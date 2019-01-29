#pragma once
#include "pch.h"

#include <stdio.h>

#include "ThreadSafeQueue.cpp"
#include <WINDOWS.H>
#include "netmanager.h"
#include "tool.cpp"
#include <iostream>  
extern netManager* nm;
//#include "tool.cpp"
using namespace std;
class commandManager {
public :
	int state;//0 表示未运行cmd 1表示已经有cmd程序运行了
	static ThreadSafeQueue<string> cmdQueue;
	HANDLE  hStdInRead;         //子进程用的stdin的读入端  
	HANDLE  hStdInWrite;        //主程序用的stdin的读入端  

	//定义句柄: 构成stdout管道的两端句柄  
	HANDLE  hStdOutRead;     ///主程序用的stdout的读入端  
	HANDLE  hStdOutWrite;    ///子进程用的stdout的写入端  

	//定义一个用于产生子进程的STARTUPINFO结构体 （定义见CreateProcess,函数说明）  
	STARTUPINFO siStartInfo;
	//定义一个用于产生子进程的PROCESS_INFORMATION结构体 （定义见CreateProcess,函数说明）  
	PROCESS_INFORMATION piProcInfo;

	void startCMD() {
		//产生一个用于stdin的管道，得到两个HANDLE:  hStdInRead用于子进程读出数据，hStdInWrite用于主程序写入数据  
		//其中saAttr是一个STARTUPINFO结构体，定义见CreatePipe函数说明  
		SECURITY_ATTRIBUTES saAttr = { 0 };
		saAttr.nLength = sizeof(saAttr);
		saAttr.bInheritHandle = TRUE;
		if (!CreatePipe(&hStdInRead, &hStdInWrite, &saAttr, 0))
			return;

		//产生一个用于stdout的管道，得到两个HANDLE:  hStdInRead用于主程序读出数据，hStdInWrite用于子程序写入数据  
		if (!CreatePipe(&hStdOutRead, &hStdOutWrite, &saAttr, 0))
			return;

		//对STARTUPINFO结构体赋值，对stdin,stdout的Handle设置为刚才得到的管道HANDLE  
		ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
		siStartInfo.cb = sizeof(STARTUPINFO);
		siStartInfo.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;	//允许设置showwindow和设置新进程的输入输出句柄参数
		siStartInfo.hStdOutput = hStdOutWrite;     //意思是：子进程的stdout输出到hStdOutWrite  
		siStartInfo.hStdError = hStdOutWrite;        //意思是：子进程的stderr输出到hStdErrWrite  
		siStartInfo.hStdInput = hStdInRead;

		//CreateProcess()中使用了CREATE_NEW_CONSOLE后, 可以去掉下面这两句注释
		//siStartInfo.wShowWindow = SW_SHOW;
		//siStartInfo.lpTitle = "新进程标题";

		//TCHAR CommandLine[] = _T("/c ping 127.0.0.1 \r\n");	//
		// 产生子进程，具体参数说明见CreateProcess函数  
		BOOL bSuccess = CreateProcess("C:\\Windows\\System32\\cmd.exe",
			NULL,//CommandLine,    // 子进程的命令行  这里加了内容的话，管道里的东西就不生效了
			NULL,                   // process security attributes  
			NULL,                   // primary thread security attributes  
			TRUE,                   // handles are inherited  
			CREATE_NO_WINDOW,       // creation flags  如果想新进程显示窗口, 可以设置CREATE_NEW_CONSOLE
			NULL,                  // use parent's environment  
			NULL,                  // use parent's current directory  
			&siStartInfo,      // STARTUPINFO pointer  
			&piProcInfo);     // receives PROCESS_INFORMATION  

		//如果失败，退出  
		if (!bSuccess) return;

		state = 1;
		thread t1(std::bind(&commandManager::detect, this));
		t1.detach();
	}

	void stopCMD() {
		
		if (state > 0) {
			state = 0;
			::CloseHandle(hStdOutWrite);
			::CloseHandle(hStdInRead);
			::CloseHandle(hStdOutRead);
			::CloseHandle(hStdInWrite);
			::CloseHandle(piProcInfo.hProcess);
			::CloseHandle(piProcInfo.hThread);
		}


	}
	commandManager() {
		state = 0;

	}

	void detect() {
		DWORD process_exit_code = 0;
		while (state && GetExitCodeProcess(piProcInfo.hProcess, &process_exit_code))
		{
				//读stdout,stderr  
			ReadFromPipe();
			//如果子进程结束，退出循环  
			if (process_exit_code != STILL_ACTIVE) break;
		
		}
	}

	BOOL WriteToPipe(string s)
	{
		// 本例子中这样写入参数  没有用
		if (s.length() <= 0) {
			s = "\r\n";
		}
		DWORD dwWritten;
		BOOL bSuccess = FALSE;
		//CHAR in_buffer[100] = "whoami \r\n";
		//用WriteFile，从hStdInWrite写入数据，数据在in_buffer中，长度为dwSize  
		//std::cout << "输入的内容:" <<s<< std::endl;
		bSuccess = WriteFile(hStdInWrite, s.c_str(), strlen(s.c_str()), &dwWritten, NULL);

		return bSuccess;
	}

	BOOL ReadFromPipe() 
	{
		char out_buffer[4096] = { 0 };
		DWORD dwRead;
		BOOL bSuccess = FALSE;

		//用WriteFile，从hStdOutRead读出子进程stdout输出的数据，数据结果在out_buffer中，长度为dwRead  
		bSuccess = ReadFile(hStdOutRead, out_buffer, 4096, &dwRead, NULL);
		if ((bSuccess) && (dwRead != 0))  //如果成功了，且长度>0  
		{
			// 此处加入你自己的代码  
			// 比如：将数据写入文件或显示到窗口中  
			//cout << "输出的内容:" << out_buffer << endl;
			//if (tool::is_str_utf8(out_buffer)) {
			//	cmdQueue.push(out_buffer);
			//}
			//else {
				cmdQueue.push(tool::GBKToUTF8(out_buffer));
			//}
			nm->addOrder();
			//cout << endl << "----得到的消息是---" << endl << out_buffer;
		}
		return bSuccess;
	}

};

