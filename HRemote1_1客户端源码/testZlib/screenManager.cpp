#pragma once
#include "pch.h"
#include <string> 
#include<WINDOWSX.H>
#include<windows.h>
#include "mStruct.cpp"
#include "ThreadSafeQueue.cpp"
//ѹ��
#include "zlib/zlib.h"
#pragma comment(lib, "./zlib/zlibstat.lib")

class screenManager {
public:
	static ThreadSafeQueue<mscreen> screenQueue;

	void screenAndZlib() {
		byte* bmpMem = NULL;
		unsigned long bmpSize = 0;
		bmpMem = screen(bmpSize);//��Ҫ�ͷ�

		byte *bmpZipMem = new byte[bmpSize];
		unsigned long  bmpZipSize = bmpSize;
		compress(bmpZipMem, &bmpZipSize, bmpMem, bmpSize);
		mscreen tmp;
		tmp.bmpSize = bmpSize;
		tmp.bmpZipSize = bmpZipSize;
		tmp.bmpZipMem = bmpZipMem;
		screenQueue.push(tmp);
		delete bmpMem;
		bmpMem = nullptr;


	}

private:

	void bmpIntoMem(byte* bmpMem, BITMAPFILEHEADER bfh, BITMAPINFOHEADER bih) {

		int i = 0;
		unsigned long tmp = 0;
		for (i = 0, tmp = bfh.bfType; i < 2; i++) {
			bmpMem[i] = tmp % 0x100;
			tmp = tmp >> 8;
		}

		for (i = 0, tmp = bfh.bfSize; i < 4; i++) {
			bmpMem[i + 2] = tmp % 0x100;
			tmp = tmp >> 8;
		}

		for (i = 0, tmp = bfh.bfReserved1; i < 2; i++) {
			bmpMem[i + 6] = tmp % 0x100;
			tmp = tmp >> 8;
		}
		for (i = 0, tmp = bfh.bfReserved2; i < 2; i++) {
			bmpMem[i + 8] = tmp % 0x100;
			tmp = tmp >> 8;
		}


		for (i = 0, tmp = bfh.bfOffBits; i < 4; i++) {
			bmpMem[i + 10] = tmp % 0x100;
			tmp = tmp >> 8;
		}

		for (i = 0, tmp = bih.biSize; i < 4; i++) {
			bmpMem[i + 14] = tmp % 0x100;
			tmp = tmp >> 8;
		}

		for (i = 0, tmp = bih.biWidth; i < 4; i++) {
			bmpMem[i + 18] = tmp % 0x100;
			tmp = tmp >> 8;
		}

		for (i = 0, tmp = bih.biHeight; i < 4; i++) {
			bmpMem[i + 22] = tmp % 0x100;
			tmp = tmp >> 8;
		}
		for (i = 0, tmp = bih.biPlanes; i < 2; i++) {
			bmpMem[i + 26] = tmp % 0x100;
			tmp = tmp >> 8;
		}

		for (i = 0, tmp = bih.biBitCount; i < 2; i++) {
			bmpMem[i + 28] = tmp % 0x100;
			tmp = tmp >> 8;
		}
		for (i = 0, tmp = bih.biCompression; i < 4; i++) {
			bmpMem[i + 30] = tmp % 0x100;
			tmp = tmp >> 8;
		}

		for (i = 0, tmp = bih.biSizeImage; i < 4; i++) {
			bmpMem[i + 34] = tmp % 0x100;
			tmp = tmp >> 8;
		}

		for (i = 0, tmp = bih.biXPelsPerMeter; i < 4; i++) {
			bmpMem[i + 38] = tmp % 0x100;
			tmp = tmp >> 8;
		}
		for (i = 0, tmp = bih.biYPelsPerMeter; i < 4; i++) {
			bmpMem[i + 42] = tmp % 0x100;
			tmp = tmp >> 8;
		}

		for (i = 0, tmp = bih.biClrUsed; i < 4; i++) {
			bmpMem[i + 46] = tmp % 0x100;
			tmp = tmp >> 8;
		}
		for (i = 0, tmp = bih.biClrImportant; i < 4; i++) {
			bmpMem[i + 50] = tmp % 0x100;
			tmp = tmp >> 8;
		}



	}

