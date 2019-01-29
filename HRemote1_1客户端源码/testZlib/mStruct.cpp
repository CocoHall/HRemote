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
	unsigned long  bmpZipSize;//压缩后的大小
	unsigned long  bmpSize;//原图的大小
	byte *bmpZipMem;//压缩后的图片数据

}mscreen;

typedef struct VideoStruct {
	unsigned long  bmpZipSize;//压缩后的大小
	unsigned long  bmpSize;//原图的大小
	byte *bmpZipMem;//压缩后的图片数据

}mvideo;

typedef struct VoiceStruct {
	unsigned long  voiceZipSize;//音频压缩后的大小
	unsigned long voiceSize;//音频原始大小
	byte *voiceZipMem;//压缩后1秒的音频数据

}mvoice;