#pragma once
#include "pch.h"
#include <string> 
#include <windows.h>
#include<list>
#include <iostream>  
#include <io.h>  
#include"mStruct.cpp"
#include "ThreadSafeQueue.cpp"
#include "tool.cpp"
using namespace std;



/*
#define DRIVE_UNKNOWN     0		δ֪�豸
#define DRIVE_NO_ROOT_DIR 1		�����ڸ�Ŀ¼
#define DRIVE_REMOVABLE   2		U��
#define DRIVE_FIXED       3		Ӳ��
#define DRIVE_REMOTE      4		�������
#define DRIVE_CDROM       5		����
#define DRIVE_RAMDISK     6		����RAM



#define _A_NORMAL 0x00 // Normal file - No read/write restrictions	0
#define _A_RDONLY 0x01 // Read only file							1
#define _A_HIDDEN 0x02 // Hidden file								2
#define _A_SYSTEM 0x04 // System file								4
#define _A_SUBDIR 0x10 // Subdirectory								16
#define _A_ARCH   0x20 // Archive file								32	



*/


class fileManager {
public:
	static ThreadSafeQueue<mfile> fileQueue;
	//·��ĩβû�� //
	void getPath(string path) {
		path.erase(0, path.find_first_not_of(" "));
		path.erase(path.find_last_not_of(" ") + 1);
		path.erase(path.find_last_not_of("/") + 1);
		path.erase(path.find_last_not_of("\\") + 1);
		if (path.length() == 0) {
			getRoot();
		}
		else {
			getDirsAndFiles(path);
		}
	}

	bool fileDelete(string fullPath) {
		//cout << fullPath << endl;
		WIN32_FIND_DATAA FindFileData;
		FindFirstFileA(fullPath.c_str(), &FindFileData);

		if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			//���ļ���

			//LPCTSTR lpszPath = fullPath.data();
			return RemoveDir(fullPath);

		}
		else
		{
			return !remove(fullPath.c_str());
		}
		
	}

	bool fileMV(string oldPath,string oldName,string newPath,string newName) {
		oldPath.erase(0, oldPath.find_first_not_of(" "));
		oldPath.erase(oldPath.find_last_not_of(" ") + 1);
		oldPath.erase(oldPath.find_last_not_of("/") + 1);
		oldPath.erase(oldPath.find_last_not_of("\\") + 1);

		newPath.erase(0, newPath.find_first_not_of(" "));
		newPath.erase(newPath.find_last_not_of(" ") + 1);
		newPath.erase(newPath.find_last_not_of("/") + 1);
		newPath.erase(newPath.find_last_not_of("\\") + 1);

		string oldFullPath = oldPath + "\\" + oldName;
		string newFullPath = newPath + "\\" + newName;

		return !rename(oldFullPath.c_str(),newFullPath.c_str());

	
	}

	bool fileCopy(string oldPath, string oldName, string newPath, string newName) {
		oldPath.erase(0, oldPath.find_first_not_of(" "));
		oldPath.erase(oldPath.find_last_not_of(" ") + 1);
		oldPath.erase(oldPath.find_last_not_of("/") + 1);
		oldPath.erase(oldPath.find_last_not_of("\\") + 1);

		newPath.erase(0, newPath.find_first_not_of(" "));
		newPath.erase(newPath.find_last_not_of(" ") + 1);
		newPath.erase(newPath.find_last_not_of("/") + 1);
		newPath.erase(newPath.find_last_not_of("\\") + 1);

		string oldFullPath = oldPath + "\\" + oldName;
		string newFullPath = newPath + "\\" + newName;

		return CopyFile(oldFullPath.c_str(), newFullPath.c_str(),false);//false ������


	}

private:
	void getRoot() {

		char DStr[MAX_PATH] = { 0 };
		GetLogicalDriveStrings(MAX_PATH, (LPTSTR)DStr);
		//ͨ��GetLogicalDriveStrings���ַ�����Ϣ���Ƶ�����������,���б�������������������Ϣ��

		for (int i = 0; i < sizeof(DStr); i++) {


			if ((DStr[i] >= 'a' && DStr[i] <= 'z') || (DStr[i] >= 'A' && DStr[i] <= 'Z')) {
				char tmpDir[4];
				tmpDir[0] = DStr[i];
				tmpDir[1] = ':';
				tmpDir[2] = '/';
				tmpDir[3] = 0x00;
				int DType = GetDriveType(tmpDir);
				unsigned _int64 i64FreeBytesToCaller;
				unsigned _int64 i64TotalBytes;
				unsigned _int64 i64FreeBytes;
				GetDiskFreeSpaceEx(
					tmpDir,
					(PULARGE_INTEGER)&i64FreeBytesToCaller,
					(PULARGE_INTEGER)&i64TotalBytes,
					(PULARGE_INTEGER)&i64FreeBytes);
				//cout << tmpDir << "  " << i64TotalBytes << "   " << i64FreeBytes << endl;
				mfile tmpFile;
				tmpFile.diskType = DType;
				tmpFile.isRoot = true;
				tmpFile.fileName = tmpDir;
				tmpFile.fileTotalSize = i64TotalBytes;
				tmpFile.fileFreeSize = i64FreeBytes;

				fileQueue.push(tmpFile);

			}
		}

	}
	
	void getDirsAndFiles(string path) {
		//�ļ����
		long hFile = 0;
		//�ļ���Ϣ
		struct _finddata_t fileinfo;  //�����õ��ļ���Ϣ��ȡ�ṹ
		string p;  //string�������˼��һ����ֵ����:assign()���кܶ����ذ汾
		if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1) {
			do {
				//if ((fileinfo.attrib & _A_SUBDIR)) {  //�Ƚ��ļ������Ƿ����ļ���
					if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0) {
						mfile tmpFile;
						
						//if (tool::is_str_utf8(fileinfo.name)) {
						//	tmpFile.fileName = fileinfo.name;
						//	cout <<"isutf8"<< fileinfo.name << endl;
						//}
						//else {
							tmpFile.fileName = tool::GBKToUTF8(fileinfo.name);					
						//}

						tmpFile.isRoot = false;
						tmpFile.attrib = fileinfo.attrib;
						//cout << fileinfo.attrib << endl;
						tmpFile.size = fileinfo.size;
						tmpFile.time_write = fileinfo.time_write;
						tmpFile.time_create = fileinfo.time_create;
						tmpFile.time_access = fileinfo.time_access;
						fileQueue.push(tmpFile);

					}
				//}
			} while (_findnext(hFile, &fileinfo) == 0);  //Ѱ����һ�����ɹ�����0������-1
			_findclose(hFile);
		}
	}

	bool RemoveDir(string strDir)
	{
		strDir.erase(0, strDir.find_first_not_of(" "));
		strDir.erase(strDir.find_last_not_of(" ") + 1);
		strDir.erase(strDir.find_last_not_of("/") + 1);
		strDir.erase(strDir.find_last_not_of("\\") + 1);
		strDir += '\\';

		WIN32_FIND_DATA wfd;
		HANDLE hFind = FindFirstFile((strDir + "*.*").c_str(), &wfd);
		if (hFind == INVALID_HANDLE_VALUE)
			return false;
		do
		{
			if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if (stricmp(wfd.cFileName, ".") != 0 &&
					stricmp(wfd.cFileName, "..") != 0)
					RemoveDir((strDir + wfd.cFileName).c_str());
			}
			else
			{
				DeleteFile((strDir + wfd.cFileName).c_str());
			}
		} while (FindNextFile(hFind, &wfd));
		FindClose(hFind);
		RemoveDirectory(strDir.c_str());
		return true;
	}
};