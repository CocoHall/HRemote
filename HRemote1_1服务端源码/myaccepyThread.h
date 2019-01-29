//#if _MSC_VER >= 1600
//#pragma execution_character_set("utf-8")
//#endif
#pragma once
#ifndef TCPCONNECT_H
#define TCPCONNECT_H
#include <QObject>
#include <QThread>

#include <winsock2.h>


//#pragma comment(lib, "ws2_32.lib")


class myaccepyThread : public QThread
{
    Q_OBJECT
public:
    myaccepyThread();
    ~myaccepyThread();
    void run();

    SOCKET socketcon;

signals:
    void signalAddLog(QString,int);
    void signalAddInfo(SOCKET);
//    void signalTest(QString);

};


#endif // TCPCONNECT_H
