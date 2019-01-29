#include "myterminalwindows.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QClipboard>
#include "tool.h"

myterminalWindows::myterminalWindows(QWidget *parent,netManager* nm) : QWidget(parent)
{
    setAttribute(Qt::WA_DeleteOnClose, true);
    this->nm = nm;
    nowKey=0;

    right_menu = new QMenu(this);
    history = new QList<QString>;
    t_result = new QTextEdit(this);
    t_cmd = new QTextEdit(this);
    a_clear = new QAction("清空",this);
    a_copy = new QAction("复制",this);
    a_restartCMD = new QAction("关闭",this);
    right_menu->addAction(a_copy);
    right_menu->addAction(a_clear);
    right_menu->addAction(a_restartCMD);

    t_result->setReadOnly(true);
    t_result->setContextMenuPolicy(Qt::CustomContextMenu);//自定义右键菜单

    connect(t_result, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(ShowRightMenu(QPoint)));

    QPalette palette = t_result->palette();
    palette.setColor(QPalette::Highlight, QColor(0xe5,0xe5,0xe5));// 被选中后文字的背景色.
    palette.setColor(QPalette::HighlightedText , QColor(00,00,00));// 被选中后文字的前景色.
    palette.setColor(QPalette::Text, QColor(0xe5,0xe5,0xe5));// 文字的前景色
    palette.setColor(QPalette::Base, QColor(00,00,00)); // QTextEdit的背景色
    t_result->setPalette(palette);
    t_result->setFont(QFont(tr("Consolas"), 12));



    QPalette palette2 = t_cmd->palette();
    palette.setColor(QPalette::Highlight, QColor(0xe5,0xe5,0xe5));// 被选中后文字的背景色.
    palette.setColor(QPalette::HighlightedText , QColor(00,00,00));// 被选中后文字的前景色.
    palette.setColor(QPalette::Text, QColor(0xe5,0xe5,0xe5));// 文字的前景色
    palette.setColor(QPalette::Base, QColor(00,00,00)); // QTextEdit的背景色
    t_cmd->setPalette(palette);
    t_cmd->setFont(QFont("Consolas", 12));
    t_cmd->installEventFilter(this);


    const int w = 800, hh = 600;
    const int x = (QApplication::desktop()->width() - w) >> 1;
    const int y = (QApplication::desktop()->height() - hh) >> 1;
    this->setGeometry(x, y, w, hh);
    //设置显示的位置和大小
//    setFixedSize(this->width(), this->height());
    //设置不可调节大小
    this->setWindowTitle("Image "+tool::GetAddressBySocket(nm->sClient));
    this->setWindowIcon(QIcon(":/image/image/terminal.png"));

//    this->setWindowFlags(this->windowFlags()& ~Qt::WindowMaximizeButtonHint);
    //隐藏最大最小按钮
    v =new QVBoxLayout();

    v->addWidget(t_result,10);
    v->addWidget(t_cmd,1);

    this->setLayout(v);
    t_cmd->setFocus();

    connect(a_clear, SIGNAL(triggered(bool)), this, SLOT(clearResult()));
    connect(a_copy, SIGNAL(triggered(bool)), this, SLOT(copyText()));
    connect(a_restartCMD, SIGNAL(triggered(bool)), this, SLOT(restartCMD()));

    connect(nm,SIGNAL(signalAddTermResult(QString)),this,SLOT(addResult(QString)));



}

myterminalWindows::~myterminalWindows(){
//    disconnect(nm,SIGNAL(signalAddTermResult(QString)),this,SLOT(addResult(QString)));
    nm->terminalwindows=nullptr;
//    delete[] history ;
//    delete[] t_result ;
//    delete[] t_cmd ;
//    delete[] a_clear ;
//    delete[] a_copy ;
//    delete[] right_menu;
//    delete[] history;
//    delete[] v;
//    history  =nullptr;
//    t_result  =nullptr;
//    t_cmd  =nullptr;
//    a_clear  =nullptr;
//    a_copy  =nullptr;
//    right_menu =nullptr;
//    history =nullptr;
//    v =nullptr;

}

void myterminalWindows::clearResult(){
    t_result->clear();
}

void myterminalWindows::addResult(QString s){
    t_result->moveCursor(QTextCursor::End);
//    t_result->insertPlainText("\n");
    t_result->insertPlainText(s);
    t_result->moveCursor(QTextCursor::End);

}

void myterminalWindows::copyText(){
    QClipboard *clipboard = QApplication::clipboard();   //获取系统剪贴板指针
    clipboard->setText(t_result->textCursor().selectedText());
}

void myterminalWindows::restartCMD(){
    morder tmporder;
    tmporder.orderType=21;
    tmporder.orderMem=nullptr;

    tmporder.orderSize=0;

    nm->orderQueue.push(tmporder);
    nm->addOrder();


}
void myterminalWindows::sendCMD(){
    QString origin = t_cmd->toPlainText();
    QString s = origin +"\n";
    std::string std_s = s.toStdString();
    unsigned long long l =std_s.length();

    morder tmporder;
    tmporder.orderType=20;
    tmporder.orderSize = l;
    tmporder.orderMem = new char[l];

    for(int i=0;i<l;i++){
        tmporder.orderMem[i]=std_s.at(i);
    }


   nm->orderQueue.push(tmporder);
   nm->addOrder();

//    addResult(s);
   if(origin.length()>2){
        history->push_back(origin);
        nowKey=history->size();
   }
   t_cmd->clear();


}


bool myterminalWindows::eventFilter(QObject *target, QEvent *event){
    if(target == t_cmd) {
        if(event->type() == QEvent::KeyPress)//回车键
        {
            QKeyEvent *k = static_cast<QKeyEvent *>(event);
            if(k->key() == Qt::Key_Return)
            {
                sendCMD();
                return true;
            }else if (k->key() == Qt::Key_Up){
                if(nowKey>0){
                    t_cmd->setText(history->at(nowKey-1));
                    nowKey=(nowKey-1)>1?(nowKey-1):1;
                    t_cmd->moveCursor(QTextCursor::End);
                    return true;
                }
            }else if (k->key() == Qt::Key_Down){
                if(nowKey>0){
                    t_cmd->setText(history->at(nowKey-1));
                    nowKey=(nowKey +1) > history->size()?history->size():(nowKey+1);
                    t_cmd->moveCursor(QTextCursor::End);
                    return true;
                }
            }
        }
    }
    return QWidget::eventFilter(target,event);


}

void myterminalWindows::ShowRightMenu(const QPoint& pos)//显示右键菜单
{
    QPoint globalPos = t_result->mapToGlobal(pos);
    right_menu->exec(globalPos);

}


