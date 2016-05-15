#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <iostream>

#include "myhelper.h"
#include "qextserialport.h"
#include "FaceDetectData.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    void Init();//初始化界面以及其他处理
    void ChangeEnable(bool b);//改变状态
    
private slots:
    void on_PButtonOpen_clicked();
    void on_PButtonSend_clicked();
    void on_CBoxCmd_currentIndexChanged(const QString &arg1);
    void ReadMyCom();//读取串口数据
    void WriteMyCom();//写串口数据

private:
    Ui::MainWindow *ui;

    QextSerialPort *myCom;
    FaceDetectData* toDev;

    QTimer *myReadTimer;//定时读取串口数据

    int SendCount;//发送数据包数
    int SendByteCount;//发送数据字节

    int ReadCount;//接收数据包数
    int ReadByteCount;//接收数据字节

    bool IsAutoClear;//是否自动清空
};

#endif // MAINWINDOW_H
