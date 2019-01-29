#pragma once
#ifndef MYFILEWINDOWS_H
#define MYFILEWINDOWS_H

#include <QObject>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTableWidget>
#include <QLineEdit>
#include <QMenu>
#include <QPushButton>
#include <QAction>
#include <QTableWidget>
#include <QStack>
#include "mystruct.cpp"
#include "netmanager.h"

class netManager;
class myfileWindows : public QWidget
{
    Q_OBJECT
public:
    explicit myfileWindows(QWidget *parent = nullptr,netManager* nm = nullptr);
    ~myfileWindows();
    QString current_path,copyFilename,copyFilepath;
    bool copyOrCutFlag;//TRUE: copy false:cut
    QStack<QString> stack_back,stack_forward;
    netManager* nm;
    QHBoxLayout* h;
    QVBoxLayout* v;
    QLineEdit* t_path;
    QPushButton* b_back, *b_forward,*b_refresh,*b_upload,*b_download;
    QPushButton *b_cut,*b_paste,*b_copy,*b_delete;
    QAction *a_delete, *a_download,*a_copy,*a_cut,*a_rename,*a_run,*a_paste;
    QMenu* right_menu;
    QTableWidget* list;

    QIcon getFileIcon(QString);
    QString attribToString(unsigned);



signals:

public slots:

    void back();
    void forward();
    void refresh();
    void upload();
    void download();
    void addFile(mfile);
    void openDir(int,int);
    void showMenu(const QPoint& pos);
    void cut();
    void paste();
    void copy();
    void deletefile();
    void run();
    void rename();

protected:
    bool eventFilter(QObject *target, QEvent *event);//事件过滤器



};

#endif // MYFILEWINDOWS_H
