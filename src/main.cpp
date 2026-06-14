#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QApplication::setApplicationName("FilterImg");
    QApplication::setApplicationDisplayName("多线程图像滤镜处理器");

    MainWindow w;
    w.show();

    return app.exec();
}
