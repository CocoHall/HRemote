#include "mylistenWindows.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QApplication>
#include <QDesktopWidget>
#include "tool.h"
#include <QMessageBox>

mylistenWindows::mylistenWindows(QWidget *parent) : QWidget(parent)
{
    button=new QPushButton(this);
    l_port=new QLabel(this);
    t_port=new QLineEdit(this);

    l_port->setText("端口：");
    t_port->setText(QString::number(tool::port, 10));
    if(tool::islisten){
        button->setText("停止监听");
        t_port->setEnabled(false);
    }else{
        button->setText("开始监听");
        t_port->setEnabled(true);
    }

    const int w = 200, hh = 150;
    const int x = (QApplication::desktop()->width() - w) >> 1;
    const int y = (QApplication::desktop()->height() - hh) >> 1;
    this->setGeometry(x, y, w, hh);
    //设置显示的位置和大小
    setFixedSize(this->width(), this->height());
    //设置不可调节大小
    this->setWindowTitle("Listen");
    this->setWindowIcon(QIcon(":/image/image/listening.png"));
    this->setWindowFlags(this->windowFlags()& ~Qt::WindowMaximizeButtonHint& ~Qt::WindowMinimizeButtonHint);
    //隐藏最大最小按钮
    h=new QHBoxLayout();
    h->addWidget(l_port,1);
    h->addWidget(t_port,3);

    v=new QVBoxLayout();

    v->addLayout(h);
    v->addWidget(button);

    this->setLayout(v);

    connect(button, SIGNAL(clicked(bool)), this, SLOT(mlisten()));




//    connect(this, SIGNAL(signalAddLog(QString,int)), tool::m, SLOT(addLog(QString,int)));
//    connect(this, SIGNAL(signalChangeState(QString,int)), tool::m, SLOT(setListenState(QString,int)));
}

mylistenWindows::~mylistenWindows(){
//    delete[] button;
//    delete[] l_port;
//    delete[] t_port;
//    delete[] h;
//    delete[] v;
//    button = nullptr;
//    l_port = nullptr;
//    t_port = nullptr;
//    h = nullptr;
//    v = nullptr;

}

void mylistenWindows::mlisten(){
    QString qp = t_port->text();
    bool ok;
    int p = qp.toInt(&ok,10);
    if(ok==false||p<1024||p>65535){
        QMessageBox::critical(nullptr, "警告", "端口不正确", QMessageBox::Yes , QMessageBox::Yes);
        return;
    }
    tool::port=p;

    if(tool::islisten==true){
        tool::islisten=false;
        closesocket(tool::sockSrv);
        tool::acc->exit();

        button->setText("开始监听");
        t_port->setEnabled(true);
//        tool::m->addLog("停止监听"+qp+"端口",4);
        emit signalAddLog("停止监听"+qp+"端口",4);

//        tool::m->setListenState("未监听");
        emit signalChangeState("未监听");

    }else{


        //创建用于监听的套接字
        tool::sockSrv = socket(AF_INET, SOCK_STREAM, 0);

        SOCKADDR_IN addrSrv;
        addrSrv.sin_family = AF_INET;
        addrSrv.sin_port = htons(tool::port);
        addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

        int retVal = bind(tool::sockSrv, (LPSOCKADDR)&addrSrv, sizeof(SOCKADDR_IN));


        if(retVal == SOCKET_ERROR){
            QString error_log="绑定端口"+qp+"失败";
//            tool::m->addLog(error_log,4);
            emit signalAddLog(error_log,4);
            closesocket(tool::sockSrv);
            return;
        }

        if(listen(tool::sockSrv,10) ==SOCKET_ERROR){
            QString error_log="监听端口"+qp+"失败";
//            tool::m->addLog(error_log,5);
            emit signalAddLog(error_log,5);
            closesocket(tool::sockSrv);
            return;
        }
        QString success_log="开始监听"+qp+"端口";
//        tool::m->addLog(success_log,4);
        emit signalAddLog(success_log,4);
        tool::islisten=true;
        t_port->setEnabled(false);
        button->setText("停止监听");

//        tool::m->setListenState("开始监听"+qp+"端口");
        emit signalChangeState("正在监听"+qp+"端口");

        tool::acc->start();
        this->hide();
//        if(tool::updateinfo==NULL){
//            tool::updateinfo=new updateInfo();
//            tool::updateinfo->start();
//        }

    }


}
