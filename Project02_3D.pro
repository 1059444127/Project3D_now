#-------------------------------------------------
#
# Project created by QtCreator 2015-07-17T22:34:37
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Project02_3D
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    ImageLabel.cpp \
    CaptureDialog.cpp \
    VideoInput.cpp \
    io_util.cpp \
    ProjectorWidget.cpp \
    structured_light.cpp

HEADERS  += mainwindow.h \
    ImageLabel.hpp \
    CaptureDialog.hpp \
    VideoInput.hpp \
    Okapi32.h \
    io_util.hpp \
    ProjectorWidget.hpp \
    structured_light.hpp


FORMS    += mainwindow.ui \
    CaptureDialog.ui
INCLUDEPATH+=F:\opencv2.4.4\opencv\release\install\include\
F:\opencv2.4.4\opencv\release\install\include\opencv\
F:\opencv2.4.4\opencv\release\install\include\opencv2
LIBS +=D:\Qt_test\Project02_3D\OKAPI32.lib\
       F:\opencv2.4.4\opencv\release\install\lib\libopencv_core244d.dll.a\
       F:\opencv2.4.4\opencv\release\install\lib\libopencv_highgui244d.dll.a\
       F:\opencv2.4.4\opencv\release\install\lib\libopencv_imgproc244d.dll.a
