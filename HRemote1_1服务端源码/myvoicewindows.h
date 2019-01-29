#pragma once
#ifndef MYVOICEWINDOWS_H
#define MYVOICEWINDOWS_H

#include <QObject>
#include <QWidget>
#include <QHBoxLayout>
#include<QPushButton>
#include <QMutex>
#include "netmanager.h"
#include "threadsafequeue.cpp"
#include "mystruct.cpp"

class netManager;
class myvoiceWindows : public QWidget
{
    Q_OBJECT
public:
    explicit myvoiceWindows(QWidget *parent = nullptr,netManager* nm = nullptr);
    ~myvoiceWindows();
    int state;//0表示没有播放或结束播放 1表示正在播放
    ThreadSafeQueue<mvoice> voiceQueue;

private:
    netManager* nm;
    QPushButton * b_play;
    QHBoxLayout* h;
    QMutex* play_mut;

    int threadNum;//表示播放声音的线程数
    void thread_playVoice();


signals:

public slots:
    void playVoice();
};

#endif // MYVOICEWINDOWS_H
