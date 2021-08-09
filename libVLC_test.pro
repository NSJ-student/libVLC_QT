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

PKGCONFIG += Qt5GStreamer-1.0 Qt5GStreamerUi-1.0 Qt5GStreamerUtils-1.0 gstreamer-1.0 gstreamer-video-1.0 gtk+-3.0

INCLUDEPATH += /usr/local/include/opencv4
LIBS += -L/usr/local/lib -lopencv_shape -lopencv_videoio -lopencv_imgcodecs
LIBS += -lopencv_highgui -lopencv_core -lopencv_imgproc -lopencv_features2d -lopencv_calib3d

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
