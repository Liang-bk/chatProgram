#include "mainwindow.h"

#include <QApplication>
#include <QFile>
#include "global.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QFile qss_file(":/style/stylesheet.qss");
    if (qss_file.open(QFile::ReadOnly)) {
        qDebug("Qss file Open success");
        QString style = QLatin1String(qss_file.readAll());
        a.setStyleSheet(style);
        qss_file.close();
    } else {
        qDebug("Qss file Open failed");
    }

    QString app_path = QCoreApplication::applicationDirPath();
    QString file_name = "config.ini";
    QString config_path = QDir::toNativeSeparators(app_path + QDir::separator() + file_name);

    QSettings settings(config_path, QSettings::IniFormat);
    QString gate_host = settings.value("GateServer/host").toString();
    QString gate_port = settings.value("GateServer/port").toString();

    gate_server_prefix = "http://" + gate_host + ":" + gate_port;
    MainWindow w;
    w.show();
    return a.exec();
}
