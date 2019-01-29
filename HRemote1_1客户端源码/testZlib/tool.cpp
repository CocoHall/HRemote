#pragma once
#include "pch.h"
#include <string>
#include <windows.h>
using namespace std;
class tool {
public:
	static HANDLE h;
	static 	string GBKToUTF8(char charGBK[])
	{
		string strOutUTF8 = "";
		WCHAR * str1;
		int n = MultiByteToWideChar(CP_ACP, 0, charGBK, -1, NULL, 0);
		str1 = new WCHAR[n];
		MultiByteToWideChar(CP_ACP, 0, charGBK, -1, str1, n);
		n = WideCharToMultiByte(CP_UTF8, 0, str1, -1, NULL, 0, NULL, NULL);
		char * str2 = new char[n];
		WideCharToMultiByte(CP_UTF8, 0, str1, -1, str2, n, NULL, NULL);
		strOutUTF8 = str2;
		delete str1;
		str1 = NULL;
		delete str2;
		str2 = NULL;
		return strOutUTF8;
	}

	//static bool is_str_utf8(const char* str)
	//{
	//	unsigned int nBytes = 0;//UFT8����1-6���ֽڱ���,ASCII��һ���ֽ�
	//	unsigned char chr = *str;
	//	bool bAllAscii = true;
	//	for (unsigned int i = 0; str[i] != '\0'; ++i) {
	//		chr = *(str + i);
	//		//�ж��Ƿ�ASCII����,�������,˵���п�����UTF8,ASCII��7λ����,���λ���Ϊ0,0xxxxxxx
	//		if (nBytes == 0 && (chr & 0x80) != 0) {
	//			bAllAscii = false;
	//		}
	//		if (nBytes == 0) {
	//			//�������ASCII��,Ӧ���Ƕ��ֽڷ�,�����ֽ���
	//			if (chr >= 0x80) {
	//				if (chr >= 0xFC && chr <= 0xFD) {
	//					nBytes = 6;
	//				}
	//				else if (chr >= 0xF8) {
	//					nBytes = 5;
	//				}
	//				else if (chr >= 0xF0) {
	//					nBytes = 4;
	//				}
	//				else if (chr >= 0xE0) {
	//					nBytes = 3;
	//				}
	//				else if (chr >= 0xC0) {
	//					nBytes = 2;
	//				}
	//				else {
	//					return false;
	//				}
	//				nBytes--;
	//			}
	//		}
	//		else {
	//			//���ֽڷ��ķ����ֽ�,ӦΪ 10xxxxxx
	//			if ((chr & 0xC0) != 0x80) {
	//				return false;
	//			}
	//			//����Ϊ��Ϊֹ
	//			nBytes--;
	//		}
	//	}
	//	//Υ��UTF8�������
	//	if (nBytes != 0) {
	//		return false;
	//	}
	//	if (bAllAscii) { //���ȫ������ASCII, Ҳ��UTF8
	//		return true;
	//	}
	//	return true;
	//}

	static string UTF8ToGBK(const char* strUTF8)
	{
		int len = MultiByteToWideChar(CP_UTF8, 0, strUTF8, -1, NULL, 0);
		wchar_t* wszGBK = new wchar_t[len + 1];
		memset(wszGBK, 0, len * 2 + 2);
		MultiByteToWideChar(CP_UTF8, 0, strUTF8, -1, wszGBK, len);
		len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
		char* szGBK = new char[len + 1];
		memset(szGBK, 0, len + 1);
		WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, szGBK, len, NULL, NULL);
		string strTemp(szGBK);
		if (wszGBK) delete wszGBK;
		if (szGBK) delete szGBK;
		return strTemp;
	}

};