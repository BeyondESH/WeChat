#include "mainwindow.h"
#include "testwindow.h"
#include <QApplication>
#include <QFile>
#include "global.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //读取qss
    QFile qssFile(":/style/stylesheet.qss");
    if(qssFile.open(QFile::ReadOnly)){
        qDebug()<<"open file success";
        QString style=QString(qssFile.readAll());
        a.setStyleSheet(style);
        qssFile.close();
    }else{
        qDebug()<<"open file failed";
    }
    //读取config配置文件
    QString fileName="config.ini";
    QString appPath=QCoreApplication::applicationDirPath();
    QString config_path=QDir::toNativeSeparators(appPath+QDir::separator()+fileName);
    //qDebug()<<config_path;
    QSettings settings(config_path,QSettings::IniFormat);
    //配置GateServer地址
    QString gateHost=settings.value("GateServer/host").toString();
    QString gatePort=settings.value("GateServer/port").toString();
    //qDebug()<<"http://"+gateHost+":"+gatePort;
    gate_url_prefix="http://"+gateHost+":"+gatePort;

    MainWindow w;
    w.show();
    // testWindow::registerdialog(w);
    // testWindow::logindialog(w);
    // testWindow::chatdialog(w);
    //testWindow::messageWidget(w);
    return a.exec();
}