	byte* screen(unsigned long &bmpsize) {
		//��ȡ��ǰ������ĻDC
		HDC hDC = GetDC(GetDesktopWindow());
		//CDC *pDC = CDC::FromHandle(hDC);

		//�����ɫģʽ
		int BitPerPixel = GetDeviceCaps(hDC, BITSPIXEL);
		int Width = GetDeviceCaps(hDC, HORZRES);
		int Height = GetDeviceCaps(hDC, VERTRES);

		//cout << "��ǰ��Ļɫ��ģʽΪ" << BitPerPixel << "λɫ��" << endl
		//	<< "��Ļ��ȣ�" << Width << endl
		//	<< "��Ļ�߶ȣ�" << Height << endl << endl;

		HDC memDC;//�ڴ�DC
		memDC = CreateCompatibleDC(hDC);

		HBITMAP memBitmap;
		HGDIOBJ oldmemBitmap;//��������Ļ���ݵ�bitmap
		memBitmap = CreateCompatibleBitmap(hDC, Width, Height);

		oldmemBitmap = SelectObject(memDC, memBitmap);//��memBitmapѡ���ڴ�DC
		BitBlt(memDC, 0, 0, Width, Height, hDC, 0, 0, SRCCOPY);//������Ļͼ���ڴ�DC

		//���´��뱣��memDC�е�λͼ���ļ�
		//BITMAP bmp;
		//GetBitmap(&bmp);//���λͼ��Ϣ

		BITMAP bm;
		GetObject(memBitmap, sizeof(BITMAP), (LPBYTE)&bm);

		BITMAPINFOHEADER bih = { 0 };//λͼ��Ϣͷ
		bih.biBitCount = bm.bmBitsPixel;//ÿ�������ֽڴ�С
		bih.biCompression = BI_RGB;
		bih.biHeight = bm.bmHeight;//�߶�
		bih.biPlanes = 1;
		bih.biSize = sizeof(BITMAPINFOHEADER);
		bih.biSizeImage = bm.bmWidthBytes * bm.bmHeight;//ͼ�����ݴ�С
		bih.biWidth = bm.bmWidth;//���

		BITMAPFILEHEADER bfh = { 0 };//λͼ�ļ�ͷ
		bfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);//��λͼ���ݵ�ƫ����
		bfh.bfSize = bfh.bfOffBits + bm.bmWidthBytes * bm.bmHeight;//�ļ��ܵĴ�С
		bfh.bfType = (WORD)0x4d42;


		byte * p = new byte[bm.bmWidthBytes * bm.bmHeight];//�����ڴ汣��λͼ����
		byte * bmpMem = new byte[bfh.bfSize];

		bmpIntoMem(bmpMem, bfh, bih);

		GetDIBits(memDC, (HBITMAP)memBitmap, 0, Height, p,
			(LPBITMAPINFO)&bih, DIB_RGB_COLORS);//��ȡλͼ����,����pָ���������

		memcpy(bmpMem + 54, p, bm.bmWidthBytes * bm.bmHeight);
		delete p;

		//FILE *fp = fopen("Screen.bmp", "w+b");
		//fwrite(&bfh, 1, sizeof(BITMAPFILEHEADER), fp);//д��λͼ�ļ�ͷ
		//fwrite(&bih, 1, sizeof(BITMAPINFOHEADER), fp);//д��λͼ��Ϣͷ
		//fwrite(p, 1, bm.bmWidthBytes * bm.bmHeight, fp);//д��λͼ����
		//fclose(fp);
		bmpsize = bfh.bfSize;//����Ŀռ�Ĵ�С
		return bmpMem;

		//SelectObject(memDC, oldmemBitmap);
		//cout << "��Ļͼ���Ѵ洢���ļ���" << endl;

	}

};


