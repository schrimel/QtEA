#include "focusagentii.h"
#include "screencapture.h"

#include <QDebug>
#include <QApplication>
#include <QWidget>
#include <QString>

#include <iostream>
#include <string>
#include <windows.h>
#include <thread>
#include <chrono>

bool isOverlapped(HWND hwnd);

FocusAgentII::FocusAgentII(QObject *parent) : QObject(parent), mScreenshotCounter(0)
{

}

void FocusAgentII::onFocusLost(Qt::ApplicationState state)
{
#ifdef QT_DEBUG
    qDebug() << state;
#endif
    if(state == Qt::ApplicationState::ApplicationActive)
        return;
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    //Screencapture sc;
    //sc.saveScreenshotToFile("tmp" + std::to_string(mScreenshotCounter) + ".png"); //TODO blur screenshot
    HWND hwnd = (HWND)QApplication::activeWindow()->winId();
    qDebug() << isOverlapped(hwnd);
    //TODO send Screenshot to Server --> Javascript injection?
    //TODO create Log-entry
    emit focusLost("Application lost focus."); //TODO: add a timestamp - either here or in agentcontroller.h
    mScreenshotCounter++;
}

bool isOverlapped(HWND hwnd)
{
    std::set<HWND> visited = {hwnd};
    tagRECT* thisRect = new tagRECT();
    GetWindowRect(hwnd, thisRect);

    while((hwnd=GetWindow(hwnd, GW_HWNDPREV)) != NULL && visited.find(hwnd) == visited.end())
    {
        visited.insert(hwnd);
        tagRECT* testRect = new tagRECT();
        tagRECT* intersection = new tagRECT();
        if(IsWindowVisible(hwnd) && GetWindowRect(hwnd, testRect) && IntersectRect(intersection, thisRect, testRect))
        {
            LPSTR *lpString = new LPSTR();
            GetWindowTextA(hwnd, *lpString, 32);
            qDebug() << intersection->top;
            qDebug() << intersection->left;

            tagRECT* region = new tagRECT();
            memcpy(region, intersection, sizeof(tagRECT));
            Screencapture sc;
            sc.screenshotOverlapToFile(region, "blurredout.png");
            return true;
        }
        delete testRect;
        delete intersection;
    }
    return false;
}

/* UNDER CONSTRUCTION
void screenshotOverlappingWindows(HWND hwnd)
{
    std::set<HWND> visited = {hwnd};
    tagRECT* thisRect = new tagRECT();
    GetWindowRect(hwnd, thisRect);
    int wScreenshotCounter = 0;

    while((hwnd=GetWindow(hwnd, GW_HWNDPREV)) != NULL && visited.find(hwnd) == visited.end())
    {
        visited.insert(hwnd);
        tagRECT* testRect = new tagRECT();
        tagRECT* intersection = new tagRECT();
        if(IsWindowVisible(hwnd) && GetWindowRect(hwnd, testRect) && IntersectRect(intersection, thisRect, testRect))
        {
            tagRECT* region = new tagRECT();
            memcpy(region, intersection, sizeof(tagRECT));
            Screencapture sc;
            std::string wFilename = "blurredout" + std::to_string(wScreenshotCounter) + ".png";
            sc.screenshotOverlapToFile(region, wFilename);
            wScreenshotCounter++;
        }
        delete testRect;
        delete intersection;
    }
}
*/
#if defined(Q_OS_LINUX)
#endif

