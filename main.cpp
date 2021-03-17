//std-c++ includes
#include <iostream>

//qt includes
#include <QApplication>
#include <QtWebEngineWidgets>

//user includes
#include "qtea.h"
#include "webview.h"
#include "focusagent.h"
#include "platform.hpp"


int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("FLEX");
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication a(argc, argv);

    QtEA w;
    w.setWindowFlags(Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint );

    FocusAgent fa;
    QObject::connect(&fa, &FocusAgent::requestFocusInformation, &w, &QtEA::onFocusInformationRequested);
    QObject::connect(&w, &QtEA::sendFocusInformation, &fa, &FocusAgent::setFocusInformation);

    w.show();
    QTime dieTime = QTime::currentTime().addMSecs(500);
    while(QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents,100);
    w.showFullScreen();

    fa.start(); //TODO implement a starting condition

    fa.terminate();
    int ret = a.exec();
    fa.quit();
    fa.wait();
    return ret;
}
