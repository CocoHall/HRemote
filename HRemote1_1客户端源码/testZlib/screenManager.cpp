#pragma once
#include "pch.h"
#include <string> 
#include<WINDOWSX.H>
#include<windows.h>
#include "mStruct.cpp"
#include "ThreadSafeQueue.cpp"
//压缩
#include "zlib/zlib.h"
#pragma comment(lib, "./zlib/zlibstat.lib")

class screenManager {
public:
	static ThreadSafeQueue<mscreen> screenQueue;

	void screenAndZlib() {
		byte* bmpMem = NULL;
		unsigned long bmpSize = 0;
		bmpMem = screen(bmpSize);//需要释放

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
		//获取当前整个屏幕DC
		HDC hDC = GetDC(GetDesktopWindow());
		//CDC *pDC = CDC::FromHandle(hDC);

		//获得颜色模式
		int BitPerPixel = GetDeviceCaps(hDC, BITSPIXEL);
		int Width = GetDeviceCaps(hDC, HORZRES);
		int Height = GetDeviceCaps(hDC, VERTRES);

		//cout << "当前屏幕色彩模式为" << BitPerPixel << "位色彩" << endl
		//	<< "屏幕宽度：" << Width << endl
		//	<< "屏幕高度：" << Height << endl << endl;

		HDC memDC;//内存DC
		memDC = CreateCompatibleDC(hDC);

		HBITMAP memBitmap;
		HGDIOBJ oldmemBitmap;//建立和屏幕兼容的bitmap
		memBitmap = CreateCompatibleBitmap(hDC, Width, Height);

		oldmemBitmap = SelectObject(memDC, memBitmap);//将memBitmap选入内存DC
		BitBlt(memDC, 0, 0, Width, Height, hDC, 0, 0, SRCCOPY);//复制屏幕图像到内存DC

		//以下代码保存memDC中的位图到文件
		//BITMAP bmp;
		//GetBitmap(&bmp);//获得位图信息

		BITMAP bm;
		GetObject(memBitmap, sizeof(BITMAP), (LPBYTE)&bm);

		BITMAPINFOHEADER bih = { 0 };//位图信息头
		bih.biBitCount = bm.bmBitsPixel;//每个像素字节大小
		bih.biCompression = BI_RGB;
		bih.biHeight = bm.bmHeight;//高度
		bih.biPlanes = 1;
		bih.biSize = sizeof(BITMAPINFOHEADER);
		bih.biSizeImage = bm.bmWidthBytes * bm.bmHeight;//图像数据大小
		bih.biWidth = bm.bmWidth;//宽度

		BITMAPFILEHEADER bfh = { 0 };//位图文件头
		bfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);//到位图数据的偏移量
		bfh.bfSize = bfh.bfOffBits + bm.bmWidthBytes * bm.bmHeight;//文件总的大小
		bfh.bfType = (WORD)0x4d42;


		byte * p = new byte[bm.bmWidthBytes * bm.bmHeight];//申请内存保存位图数据
		byte * bmpMem = new byte[bfh.bfSize];

		bmpIntoMem(bmpMem, bfh, bih);

		GetDIBits(memDC, (HBITMAP)memBitmap, 0, Height, p,
			(LPBITMAPINFO)&bih, DIB_RGB_COLORS);//获取位图数据,放在p指向的区域内

		memcpy(bmpMem + 54, p, bm.bmWidthBytes * bm.bmHeight);
		delete p;

		//FILE *fp = fopen("Screen.bmp", "w+b");
		//fwrite(&bfh, 1, sizeof(BITMAPFILEHEADER), fp);//写入位图文件头
		//fwrite(&bih, 1, sizeof(BITMAPINFOHEADER), fp);//写入位图信息头
		//fwrite(p, 1, bm.bmWidthBytes * bm.bmHeight, fp);//写入位图数据
		//fclose(fp);
		bmpsize = bfh.bfSize;//申请的空间的大小
		return bmpMem;

		//SelectObject(memDC, oldmemBitmap);
		//cout << "屏幕图像已存储到文件中" << endl;

	}

};


