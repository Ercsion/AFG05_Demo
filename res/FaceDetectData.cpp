#include "FaceDetectData.h"
#include "myhelper.h"
#include <QDebug>
#include <QDateTime>

FaceDetectData::FaceDetectData()
{
    dataArr_m = new QByteArray(1024,0);
}

FaceDetectData::~FaceDetectData()
{
    delete dataArr_m;
    dataArr_m = NULL;
}

void FaceDetectData::setUp(cmdList cmd)
{
    dataArr_m->fill(0);
    u8* buff = (u8*)dataArr_m->data();
    dataLen_m = 0;
    u16 len = setUpData(cmd,buff+sizeof(dataHead_S));
    if(0 >= len) return;

    dataHead_S* p_dataHead = (dataHead_S*)buff;
    memset(p_dataHead->head,0x23,2);
    p_dataHead->len[0]=LBYTE(len+sizeof(dataHead_S)+4);//长度低位字节在前
    p_dataHead->len[1]=HBYTE(len+sizeof(dataHead_S)+4);//长度高位字节在后
    p_dataHead->type = 0x0B;
    p_dataHead->priCmd = 0x40;
    p_dataHead->subCmd = LBYTE(cmd);
    dataLen_m +=sizeof(dataHead_S);

    dataLen_m+=len;

    XC_m = XorCheck(buff,dataLen_m);
    memcpy(buff + dataLen_m, &XC_m, sizeof(XC_m));
    //data_m[dataLen_m] = LBYTE(XC_m); // 低位字节在前
    //data_m[dataLen_m+1] = HBYTE(XC_m); // 高位字节在后

    dataLen_m+=2;

    buff[dataLen_m] = 0x0D;
    buff[dataLen_m+1] = 0x0A;
    dataLen_m+=2;
    dataArr_m->resize(dataLen_m);
    DEMO_DPrintf(buff,dataLen_m,"buff");
}

u16 FaceDetectData::setUpData(cmdList cmd,u8* buf)
{
    switch(cmd)
    {
    /// 握手
    case C0x01:
    {
        QDateTime date = QDateTime::currentDateTime();
        QString str = date.toString("yyyyMMddhhmmss");
        QByteArray outData = myHelper::DecimalStrToByteArray(str);
        memcpy(buf,outData,outData.size());
        return outData.size();
    }
    /// 查询人员ID
    case C0x02:
    /// 删除人员ID
    case C0x04:
    {
        int userID = myHelper::ShowInputBox(0,"Please input a user ID").toInt();
        if(0 >= userID){ return 0;}

        QString str = QString("%1").arg(userID,8,10,QLatin1Char('0'));
        QByteArray outData = str.toAscii();
        memcpy(buf,outData,outData.size());
        return outData.size();
    }
    /// 下载特征文件
    case C0x03:
    {
        myHelper::ShowMessageBoxInfoX("Unused for now ...");
        //先选择上传的文件
        int userID = myHelper::ShowInputBox(0,"Please input a user ID").toInt();
        if(0 >= userID){ return 0;}

        QString filePath = myHelper::GetFileName("*.dat");
        if(""==filePath){return 0;}
        return 0;
        QString str = QString("%1").arg(userID,8,10,QLatin1Char('0'));
        QByteArray outData = str.toAscii();
        memcpy(buf,outData,outData.size());
        return outData.size();
    }
    /// 开始对比操作
    case C0x05:
    {
        int userID = myHelper::ShowInputBox(0,"Please input a user ID").toInt();
        if(0 >= userID){ return 0;}

        QString str = QString("%1").arg(userID,8,10,QLatin1Char('0'));
        QByteArray outData = str.toAscii();
        memcpy(buf,outData,outData.size());
        *(buf+outData.size()) = 0x01;
        return outData.size();
    }
    }
    return 0;
}

u16 FaceDetectData::XorCheck(u8* buf, u16 len)
{
    u16 xc=0;
    while(len!=0)
    {
    xc^=(u16)*buf;
    buf++;
    len--;
    }
    return xc;

}
