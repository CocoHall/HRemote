#include "myaccepyThread.h"
#include "tool.h"
#include "netmanager.h"
#include "mainwindow.h"


myaccepyThread::myaccepyThread(){

}

myaccepyThread::~myaccepyThread(){

}
void myaccepyThread::run(){

    SOCKADDR_IN addrClient;
    int len = sizeof(SOCKADDR);

//    emit signalAddLog("开始accept",4);
    while(tool::islisten)
    {
        //等待客户请求到来
        SOCKET sClient = accept(tool::sockSrv, (SOCKADDR *) &addrClient, &len);
        if(sClient == SOCKET_ERROR){
//            emit signalAddLog("accept失败",4);
            break;
        }

        netManager* nm =new netManager(sClient);
        nm->start();
        tool::mut->lock();
        tool::storeList->push_back(nm);
        tool::mut->unlock();

        emit signalAddInfo(sClient);
        emit signalAddLog(tool::GetAddressBySocket(nm->sClient)+"连接成功",0);

    }
//    tool::m->addLog("停止accept",4);
//    emit signalAddLog("停止accept",4);

}
