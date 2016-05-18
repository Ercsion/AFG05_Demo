#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QDebug>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    Init();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::Init()
{

    QStringList ComList;//串口号
    QStringList CmdList;//命令字

#ifdef Q_OS_WIN//如果是windows系统
    ComList<<"COM1"<<"COM2"<<"COM3"<<"COM4"<<"COM5"<<"COM6"
          <<"COM7"<<"COM8"<<"COM9"<<"COM10"<<"COM11"<<"COM12"
         <<"COM13"<<"COM14"<<"COM15";
#else//如果是unix或者其他系统
    ComList<<"ttyUSB0"<<"ttyUSB1"<<"ttyUSB2"<<"ttyUSB3"<<"ttyUSB4"<<"ttyUSB5"
          <<"ttyS0"<<"ttyS1"<<"ttyS2"<<"ttyS3"<<"ttyS4"<<"ttyS5"<<"ttyS6"
         <<"ttyS7"<<"ttyS8"<<"ttyS9";
#endif
    ui->CBoxPort->addItems(ComList);
    ui->CBoxPort->setCurrentIndex(0);

    CmdList<<"0x01|握手信息"<<"0x02|查询学员信息"<<"0x03|下载人脸特征文件"<<"0x04|删除人员信息"
          <<"0x05|开始比对操作"<<"0x06|查询比对结果"<<"0x07|过程拍照"<<"0x08|抓拍"
         <<"0x09|获取记录(照片记录)"<<"0x0A|删除记录（照片记录）"<<"0x0B|临时关闭换人比对和过程拍照"
        <<"0xE0|固件升级"<<"0xE1|设置比对溢出时间"<<"0xE2|设置比对参数（包括签到、签退及过程比对）"
       <<"0xE3|主机当前网络状态（保留，暂时无用）"<<"0xE4|设置开关立即下发过程换人报警"<<"0xE5|检查是u盘是否挂载"
      <<"0xE6|创建文件"<<"0xE7|文件数据写入"<<"0xE8|文转储"<<"0xE9|训练拍照"<<"0xEA|从设备待机"
     <<"0xEB|密钥更新"<<"0xA1|从设备下发报警信息"<<"0xB0|配置文件传输"<<"0xD1|查询激活状态";
    ui->CBoxCmd->addItems(CmdList);
    ui->CBoxCmd->setCurrentIndex(0);

    ReadCount = 0;
    ReadByteCount = 0;
    ui->LabelRxInfo->setText(QString("接收:%1 包，%2 字节").arg(ReadCount).arg(ReadByteCount));

    SendCount = 0;
    SendByteCount = 0;
    ui->LabelTxInfo->setText(QString("发送:%1 包，%2 字节").arg(SendCount).arg(SendByteCount));

    //读取数据(采用定时器读取数据，不采用事件，方便移植到linux)
    myReadTimer=new QTimer(this);
    myReadTimer->setInterval(300);
    connect(myReadTimer,SIGNAL(timeout()),this,SLOT(ReadMyCom()));

    toDev = new FaceDetectData();


#ifdef QT_NO_DEBUG
    ChangeEnable(false);
#else
    ChangeEnable(true);
#endif

}

void MainWindow::ChangeEnable(bool b)
{
    ui->CBoxPort->setEnabled(!b);

    ui->CBoxCmd->setEnabled(b);
    ui->PButtonSend->setEnabled(b);
    ui->TBrowserTx->setEnabled(b);
    ui->TBrowserRx->setEnabled(b);
}

void MainWindow::on_PButtonOpen_clicked()
{
    if (ui->PButtonOpen->text()=="打开")
    {
        QString portName = ui->CBoxPort->currentText();

#ifdef Q_OS_WIN//如果是windows系统
        myCom = new QextSerialPort(portName);
#else
        myCom = new QextSerialPort("/dev/" + portName);
#endif
        if (myCom->open(QIODevice::ReadWrite))
        {
            //清空缓冲区
            myCom->flush();
            //设置波特率
            myCom->setBaudRate(BAUD115200);
            //设置数据位
            myCom->setDataBits(DATA_8);
            //设置校验位
            myCom->setParity(PAR_NONE);
            //设置停止位
            myCom->setStopBits(STOP_1);
            myCom->setFlowControl(FLOW_OFF);
            myCom->setTimeout(10);

            this->ChangeEnable(true);
            ui->PButtonOpen->setText("关闭");
            this->myReadTimer->start();
        }
        else
        {
            myHelper::ShowMessageBoxErrorX("Failed to open serial port!");
        }
    }
    else
    {
        myCom->close();
        this->ChangeEnable(false);
        ui->PButtonOpen->setText("打开");
        this->myReadTimer->stop();
    }

}

void MainWindow::on_PButtonSend_clicked()
{
    this->WriteMyCom();
}

void MainWindow::on_PButtonRxClean_clicked()
{
    SendCount = 0;
    SendByteCount = 0;
    ui->LabelTxInfo->setText(QString("发送:%1 包，%2 字节").arg(SendCount).arg(SendByteCount));
    ui->TBrowserTx->clear();
}

void MainWindow::on_PButtonTxClean_clicked()
{
    ReadCount = 0;
    ReadByteCount = 0;
    ui->LabelRxInfo->setText(QString("接收:%1 包，%2 字节").arg(ReadCount).arg(ReadByteCount));
    ui->TBrowserRx->clear();
}

void MainWindow::ReadMyCom()
{
    //这个判断尤为重要,否则的话直接延时再接收数据,空闲时和会出现高内存占用
    if (myCom->bytesAvailable()<=0){return;}

    myHelper::Sleep(100);//延时100毫秒保证接收到的是一条完整的数据,而不是脱节的
    QByteArray buffer=myCom->readAll();

    QString tempDataHex=myHelper::ByteArrayToHexStr(buffer);
    ui->TBrowserRx->append(QString("[%1] %2")
                           .arg(QTime::currentTime().toString("HH:mm:ss"))
                           .arg(tempDataHex));

    ReadCount++;
    ReadByteCount+=buffer.size();
    ui->LabelRxInfo->setText(QString("接收:%1 包，%2 字节").arg(ReadCount).arg(ReadByteCount));
}

void MainWindow::WriteMyCom()
{
#ifdef QT_NO_DEBUG
    if (!myCom->isOpen())
    {
        myHelper::ShowMessageBoxErrorX("Please open your serial port first.");
        return;
    }
#endif


    QString str = ui->CBoxCmd->currentText();
    cmdList cmd = (cmdList)(str.split("|").at(0).toInt(0,16));

    toDev->setUp(cmd);
    if(0 >= toDev->getDataLen()){return;}

    int ret = myCom->write(*(toDev->getData()));

#ifdef QT_NO_DEBUG
    if(0 >= ret)
    {
        myHelper::ShowMessageBoxErrorX("Failed to write serial port!");
        return;
    }
#endif
    ///-- 显示发送数据信息
    ui->TBrowserTx->append(QString("[%1]").arg(QTime::currentTime().toString("HH:mm:ss")));
    ui->TBrowserTx->append(QString("%1").arg(str));
    ui->TBrowserTx->append(QString("%1").arg(myHelper::ByteArrayToHexStr(*(toDev->getData()))));

    SendCount++;
    SendByteCount+=ret;
    ui->LabelTxInfo->setText(QString("发送:%1 包，%2 字节").arg(SendCount).arg(SendByteCount));
}
