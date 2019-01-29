#pragma once
#include"pch.h"
#include <string>
#include <windows.h>
#include "ThreadSafeQueue.cpp"

using namespace std;

typedef struct FileStruct {
	
	bool isRoot;			//0-0
	long long fileTotalSize;//1-8
	long long fileFreeSize;//9-16
	int diskType;			//17-20

	unsigned    attrib;		//21-24
	__time64_t  time_create;    // -1 for FAT file systems		//25-32
	__time64_t  time_access;    // -1 for FAT file systems		//33-40
	__time64_t  time_write;									    //41-48
	_fsize_t    size;			//								//
	string fileName;

}mfile;

typedef struct ProcessStruct {
	string pName;
	DWORD pID;
}mprocess;

typedef struct InfoStruct {
	int infoIndex;
	string data;
}minfo;

typedef struct ScreenStruct {
	unsigned long  bmpZipSize;//ѹ����Ĵ�С
	unsigned long  bmpSize;//ԭͼ�Ĵ�С
	byte *bmpZipMem;//ѹ�����ͼƬ����

}mscreen;

typedef struct VideoStruct {
	unsigned long  bmpZipSize;//ѹ����Ĵ�С
	unsigned long  bmpSize;//ԭͼ�Ĵ�С
	byte *bmpZipMem;//ѹ�����ͼƬ����

}mvideo;

typedef struct VoiceStruct {
	unsigned long  voiceZipSize;//��Ƶѹ����Ĵ�С
	unsigned long voiceSize;//��Ƶԭʼ��С
	byte *voiceZipMem;//ѹ����1�����Ƶ����

}mvoice;