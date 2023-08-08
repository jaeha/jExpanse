QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    account.cpp \
    category.cpp \
    infotab.cpp \
    jattachmentwg.cpp \
    jcalendar.cpp \
    jdb.cpp \
    jdbtabledlg.cpp \
    jinputdlg.cpp \
    jlistwidget.cpp \
    jtablewidget.cpp \
    jtextedit.cpp \
    main.cpp \
    mainwindow.cpp \
    payee.cpp \
    searchwg.cpp \
    settingsdlg.cpp \
    trans.cpp \
    transdlg.cpp

HEADERS += \
    account.h \
    category.h \
    common.h \
    infotab.h \
    jattachmentwg.h \
    jcalendar.h \
    jdb.h \
    jdbtabledlg.h \
    jinputdlg.h \
    jlistwidget.h \
    jtablewidget.h \
    jtextedit.h \
    mainwindow.h \
    payee.h \
    searchwg.h \
    settingsdlg.h \
    trans.h \
    transdlg.h

FORMS +=

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resource.qrc

RC_ICONS = images\app.ico
ICON = images\app.icns
