//std-c++ includes
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <chrono>

//qt includes
#include <QtGlobal>
#include <QApplication>
#include <QtWebEngineWidgets>
#include <QNetworkInterface>
#include <QDebug>

//user includes
#include "qtea.h"
#include "webview.h"
#include "focusagent.h"
#include "misc_functions.hpp"
#include "keyboardagent.h"
#include "clipboardagent.h"
//#include "keyboardfilter.h"
#include "networkagent.h"
#include "focusagentii.h"


#if defined(Q_OS_WIN32)
#include <windows.h>
#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "comdlg32.lib")
#endif

const std::string STD_CONFIG_FILE = "res/example-url-ssl.conf";

std::string getUrlFromConfig(const std::string & iFilename)
{    
    std::ifstream wConfigFile(iFilename);
    std::string wLine;
    std::string rUrl;
    while(std::getline(wConfigFile, wLine))
    {
        std::istringstream wBuffer(wLine);
        std::string key, value;
        if(std::getline(wBuffer, key, '='))
        {
            if(key.compare("url"))
            {
                if(std::getline(wBuffer, value)){
                    rUrl = value;
                    break;
                }
                else
                {
                    std::cerr << "Config file syntax is not correct." << std::endl;
                    return "https://en.wikipedia.org/wiki/HTTP_404";
                }
            }
            else if(key.compare("title"))
            {
                if(std::getline(wBuffer, value))
                {

                }
            }
        }

    }
    if(rUrl.empty())
    {
        std::cerr << "Could not read URL." << std::endl;
        return "https://en.wikipedia.org/wiki/HTTP_404";
    }
    misc::trim(rUrl);
    return rUrl;
}

#if defined(Q_OS_LINUX)
void signalHandler(int signum)
{

}
#endif
void wait(int ms)
{
    QTime dieTime = QTime::currentTime().addMSecs(ms);
    while(QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents,100);
}


int main(int argc, char *argv[])
{
    //std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    //TODO read config file from command line
    std::string wBaseUrl = getUrlFromConfig(STD_CONFIG_FILE);
    if(argc > 1)
    {
        wBaseUrl = getUrlFromConfig(argv[1]);
    }

    //TODO calculate checksum

#if defined(Q_OS_WIN32)
    //platform-specific! --> WIN BEGIN
    //HDESK hOld = GetThreadDesktop(GetCurrentThreadId());

    //HDESK desk = CreateDesktop((LPCWSTR)"QtEA", NULL, NULL, 0, GENERIC_ALL, NULL);
//    SwitchDesktop(desk);
//    SetThreadDesktop(desk);

    //platform-specific! --> WIN END
#endif

    QCoreApplication::setOrganizationName("QtEA");
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication a(argc, argv);
    QWebEngineProfile::defaultProfile()->setUseForGlobalCertificateVerification();
    QtEA w(nullptr, wBaseUrl);

#ifdef QT_DEBUG
    qDebug() << QString::fromStdString(wBaseUrl);
    qDebug() << a.screens().size();
#endif

#if defined(Q_OS_LINUX)
    w.setWindowFlags(Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint | Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint);
#elif defined(Q_OS_WIN32)
    w.setWindowFlags(Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint | Qt::WindowStaysOnTopHint);
#endif

    //Initialize Qt-EApp Security Components

    /*
    FocusAgent fa;
    QObject::connect(&fa, &FocusAgent::requestFocusInformation, &w, &QtEA::onFocusInformationRequested);
    QObject::connect(&w, &QtEA::sendFocusInformation, &fa, &FocusAgent::setFocusInformation);
    QObject::connect(&w, &QtEA::closeRequest, &fa, &FocusAgent::terminate);*/

    FocusAgentII faii;
    QObject::connect(&a, &QApplication::applicationStateChanged, &faii, &FocusAgentII::onFocusLost);

    KeyboardAgent ka;
    QObject::connect(&w, &QtEA::closeRequest, &ka, &KeyboardAgent::terminate);
    ka.start();

    ClipboardAgent ca;
    QClipboard *qc = QApplication::clipboard();
    QObject::connect(qc, &QClipboard::dataChanged, &ca, &ClipboardAgent::processClipboardChange);
    //DBG: QObject::connect(&ca, &ClipboardAgent::externalClipboardContentDetected, &w, &QtEA::onNetworkInterfacesChanged);

    NetworkAgent na;
    QObject::connect(&na, &NetworkAgent::networkInterfacesChanged, &w, &QtEA::onNetworkInterfacesChanged);
    QObject::connect(&w, &QtEA::closeRequest, &na, &NetworkAgent::terminate);
    na.start();

    //new class eventfilter:
    //a.installEventFilter(new KeyboardFilter());

    w.show();
    //wait(500);
    //w.showFullScreen();

    QWidget * widget = new QWidget();
    if(a.screens().size() > 1)
    {
#if defined(Q_OS_LINUX)
        widget->setWindowFlags(Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint | Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint);
#elif defined(Q_OS_WIN32)
        widget->setWindowFlags(Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint | Qt::WindowStaysOnTopHint);
#endif
        widget->show();
        widget->setAttribute(Qt::WA_QuitOnClose, false);

        QScreen* secondScreen;
        QList<QScreen*> screens = a.screens();
        for(QScreen* screen : screens)
        {
            if(screen != a.primaryScreen())
            {
                secondScreen = screen;
            }
        }

        widget->windowHandle()->setScreen(secondScreen);
        widget->showFullScreen();
    }

    //fa.start(); //TODO implement a starting condition
    //std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    //std::cout << "Time difference" << std::chrono::duration_cast<std::chrono::milliseconds> (end-begin).count() << "[ns]" << std::endl;

    int ret = a.exec();
    /*fa.terminate();
    fa.wait(5000);*/
    ka.terminate();
    na.terminate();
    na.wait(5000);
    delete widget;

    //platform-specific! --> WIN BEGIN
//    SwitchDesktop(hOld);
//    CloseDesktop(desk);
    //platform-specific! --> WIN END

    return ret;
}
