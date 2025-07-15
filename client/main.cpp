#include "mainwindow.h"

#include <QApplication>
#include <QFile>
#include "global.h"
// qss加载改为ui对应的类内载入
void loadStyleSheet(QApplication& app) {
    QDir style_dir(":/style");
    if (!style_dir.exists()) {
        qDebug() << "style resources didn't exist!\n";
    }
    style_dir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
    style_dir.setSorting(QDir::Name);

    QFileInfoList file_list = style_dir.entryInfoList();
    for (int i = 0; i < file_list.size(); i++) {
        QFileInfo file_info = file_list[i];
        QFile qss_file(file_info.filePath());
        if (qss_file.open(QFile::ReadOnly)) {
            qDebug() << "QSS file: " << file_info.fileName() << " open success.\n";
            QString style = QLatin1StringView(qss_file.readAll());
            app.setStyleSheet(style);
            qss_file.close();
        } else {
            qDebug() << "QSS file: " << file_info.fileName() << " open failed, not exist.\n";
        }
    }

}
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // loadStyleSheet(a);
    // QFile qss_file(":/style/register.qss");
    // if (qss_file.open(QFile::ReadOnly)) {
    //     qDebug("Qss file Open success");
    //     QString style = QLatin1String(qss_file.readAll());
    //     a.setStyleSheet(style);
    //     qss_file.close();
    // } else {
    //     qDebug("Qss file Open failed");
    // }

    QString app_path = QCoreApplication::applicationDirPath();
    QString file_name = "config.ini";
    QString config_path = QDir::toNativeSeparators(app_path + QDir::separator() + file_name);

    QSettings settings(config_path, QSettings::IniFormat);
    QString gate_host = settings.value("GateServer/host").toString();
    QString gate_port = settings.value("GateServer/port").toString();

    gate_server_prefix = "http://" + gate_host + ":" + gate_port;
    qDebug() << gate_server_prefix;
    MainWindow w;
    w.loadStyleSheet(":/style/stylesheet.qss");
    w.show();
    return a.exec();
}
