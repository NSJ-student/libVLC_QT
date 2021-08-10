QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets multimedia multimediawidgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

linux-g++ | linux-g++-64 | linux-g++-32 {
QMAKE_LFLAGS += -no-pie
# Tell qmake to use pkg-config to find QtGStreamer.
CONFIG += link_pkgconfig c++11

PKG_CONFIG_PATH=$PKG_CONFIG_PATH:/home/mik21/vlc/build/lib/pkgconfig
PKGCONFIG += libvlc

# sudo apt-get install libvlc-dev
#INCLUDEPATH += /usr/include
#LIBS += -L/usr/lib/aarch64-linux-gnu/vlc -lvlc

QMAKE_INCDIR += /home/mik21/vlc/build/include
QMAKE_LFLAGS += -L/home/mik21/vlc/build/lib -lvlc -lvlccore
QMAKE_LFLAGS += "-Wl,-rpath=/home/mik21/vlc/build/lib"

}

win32 {

PATH+=$$PWD\vlc-3.0.4
SDKPATH=$$PWD\vlc-3.0.4\sdk
INCLUDEPATH += $$SDKPATH\include
# MinGW 32bit
#LIBS      += -L$$SDKPATH\lib -lvlc -lvlccore
# MSVC2019 32bit
LIBS      += -L$$SDKPATH\lib -llibvlc -llibvlccore

#LIBS += $$PWD\vlc-3.0.4\libvlc.dll
#LIBS += $$PWD\vlc-3.0.4\libvlccore.dll

}

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    videolist.cpp \
    vlc_player.cpp

HEADERS += \
    mainwindow.h \
    videolist.h \
    vlc_player.h

FORMS += \
    mainwindow.ui \
    videolist.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
