#ifndef FACEDETECTDATA_H
#define FACEDETECTDATA_H

#include <string.h>
#include <iostream>
#include <QByteArray>

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;

#define LBYTE(byte) ((byte)&0xFF)
#define HBYTE(byte) (((byte)>>8)&0xFF)

enum cmdList{C0x01=0x01,C0x02=0x02,C0x03=0x03,C0x04=0x04,C0x05=0x05,C0x06=0x06,
         C0x07=0x07,C0x08=0x08,C0x09=0x09,C0x0A=0x0A,C0x0B=0x0B,
         C0xE0=0xE0,C0xE1=0xE1,C0xE2=0xE2,C0xE3=0xE3,C0xE4=0xE4,C0xE5=0xE5,
         C0xE6=0xE6,C0xE7=0xE7,C0xE8=0xE8,C0xE9=0xE9,C0xEA=0xEA,C0xEB=0xEB,
         C0xA1=0xA1,C0xB0=0xB0,C0xD1=0xD1};

typedef struct{
    u8 head[2];      //主机下发为0x23 0x23;设备上传为 0x26 2x26
    u8 len[2];       //数据头到结束符的整个包长度，字节为前低后高排序
    u8 type;         //外设类型，人脸设备为0x0B
    u8 priCmd;       //主命令字，0x40
    u8 subCmd;     //从命令字
}__attribute__ ((packed)) dataHead_S;

#define DEMO_Print()\
    {\
        {\
            printf("#[demo][%24s ][%-3d]", (__FILE__), __LINE__);\
        }\
    }
#define DEMO_Printf(format, ...)\
    {\
        DEMO_Print();\
        printf(format,##__VA_ARGS__);std::cout<<std::endl;\
    }

#define DEMO_DPrintf(buf,len,format,...)\
{\
    DEMO_Print();\
    printf("   /---------------------------------[ ");\
    printf(format,##__VA_ARGS__);printf("\n");\
    DEMO_Print();printf("   |");\
    for (int i = 0; i < len; i++)\
    {\
        printf("%02X ", (buf)[i] & 0xff);\
        if (!((i + 1) % 10))\
        {\
            if(i!=len-1){\
                printf("\n");\
                DEMO_Print();printf("   |");}\
        }\
    }\
    printf("\n");\
    DEMO_Printf("   \\---------------------------------[ length %3d",len);\
}

class FaceDetectData
{
public:
    FaceDetectData();
    ~FaceDetectData();
    void setUp(cmdList cmd);
    QByteArray* getData(void){return dataArr_m;}
    u16 getDataLen(void){return dataLen_m;}

private:
    u16 XorCheck(u8* buf, u16 len);
    u16 setUpData(cmdList cmd,u8* buf);


    QByteArray* dataArr_m;
    u16 dataLen_m;
    u16 XC_m;
};

#endif // FACEDETECTDATA_H
