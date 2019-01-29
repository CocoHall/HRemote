#pragma once
#ifndef MYPROCESSWINDOWS_H
#define MYPROCESSWINDOWS_H

#include <QObject>
#include <QWidget>
#include <QVBoxLayout>
#include <QTableWidget>
#include <QMenu>
#include <QAction>
#include "netmanager.h"

class netManager;
class myprocessWindows : public QWidget
{
    Q_OBJECT
public:
    explicit myprocessWindows(QWidget *parent = nullptr,netManager* nm = nullptr);
    ~myprocessWindows();
    netManager* nm;
    QVBoxLayout* v;
    QTableWidget * list;
    QMenu* right_menu;
    QAction* a_refresh,*a_close;


signals:

public slots:
    void addProcess(int,QString);
    void closeProcess();
    void showProcess();
    void showMenu(const QPoint& pos);




};

#endif // MYPROCESSWINDOWS_H
