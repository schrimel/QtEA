QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11 file_copies

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    focusagent.cpp \
    main.cpp \
    qtea.cpp \
    screencapture.cpp \
    webview.cpp

HEADERS += \
    focusagent.h \
    misc_functions.hpp \
    platform.hpp \
    qtea.h \
    screencapture.h \
    webview.h

FORMS += \
    qtea.ui

QT += webenginewidgets core

COPIES += install_it

LIBS += "C:/lib/opencv/build/x64/vc15/lib/opencv_world451.lib"

INCLUDEPATH += "C:/lib/opencv/build/include"
DEPENDPATH += "C:/lib/opencv/build/include"

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

install_it.path = $$OUT_PWD/res
install_it.files += $$files(res/*.conf)
