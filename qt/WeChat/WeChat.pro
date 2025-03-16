QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    chatdialog.cpp \
    chatuserwidget.cpp \
    global.cpp \
    httpmgr.cpp \
    listitembase.cpp \
    logindialog.cpp \
    main.cpp \
    mainwindow.cpp \
    registerdialog.cpp \
    resetdialog.cpp \
    tcpmgr.cpp \
    testwindow.cpp \
    usermgr.cpp

HEADERS += \
    chatdialog.h \
    chatuserwidget.h \
    global.h \
    httpmgr.h \
    listitembase.h \
    logindialog.h \
    mainwindow.h \
    registerdialog.h \
    resetdialog.h \
    singleton.hpp \
    tcpmgr.h \
    testwindow.h \
    usermgr.h

FORMS += \
    chatdialog.ui \
    chatuserwidget.ui \
    logindialog.ui \
    mainwindow.ui \
    registerdialog.ui \
    resetdialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    rc.qrc

DISTFILES += \
    config.ini

# 根据构建配置自动选择目标目录
CONFIG(debug, debug|release) {
targetDir = $$OUT_PWD/Debug
} else {
targetDir = $$OUT_PWD/Release
}

# 在构建时将 config.ini 文件拷贝到目标目录
win32 {
CONFIG_FILE = $$PWD/config.ini
CONFIG_FILE = $$replace(CONFIG_FILE, /, \\)
QMAKE_POST_LINK += copy /Y \"$$CONFIG_FILE\" \"$$targetDir\"
}
