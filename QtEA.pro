QT       += core gui testlib

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11 file_copies testcase

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

RC_ICONS = res/background_img.ico

SOURCES += \
    agentcontroller.cpp \
    clipboardagent.cpp \
    focusagent.cpp \
    focusagentii.cpp \
    keyboardagent.cpp \
#    keyboardfilter.cpp \
    lockoverlay.cpp \
    main.cpp \
    networkagent.cpp \
    qtea.cpp \
    screencapture.cpp \
    webpage.cpp \
    webview.cpp

HEADERS += \
    agentcontroller.h \
    clipboardagent.h \
    focusagent.h \
    focusagentii.h \
    keyboardagent.h \
#    keyboardfilter.h \
    lockoverlay.h \
    misc_functions.hpp \
    networkagent.h \
    qtea.h \
    screencapture.h \
    webpage.h \
    webview.h

FORMS += \
    certificateerrordialog.ui \
    qtea.ui \
    lockoverlay.ui

QT += webenginewidgets core

unix:!android: QT += x11extras

COPIES += install_it

win32: INCLUDEPATH += "C:/lib/opencv/build/include"
else: unix:!android: INCLUDEPATH += /usr/include/opencv4
win32: DEPENDPATH += "C:/lib/opencv/build/include"
else: unix:!android: DEPENDPATH += /usr/include/opencv4

release{

win32: LIBS += "C:/lib/opencv/build/x64/vc15/lib/opencv_world451.lib"
else: unix:!android: LIBS += -L/usr/lib/x86_64-linux-gnu -lopencv_core -lopencv_calib3d -lopencv_features2d -lopencv_imgproc -lopencv_imgcodecs -lX11

}
debug{
    win32: LIBS += "C:/lib/opencv/build/x64/vc15/lib/opencv_world451d.lib"
    else: unix:!android: LIBS += -L/usr/lib/x86_64-linux-gnu -lopencv_cored -lopencv_calib3dd -lopencv_features2dd -lopencv_imgprocd -lX11

}


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

install_it.path = $$OUT_PWD/res
install_it.files += $$files(res/*.conf)

DISTFILES += \
    diagrams/qtea.qmodel \
    gui.qmodel \
    keycombinations
