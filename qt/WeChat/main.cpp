#include "mainwindow.h"
#include <QApplication>
#include <QFile>
#include <QDir>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QCoreApplication::setOrganizationName("DistributedChatSystem");
    QCoreApplication::setApplicationName("WeChat");
    QCoreApplication::setApplicationVersion("1.0.0");

    app.setFont(QFont("Microsoft YaHei", 10));

    QFile styleFile(":/qss/default.qss");
    if (styleFile.open(QFile::ReadOnly)) {
        QString styleSheet = QLatin1String(styleFile.readAll());
        app.setStyleSheet(styleSheet);
        styleFile.close();
    }

    MainWindow mainWindow;
    mainWindow.show();

    return app.exec();
}
