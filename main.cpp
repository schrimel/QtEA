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
#include <QElapsedTimer>

//user includes
#include "qtea.h"
#include "webview.h"
#include "focusagent.h"
#include "misc_functions.hpp"
#include "keyboardagent.h"
#include "clipboardagent.h"
#include "networkagent.h"
#include "focusagentii.h"

#if defined(Q_OS_WIN32)
#include <windows.h>
#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "comdlg32.lib")
#endif

const QString STD_CONFIG_FILE = "res/example-url-ssl.conf";
QString title;
QString baseUrl;
bool measurePerformance = false;

void readConfigFile(const QString& iFilename)
{
    QFile wConfigFile(iFilename);
    wConfigFile.open(QFile::OpenModeFlag::ReadOnly);
    QTextStream wTStream(&wConfigFile);

    QString wLine;

    while(wTStream.readLineInto(&wLine))
    {
        QStringList li = wLine.split('=');
        qDebug() << li;
        if(li.size() < 2){
            //invalid format
            continue;
        }
        else
        {
            QString key = li.at(0).trimmed();
            if(key == "url")
            {
                baseUrl = li.at(1).trimmed();
                qDebug() << baseUrl;
            }
            else if(key == "title")
            {
                title = li.at(1).trimmed();
            }
            else if(key == "measurePerformance")
            {
                measurePerformance = true;
            }
        }
    }
    if(baseUrl.isEmpty())
    {
#if defined(QT_DEBUG)
        qDebug() << "Could not read URL.";
#endif
        bool ok;
        QString title("URL missing!");
        QString message("The configuration file is not correct as it contains no URL.");
        int c=0;
        char**v;
        QApplication app(c,v);
        QMessageBox b( QMessageBox::Icon::Critical,title, message,QMessageBox::NoButton,nullptr,Qt::Dialog);
        b.addButton(QMessageBox::StandardButton::Ok);
        b.show();
        b.exec();
        exit(0);
        //baseUrl = "https://en.wikipedia.org/wiki/HTTP_404";
    }
    wConfigFile.close();
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


void initializeUi(QtEA & qtea)
{
#if defined(Q_OS_LINUX)
    qtea.setWindowFlags(Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint | Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint);
#elif defined(Q_OS_WIN32)
    qtea.setWindowFlags(Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint | Qt::WindowStaysOnTopHint);
#endif
}

void setupScreens(QApplication & app, QWidget** widgetArray)
{
    //todo: screens that are connected afterwards (screenagent)

    QScreen* visited[10];
    visited[0] = app.primaryScreen();
    int screenCount = app.screens().size();

    while(screenCount > 1)
    {
        QWidget * widget = new QWidget();
        widgetArray[app.screens().size()-screenCount+1] = widget;
    #if defined(Q_OS_LINUX)
            widget->setWindowFlags(Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint | Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint);
    #elif defined(Q_OS_WIN32)
            widget->setWindowFlags(Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint | Qt::WindowStaysOnTopHint);
    #endif
        widget->show();
        widget->setAttribute(Qt::WA_QuitOnClose, false);

        QScreen* secondScreen;
        QList<QScreen*> screens = app.screens();
        for(QScreen* screen : screens)
        {
            bool contains = false;
            for(int i = 0; i < app.screens().size()-screenCount+1;++i)
            {
                if(visited[i] == screen)
                 {
                    contains = true;
                    break;
                 }
            }
            if(!contains)
            {
                secondScreen = screen;
                visited[app.screens().size()-screenCount+1] = screen;
            }
        }

        widget->windowHandle()->setScreen(secondScreen);
        widget->showFullScreen();
        screenCount--;
    }
}


int main(int argc, char *argv[])
{
    QElapsedTimer timer;
    timer.start();

    if(argc > 1)
    {
        readConfigFile(argv[1]);
    }
    else
    {
        readConfigFile(STD_CONFIG_FILE);
    }

    qint64 t_cfg = timer.elapsed();
    //TODO calculate checksum

    QCoreApplication::setOrganizationName("QtEA");
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication a(argc, argv);
    QWebEngineProfile::defaultProfile()->setUseForGlobalCertificateVerification();
    QtEA w(nullptr, baseUrl);


#ifdef QT_DEBUG
    qDebug() << baseUrl;
    qDebug() << a.screens().size();
#endif

    initializeUi(w);
    qint64 t_qtea = timer.elapsed();
    //Initialize Qt-EApp Security Components

    FocusAgentII faii;
    QObject::connect(&a, &QApplication::applicationStateChanged, &faii, &FocusAgentII::onFocusLost);
    QObject::connect(&faii, &FocusAgentII::focusLost, &w, &QtEA::onFocusLost);

    KeyboardAgent ka;
    QObject::connect(&w, &QtEA::closeRequest, &ka, &KeyboardAgent::terminate);

    ClipboardAgent ca;
    QClipboard *qc = QApplication::clipboard();
    QObject::connect(qc, &QClipboard::dataChanged, &ca, &ClipboardAgent::processClipboardChange);

    NetworkAgent na;
    QObject::connect(&na, &NetworkAgent::networkInterfacesChanged, &w, &QtEA::onNetworkInterfacesChanged);
    QObject::connect(&w, &QtEA::closeRequest, &na, &NetworkAgent::terminate);

    qint64 t_security = timer.elapsed();

    na.start();
    ka.start();

    w.show();
    w.showFullScreen();
    QWidget* widgetArray[10];   //assuming that 10 additional screens are enough
    setupScreens(a, widgetArray);

    qint64 t_browser = timer.elapsed();

    qDebug() << "P: config: " << t_cfg;
    qDebug() << "P: qtea: " << t_qtea-t_cfg;
    qDebug() << "P: security: " << t_security-t_qtea;
    qDebug() << "P: browser: " << t_browser-t_security;

    if(measurePerformance)
    {

        wait(500);
        exit(0);
    }
    w.startBrowser();

    int ret = a.exec();
    for(QWidget* w : widgetArray)
    {
        if(w != nullptr)
            delete w;
    }

    ka.terminate();
    na.terminate();
    na.wait(5000);

    return ret;
}
