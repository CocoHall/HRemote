//#if _MSC_VER >= 1600
//#pragma execution_character_set("utf-8")
//#endif
#include "mainwindow.h"
#include <QApplication>
#include <QThread>
#include <winsock2.h>
#include <QMessageBox>

int main(int argc, char *argv[])
{

    WSADATA wsaData;
    if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        QMessageBox::critical(nullptr, "警告", "WSADATA网络初始化失败", QMessageBox::Yes , QMessageBox::Yes);
        return -1;
    }

    QApplication a(argc, argv);
// 主窗口
    MainWindow w;
    w.show();


    /// netmanager 释放 不会，现在的释放应该是有问题的


    return a.exec();
}
