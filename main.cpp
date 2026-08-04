#include "mainwindow.h"
#include <QApplication>
#include <QMetaType>
#include "worker.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // 使用 Fusion 风格，确保 QSS 样式（圆角、渐变背景、悬停）完全生效
    a.setStyle("Fusion");


    // 注册跨线程信号/槽所需的元类型
    qRegisterMetaType<Job>("Job");
    qRegisterMetaType<QVector<Job>>("QVector<Job>");

    MainWindow w;
    w.show();
    return a.exec();
}
