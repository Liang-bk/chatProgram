#include "mainwindow.h"

#include <QApplication>
#include <QFile>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QFile qss_file(":/style/stylesheet.qss");
    if (qss_file.open(QFile::ReadOnly)) {
        qDebug("Open success");
        QString style = QLatin1String(qss_file.readAll());
        a.setStyleSheet(style);
        qss_file.close();
    } else {
        qDebug("Open failed");
    }
    MainWindow w;
    w.show();
    return a.exec();
}
