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
	int state;//0 ��ʾδ����cmd 1��ʾ�Ѿ���cmd����������
	static ThreadSafeQueue<string> cmdQueue;
	HANDLE  hStdInRead;         //�ӽ����õ�stdin�Ķ����  
	HANDLE  hStdInWrite;        //�������õ�stdin�Ķ����  

	//������: ����stdout�ܵ������˾��  
	HANDLE  hStdOutRead;     ///�������õ�stdout�Ķ����  
	HANDLE  hStdOutWrite;    ///�ӽ����õ�stdout��д���  

	//����һ�����ڲ����ӽ��̵�STARTUPINFO�ṹ�� �������CreateProcess,����˵����  
	STARTUPINFO siStartInfo;
	//����һ�����ڲ����ӽ��̵�PROCESS_INFORMATION�ṹ�� �������CreateProcess,����˵����  
	PROCESS_INFORMATION piProcInfo;

	void startCMD() {
		//����һ������stdin�Ĺܵ����õ�����HANDLE:  hStdInRead�����ӽ��̶������ݣ�hStdInWrite����������д������  
		//����saAttr��һ��STARTUPINFO�ṹ�壬�����CreatePipe����˵��  
		SECURITY_ATTRIBUTES saAttr = { 0 };
		saAttr.nLength = sizeof(saAttr);
		saAttr.bInheritHandle = TRUE;
		if (!CreatePipe(&hStdInRead, &hStdInWrite, &saAttr, 0))
			return;

		//����һ������stdout�Ĺܵ����õ�����HANDLE:  hStdInRead����������������ݣ�hStdInWrite�����ӳ���д������  
		if (!CreatePipe(&hStdOutRead, &hStdOutWrite, &saAttr, 0))
			return;

		//��STARTUPINFO�ṹ�帳ֵ����stdin,stdout��Handle����Ϊ�ղŵõ��Ĺܵ�HANDLE  
		ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
		siStartInfo.cb = sizeof(STARTUPINFO);
		siStartInfo.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;	//��������showwindow�������½��̵���������������
		siStartInfo.hStdOutput = hStdOutWrite;     //��˼�ǣ��ӽ��̵�stdout�����hStdOutWrite  
		siStartInfo.hStdError = hStdOutWrite;        //��˼�ǣ��ӽ��̵�stderr�����hStdErrWrite  
		siStartInfo.hStdInput = hStdInRead;

		//CreateProcess()��ʹ����CREATE_NEW_CONSOLE��, ����ȥ������������ע��
		//siStartInfo.wShowWindow = SW_SHOW;
		//siStartInfo.lpTitle = "�½��̱���";

		//TCHAR CommandLine[] = _T("/c ping 127.0.0.1 \r\n");	//
		// �����ӽ��̣��������˵����CreateProcess����  
		BOOL bSuccess = CreateProcess("C:\\Windows\\System32\\cmd.exe",
			NULL,//CommandLine,    // �ӽ��̵�������  ����������ݵĻ����ܵ���Ķ����Ͳ���Ч��
			NULL,                   // process security attributes  
			NULL,                   // primary thread security attributes  
			TRUE,                   // handles are inherited  
			CREATE_NO_WINDOW,       // creation flags  ������½�����ʾ����, ��������CREATE_NEW_CONSOLE
			NULL,                  // use parent's environment  
			NULL,                  // use parent's current directory  
			&siStartInfo,      // STARTUPINFO pointer  
			&piProcInfo);     // receives PROCESS_INFORMATION  

		//���ʧ�ܣ��˳�  
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
				//��stdout,stderr  
			ReadFromPipe();
			//����ӽ��̽������˳�ѭ��  
			if (process_exit_code != STILL_ACTIVE) break;
		
		}
	}

	BOOL WriteToPipe(string s)
	{
		// ������������д�����  û����
		if (s.length() <= 0) {
			s = "\r\n";
		}
		DWORD dwWritten;
		BOOL bSuccess = FALSE;
		//CHAR in_buffer[100] = "whoami \r\n";
		//��WriteFile����hStdInWriteд�����ݣ�������in_buffer�У�����ΪdwSize  
		//std::cout << "���������:" <<s<< std::endl;
		bSuccess = WriteFile(hStdInWrite, s.c_str(), strlen(s.c_str()), &dwWritten, NULL);

		return bSuccess;
	}

	BOOL ReadFromPipe() 
	{
		char out_buffer[4096] = { 0 };
		DWORD dwRead;
		BOOL bSuccess = FALSE;

		//��WriteFile����hStdOutRead�����ӽ���stdout��������ݣ����ݽ����out_buffer�У�����ΪdwRead  
		bSuccess = ReadFile(hStdOutRead, out_buffer, 4096, &dwRead, NULL);
		if ((bSuccess) && (dwRead != 0))  //����ɹ��ˣ��ҳ���>0  
		{
			// �˴��������Լ��Ĵ���  
			// ���磺������д���ļ�����ʾ��������  
			//cout << "���������:" << out_buffer << endl;
			//if (tool::is_str_utf8(out_buffer)) {
			//	cmdQueue.push(out_buffer);
			//}
			//else {
				cmdQueue.push(tool::GBKToUTF8(out_buffer));
			//}
			nm->addOrder();
			//cout << endl << "----�õ�����Ϣ��---" << endl << out_buffer;
		}
		return bSuccess;
	}

};

