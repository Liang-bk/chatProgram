QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
RC_ICONS = telegram_icon.ico
DESTDIR = ./bin
SOURCES += \
    global.cpp \
    httpmanager.cpp \
    logindialog.cpp \
    main.cpp \
    mainwindow.cpp \
    register_ui/checklineedit.cpp \
    register_ui/passwordlineedit.cpp \
    register_ui/validatedlineedit.cpp \
    register_ui/verificationcodeedit.cpp \
    register_ui/waitingspinnerwidget.cpp \
    registerdialog.cpp

HEADERS += \
    global.h \
    httpmanager.h \
    logindialog.h \
    mainwindow.h \
    register_ui/checklineedit.h \
    register_ui/passwordlineedit.h \
    register_ui/validatedlineedit.h \
    register_ui/verificationcodeedit.h \
    register_ui/waitingspinnerwidget.h \
    registerdialog.h \
    singleton.h

FORMS += \
    logindialog.ui \
    mainwindow.ui \
    registerdialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    rc.qrc

DISTFILES += \
    config.ini

win32:CONFIG(debug, debug | release)
{
    #指定要拷贝的文件目录为工程目录下release目录下的所有dll、lib文件，例如工程目录在D:\QT\Test
    #PWD就为D:/QT/Test，DllFile = D:/QT/Test/release/*.dll
    TargetConfig = $${PWD}/config.ini
    #将输入目录中的"/"替换为"\"
    TargetConfig = $$replace(TargetConfig, /, \\)
    #将输出目录中的"/"替换为"\"
    OutputDir =  $${OUT_PWD}/$${DESTDIR}
    OutputDir = $$replace(OutputDir, /, \\)
    //执行copy命令
    QMAKE_POST_LINK += copy /Y \"$$TargetConfig\" \"$$OutputDir\"
}
