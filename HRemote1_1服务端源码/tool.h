#pragma once
//#if _MSC_VER >= 1600
//#pragma execution_character_set("utf-8")
//#endif
#ifndef TOOL_H
#define TOOL_H

#include <QString>
#include <QMap>
#include "mainwindow.h"
#include <winsock2.h>
#include "myaccepyThread.h"
#include <QMutex>
#include "netmanager.h"
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

class  tool
{
public:
    tool();
    static bool isQuit;
    static QSqlDatabase database;
    static QMutex* mut;     //ui线程的info表的锁
    static int port;
    static QList<netManager*>* storeList;
    static ThreadSafeQueue<netManager*>* deleteQueue;
    static bool islisten;
    static MainWindow* m;
    static SOCKET sockSrv;
    static myaccepyThread* acc;

    static QString GetAddressBySocket(SOCKET client);
    static QString GetIPBySocket(SOCKET client);

    static QString translateTime(int millisecond);

    static long long pow(int a,int b);
    static QString ByteConvert(long long byteSize);
    static QString getFileType(QString);
    static QString timestampConvert(__time64_t);
    static void openDatabase();
    static void insertNickname(QString,QString);
    static QString getNickname(QString);


};

#endif // TOOL_H
