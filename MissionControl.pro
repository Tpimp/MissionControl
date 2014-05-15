#-------------------------------------------------
#
# Project created by QtCreator 2014-04-03T11:07:18
#
#-------------------------------------------------


greaterThan(QT_MAJOR_VERSION, 4): QT +=  widgets multimedia

TARGET = MissionControl
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    configdialog.cpp \
    serverdialog.cpp \
    Tier2_Business/clientconnectionmanager.cpp \
    Tier2_Business/requestmanager.cpp \
    Tier2_Business/streamtransferthread.cpp \
    Tier3_Persistance/stream.cpp \
    Tier3_Persistance/videolocator.cpp \
    filetransferprogress.cpp \
    Tier3_Persistance/videoinforeader.cpp \
    Tier3_Persistance/videowriterprocess.cpp \
    Tier2_Business/videorecordingmanager.cpp

HEADERS  += mainwindow.h \
    configdialog.h \
    serverdialog.h \
    Tier2_Business/clientconnectionmanager.h \
    Tier2_Business/requestmanager.h \
    Tier2_Business/streamtransferthread.h \
    Tier3_Persistance/stream.h \
    Tier3_Persistance/videolocator.h \
    filetransferprogress.h \
    Tier3_Persistance/videoinforeader.h \
    Tier3_Persistance/videowriterprocess.h \
    Tier2_Business/videorecordingmanager.h

FORMS    += mainwindow.ui \
    configdialog.ui \
    serverdialog.ui \
    filetransferprogress.ui

CONFIG += C++11

RESOURCES += \
    images.qrc
