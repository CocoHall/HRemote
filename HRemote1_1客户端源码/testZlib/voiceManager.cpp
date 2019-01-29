#pragma once
#include "pch.h"
//��Ƶ
//#include<mmsystem.h>
#pragma comment(lib, "winmm.lib") 
#include<WINDOWSX.H>
#include<windows.h>

#include <iostream>
#include<stdio.h>
#include<stdlib.h>
#include "mStruct.cpp"
#include "ThreadSafeQueue.cpp"
//¼��

#include "zlib/zlib.h"
#pragma comment(lib, "./zlib/zlibstat.lib")
#include "netmanager.h"
#include "tool.cpp"

extern netManager* nm;
using namespace std;
class voiceManager{
public:
	
	static ThreadSafeQueue<mvoice> voiceQueue;
	
	void start() {
		if (state >0)return;//�Ѿ��ڼ�¼��
		thread t1(std::bind(&voiceManager::recordVoice, this));
		t1.detach();
	}
	
	void stop() {
		state = 0;
	}
private:
	int state = 0;

	int playVoice(string fileName) {
		char tmpbuf[1024 * 512];//����������Ƶ
		FILE*           thbgm;//�ļ�
		int             cnt;
		HWAVEOUT        hwo;
		WAVEHDR         wh;
		WAVEFORMATEX    wfx;
		HANDLE          wait;

		wfx.wFormatTag = WAVE_FORMAT_PCM;//���ò��������ĸ�ʽ
		wfx.nChannels = 1;//������Ƶ�ļ���ͨ������
		wfx.nSamplesPerSec = 8000;//����ÿ���������źͼ�¼ʱ������Ƶ��
		wfx.nAvgBytesPerSec = 16000;//���������ƽ�����ݴ�����,��λbyte/s�����ֵ���ڴ��������С�Ǻ����õ�
		wfx.nBlockAlign = 2;//���ֽ�Ϊ��λ���ÿ����
		wfx.wBitsPerSample = 16;
		wfx.cbSize = 0;//������Ϣ�Ĵ�С
		wait = CreateEvent(NULL, 0, 0, NULL);
		waveOutOpen(&hwo, WAVE_MAPPER, &wfx, (DWORD_PTR)wait, 0L, CALLBACK_EVENT);//��һ�������Ĳ�����Ƶ���װ�������лط�

		fopen_s(&thbgm, fileName.c_str(), "rb");
		//cout << "ok";
		cnt = fread(tmpbuf, sizeof(char), sizeof(tmpbuf), thbgm);//��ȡ�ļ�0.5M�����ݵ��ڴ������в��ţ�ͨ��������ֵ��޸ģ������߳̿ɱ��������Ƶ���ݵ�ʵʱ���䡣��Ȼ���ϣ��������������Ƶ�ļ���Ҳ��Ҫ��������΢��һ��

		int dolenght = 0;
		int playsize = 1024;
		while (cnt >= 0) {//��һ������Ҫ�ر�ע�������ѭ������֮���ܻ�̫����ʱ��ȥ����ȡ����֮��Ĺ�������Ȼ��ÿ��ѭ���ļ�϶���С����ա�������
			wh.lpData = tmpbuf + dolenght;
			wh.dwBufferLength = playsize;
			wh.dwFlags = 0L;
			wh.dwLoops = 1L;
			waveOutPrepareHeader(hwo, &wh, sizeof(WAVEHDR));//׼��һ���������ݿ����ڲ���
			waveOutWrite(hwo, &wh, sizeof(WAVEHDR));//����Ƶý���в��ŵڶ�������whָ��������
			WaitForSingleObject(wait, INFINITE);//�������hHandle�¼����ź�״̬����ĳһ�߳��е��øú���ʱ���߳���ʱ��������ڹ����INFINITE�����ڣ��߳����ȴ��Ķ����Ϊ���ź�״̬����ú�����������
			dolenght = dolenght + playsize;
			cnt = cnt - playsize;
		}
		waveOutClose(hwo);
		fclose(thbgm);
		return 0;

	}

