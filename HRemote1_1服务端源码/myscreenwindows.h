#pragma once
#ifndef MYSCREENWINDOWS_H
#define MYSCREENWINDOWS_H
//#if _MSC_VER >= 1600
//#pragma execution_character_set("utf-8")
//#endif
#include<QPushButton>
#include<QLabel>
#include<QScrollArea>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "netmanager.h"

class netManager;
class myscreenWindows: public QWidget
{
    Q_OBJECT
public:
    explicit myscreenWindows(QWidget *parent = nullptr,netManager* nm=nullptr);
    ~myscreenWindows();
    QPushButton * b_screen,*b_photo;
    QLabel *l_image;
    netManager* nm;
    QScrollArea *scrollArea;
    QVBoxLayout* v;
    QHBoxLayout* h,*h_image;


public slots:
    void screen();
    void addImage(QString);
    void takephoto();

};

#endif // MYSCREENWINDOWS_H
