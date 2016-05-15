#include <QtGui/QApplication>
#include "MainWindow.h"
#include "myhelper.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //设置字符编码和外观样式
    myHelper::SetUTF8Code();
    myHelper::SetStyle();

    MainWindow w;
    w.show();
    
    return a.exec();
}
