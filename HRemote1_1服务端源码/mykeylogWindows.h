#pragma once
#ifndef MYKEYLOGWINDOWS_H
#define MYKEYLOGWINDOWS_H
//#if _MSC_VER >= 1600
//#pragma execution_character_set("utf-8")
//#endif
#include<QPushButton>
#include<QLineEdit>
#include<QTextEdit>
#include<QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "netmanager.h"

class netManager;
class mykeylogWindows : public QWidget
{
    Q_OBJECT
public:
    explicit mykeylogWindows(QWidget *parent = nullptr,netManager* nm=nullptr);
    ~mykeylogWindows();
    QPushButton * b_start,*b_stop,*b_clear;
    QLineEdit* t_time;
    QTextEdit *showinfo;
    QLabel *l_time;
    netManager*nm;
    QVBoxLayout* v;
    QHBoxLayout* h;


public slots:
    void start();

    void stop();

    void clear();
    void addKeylog(QString);


private:



};

#endif // MYKEYLOGWINDOWS_H
