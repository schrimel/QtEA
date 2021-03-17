//std-c++ includes
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

//qt includes
#include <QApplication>
#include <QtWebEngineWidgets>

//user includes
#include "qtea.h"
#include "webview.h"
#include "focusagent.h"
#include "platform.hpp"
#include "misc_functions.hpp"


#if defined(WINDOWS)
#include <windows.h>
#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "comdlg32.lib")
#endif

const std::string STD_CONFIG_FILE = "res/example-url.conf";

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
                    /*
                        TODO:
                        At the moment, this would cause an error (formatting problem):
                        url =
                        "http://abc4711.com"
                    */
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

void wait(int ms)
{
    QTime dieTime = QTime::currentTime().addMSecs(ms);
    while(QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents,100);
}


int main(int argc, char *argv[])
{
    //TODO read config file from command line
    std::string wBaseUrl = getUrlFromConfig(STD_CONFIG_FILE);

    //TODO calculate checksum

    //platform-specific! --> WIN BEGIN
    HDESK hOld = GetThreadDesktop(GetCurrentThreadId());

    HDESK desk = CreateDesktop((LPCWSTR)"QtEA", NULL, NULL, 0, GENERIC_ALL, NULL);
    //SwitchDesktop(desk);
    //SetThreadDesktop(desk);

    //platform-specific! --> WIN END


    QCoreApplication::setOrganizationName("QtEA");
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication a(argc, argv);

    QtEA w(nullptr, wBaseUrl);;
    w.setWindowFlags(Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint );

    FocusAgent fa;
    QObject::connect(&fa, &FocusAgent::requestFocusInformation, &w, &QtEA::onFocusInformationRequested);
    QObject::connect(&w, &QtEA::sendFocusInformation, &fa, &FocusAgent::setFocusInformation);
    QObject::connect(&w, &QtEA::closeRequest, &fa, &FocusAgent::terminate);


    w.show();
    wait(500);
    w.showFullScreen();

    fa.start(); //TODO implement a starting condition

    int ret = a.exec();
    fa.terminate();
    fa.quit();
    fa.wait();

    //platform-specific! --> WIN BEGIN
    //SwitchDesktop(hOld);
    //CloseDesktop(desk);
    //platform-specific! --> WIN END

    return ret;
}