	void recordVoice() {
		
		state=1;
		//FILE *pf;     //�洢�ɼ�������Ƶ�ļ�
		HWAVEIN hWaveIn;       //�����豸
		WAVEFORMATEX waveform;      //�ɼ���Ƶ�ĸ�ʽ���ṹ��
		BYTE *pBuffer1, *pBuffer2;     //�ɼ���Ƶʱ�����ݻ���
		WAVEHDR wHdr1, wHdr2;      //�ɼ���Ƶʱ�������ݻ���Ľṹ��
		DWORD bufsize = 1024 * 100;//ÿ�ο���100k�Ļ���洢¼������

		HANDLE wait;
		waveform.wFormatTag = WAVE_FORMAT_PCM;//������ʽΪPCM
		waveform.nSamplesPerSec = 8000;//�����ʣ�16000��/��
		waveform.wBitsPerSample = 16;//�������أ�16bits/��
		waveform.nChannels = 1;//������������2����
		waveform.nAvgBytesPerSec = 16000 * 4;//ÿ��������ʣ�����ÿ���ܲɼ������ֽڵ�����
		waveform.nBlockAlign = 2;//һ����Ĵ�С������bit���ֽ�������������
		waveform.cbSize = 0;//һ��Ϊ0

		wait = CreateEvent(NULL, 0, 0, NULL);
		//ʹ��waveInOpen����������Ƶ�ɼ�
		waveInOpen(&hWaveIn, WAVE_MAPPER, &waveform, (DWORD_PTR)wait, 0L, CALLBACK_EVENT);
		//fopen_s(&pf, "1.pcm", "wb");
		while (state)//¼�������������Ƶ��������紫������޸�Ϊʵʱ¼�����ŵĻ�����ʵ�ֶԽ�����
		{
			pBuffer1 = new BYTE[bufsize];
			pBuffer2 = new BYTE[bufsize];

			wHdr1.lpData = (LPSTR)pBuffer1;
			wHdr1.dwBufferLength = bufsize;
			wHdr1.dwBytesRecorded = 0;
			wHdr1.dwUser = 0;
			wHdr1.dwFlags = 0;
			wHdr1.dwLoops = 1;

			wHdr1.lpNext = NULL;
			wHdr1.reserved = 0;

			waveInPrepareHeader(hWaveIn, &wHdr1, sizeof(WAVEHDR));//׼��һ���������ݿ�ͷ����¼��

			//˫����
			wHdr2.lpData = (LPSTR)pBuffer2;
			wHdr2.dwBufferLength = bufsize;
			wHdr2.dwBytesRecorded = 0;
			wHdr2.dwUser = 0;
			wHdr2.dwFlags = 0;
			wHdr2.dwLoops = 1;
			wHdr2.lpNext = NULL;
			wHdr2.reserved = 0;
			waveInPrepareHeader(hWaveIn, &wHdr2, sizeof(WAVEHDR));
			//������wHdr��ӵ�waveIn��ȥ

			waveInAddBuffer(hWaveIn, &wHdr1, sizeof(WAVEHDR));
			waveInAddBuffer(hWaveIn, &wHdr2, sizeof(WAVEHDR));


			waveInStart(hWaveIn);

			Sleep(1000);//�ȴ�����¼��1s
			waveInReset(hWaveIn);//ֹͣ¼��
			//fwrite(pBuffer1, 1, wHdr1.dwBytesRecorded, pf);


			byte *voiceZipMem = new byte[wHdr1.dwBytesRecorded];
			unsigned long  voiceZipSize = wHdr1.dwBytesRecorded;
			compress(voiceZipMem, &voiceZipSize, pBuffer1, wHdr1.dwBytesRecorded);

			mvoice tmpvoice;
			tmpvoice.voiceZipMem = voiceZipMem;
			tmpvoice.voiceZipSize = voiceZipSize;
			tmpvoice.voiceSize = wHdr1.dwBytesRecorded;
			voiceQueue.push(tmpvoice);
			nm->addOrder();



			delete pBuffer1;
			delete pBuffer2;
		}

		Sleep(2000);//ȷ�����ݶ�������
		//fclose(pf);

		waveInClose(hWaveIn);
		return;
	}
};
