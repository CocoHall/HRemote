#pragma once
#ifndef MYTERMINALWINDOWS_H
#define MYTERMINALWINDOWS_H

#include <QObject>
#include <QWidget>
#include <QTextEdit>
#include <QList>
#include <QVBoxLayout>
#include "netmanager.h"

class netManager;
class myterminalWindows : public QWidget
{
    Q_OBJECT
public:
    explicit myterminalWindows(QWidget *parent = nullptr,netManager* nm=nullptr);
    ~myterminalWindows();
    QTextEdit* t_result,*t_cmd;
    netManager* nm;
    QMenu* right_menu;
    QAction *a_clear,*a_copy,*a_restartCMD;
    QList<QString> *history;
    QVBoxLayout* v;
    int nowKey=0;

    void sendCMD();


protected:
    bool eventFilter(QObject *target, QEvent *event);//事件过滤器

signals:

public slots:

    void clearResult();
    void ShowRightMenu(const QPoint& pos);
    void copyText();
    void restartCMD();
    void addResult(QString);

};

#endif // MYTERMINALWINDOWS_H
