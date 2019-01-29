//#if _MSC_VER >= 1600
//#pragma execution_character_set("utf-8")
//#endif
#pragma once
#ifndef MYTERMINAL_H
#define MYTERMINAL_H
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>




class mylistenWindows : public QWidget
{
    Q_OBJECT
public:
    explicit mylistenWindows(QWidget *parent = nullptr);
    ~mylistenWindows();
    QPushButton * button;
    QLabel* l_port;
    QLineEdit* t_port;
    QHBoxLayout* h;
    QVBoxLayout* v;

public slots:
    void mlisten();
signals:
    void signalAddLog(QString,int);
    void signalChangeState(QString);

};

#endif // MYLISTEN_H
