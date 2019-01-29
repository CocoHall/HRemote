#pragma once
#include "pch.h"
#include <stdio.h>
#include <windows.h>
#include <tlhelp32.h>
#include <string>
#include <tchar.h>
#include <Psapi.h>
#include "mStruct.cpp"
#include "ThreadSafeQueue.cpp"
#include "tool.cpp"
#pragma comment (lib,"Psapi.lib")

using namespace std;

class processManager {
public:
	static ThreadSafeQueue<mprocess>processQueue;

	void getProcessList(){
		HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (INVALID_HANDLE_VALUE == hSnapshot)
		{
			return ;
		}
		PROCESSENTRY32 pe = { 0 };
		pe.dwSize = sizeof(PROCESSENTRY32);

		BOOL fOk;
		for (fOk = Process32First(hSnapshot, &pe); fOk; fOk = Process32Next(hSnapshot, &pe))
		{
			TCHAR szProcessName[MAX_PATH] = { 0 };
			GetProcessFullPath(pe.th32ProcessID, szProcessName);
		}
		return ;
	}
	BOOL KillProcessById(DWORD pID)
	{
		HANDLE Hwnd;
		bool ret = FALSE;
		Hwnd = OpenProcess(PROCESS_TERMINATE | SYNCHRONIZE, 0, pID);
		if (Hwnd)
		{
			if (TerminateProcess(Hwnd, 0))
			{
				ret = true;
			}
		}
		return ret;
	}


private:
	BOOL DosPathToNtPath(LPTSTR pszDosPath, LPTSTR pszNtPath)
	{
		TCHAR            szDriveStr[500];
		TCHAR            szDrive[3];
		TCHAR            szDevName[100];
		INT                cchDevName;
		INT                i;

		//检查参数
		if (!pszDosPath || !pszNtPath)
			return FALSE;

		//获取本地磁盘字符串
		if (GetLogicalDriveStrings(sizeof(szDriveStr), szDriveStr))
		{
			for (i = 0; szDriveStr[i]; i += 4)
			{
				if (!lstrcmpi(&(szDriveStr[i]), _T("A:\\")) || !lstrcmpi(&(szDriveStr[i]), _T("B:\\")))
					continue;

				szDrive[0] = szDriveStr[i];
				szDrive[1] = szDriveStr[i + 1];
				szDrive[2] = '\0';
				if (!QueryDosDevice(szDrive, szDevName, 100))//查询 Dos 设备名
					return FALSE;

				cchDevName = lstrlen(szDevName);
				if (_tcsnicmp(pszDosPath, szDevName, cchDevName) == 0)//命中
				{
					lstrcpy(pszNtPath, szDrive);//复制驱动器
					lstrcat(pszNtPath, pszDosPath + cchDevName);//复制路径

					return TRUE;
				}
			}
		}

		lstrcpy(pszNtPath, pszDosPath);

		return FALSE;
	}
	//获取进程完整路径
	BOOL GetProcessFullPath(DWORD dwPID, TCHAR pszFullPath[MAX_PATH])
	{
		TCHAR        szImagePath[MAX_PATH];
		HANDLE        hProcess;
		if (!pszFullPath)
			return FALSE;

		pszFullPath[0] = '\0';
		hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, 0, dwPID);
		if (!hProcess)
			return FALSE;

		if (!GetProcessImageFileName(hProcess, szImagePath, MAX_PATH))
		{
			CloseHandle(hProcess);
			return FALSE;
		}

		if (!DosPathToNtPath(szImagePath, pszFullPath))
		{
			CloseHandle(hProcess);
			return FALSE;
		}

		CloseHandle(hProcess);
		mprocess tmp;
		

		tmp.pID = dwPID;

		//if (tool::is_str_utf8(pszFullPath)) {
		//	tmp.pName = pszFullPath;
		//}
		//else {
			tmp.pName = tool::GBKToUTF8(pszFullPath);
		//}
		processQueue.push(tmp);
		//_tprintf(_T("%d,%s \r\n"), dwPID, pszFullPath);
		return TRUE;
	}


};

