QT       += core gui network widgets

greaterThan(QT_MAJOR_VERSION, 5): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    loginwidget.cpp \
    registerwidget.cpp \
    chatwindow.cpp \
    contactlistwidget.cpp \
    grouplistwidget.cpp \
    tcpclient.cpp \
    httpclient.cpp \
    usermanager.cpp \
    messagemanager.cpp \
    sessionmanager.cpp \
    bubbleframe.cpp \
    textbubble.cpp \
    picturebubble.cpp \
    chatitembase.cpp \
    searchuserdialog.cpp \
    friendrequestdialog.cpp \
    groupinfodialog.cpp

HEADERS += \
    mainwindow.h \
    loginwidget.h \
    registerwidget.h \
    chatwindow.h \
    contactlistwidget.h \
    grouplistwidget.h \
    tcpclient.h \
    httpclient.h \
    usermanager.h \
    messagemanager.h \
    sessionmanager.h \
    bubbleframe.h \
    textbubble.h \
    picturebubble.h \
    chatitembase.h \
    searchuserdialog.h \
    friendrequestdialog.h \
    groupinfodialog.h

FORMS += \
    searchuserdialog.ui \
    friendrequestdialog.ui \
    groupinfodialog.ui

RESOURCES += \
    resource.qrc

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
