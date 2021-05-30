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
void screenshotOverlappingWindows(HWND);

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
    //qDebug() << isOverlapped(hwnd);
    screenshotOverlappingWindows(hwnd);
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

/* UNDER CONSTRUCTION*/
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
/*
#if defined(Q_OS_LINUX)
bool isOverlapped(const void* hWnd, _XDisplay* pd)
{
    Display* pDisplay = pd == nullptr ? XOpenDisplay(nullptr) : pd;
    if(pDisplay == nullptr)
    {
        return true;
    }
    auto root = XDefaultRootWindow(pDisplay);
    Window parent;
    /*Window* windowList;
    unsigned nchildren;
    if (!XQueryTree(pDisplay, root, &root, &parent, &windowList, &nchildren))
    {
        if(pd == nullptr)
        {
            XCloseDisplay(pDisplay);
        }
        return true;
    }*7/

    Atom propCleints = XInternAtom(pDisplay, "_NET_CLIENT_LIST_STACKING", True);
    unsigned long ulBytesReturned = 0;
    Window *windowList = (Window *)GetWindowProperty(pDisplay, root, propCleints, &ulBytesReturned);
    unsigned long nchildren = ulBytesReturned / sizeof(Window);
    int32_t actualDesktop = GetWindowDesktop(pDisplay, (TWindow) hWnd);

    WindowRect targetWindowRect;
    GetWindowRect(hWnd, targetWindowRect, pDisplay);
    GdkRectangle targetGdkRect;
    targetGdkRect.x = targetWindowRect.x;
    targetGdkRect.y = targetWindowRect.y;
    targetGdkRect.width = targetWindowRect.width;
    targetGdkRect.height = targetWindowRect.height;
    bool handleWindow = false;
    bool res = false;
    for (unsigned long i = 0; i < nchildren; ++i)
    {
        auto window = windowList[i];
        if((Window)hWnd == window)
        {
            handleWindow = true;
            continue;
        }
        if(handleWindow)
        {
            if((GetWindowDesktop(pDisplay, window) != actualDesktop) || IsWindowHidden((void*)window, pDisplay))
            {
                continue;
            }
            else
            {
                WindowRect rc;
                GetWindowRect((void*)window, rc, pDisplay);
                GdkRectangle gdkRect;
                gdkRect.x = rc.x;
                gdkRect.y = rc.y;
                gdkRect.width = rc.width;
                gdkRect.height = rc.height;
                if(gdk_rectangle_intersect(&targetGdkRect, &gdkRect, nullptr))
                {
                    res = true;
                    break;
                }
            }
        }
    }
    XFree(windowList);
    if(pd == nullptr)
    {
        XCloseDisplay(pDisplay);
    }

    return res;
}

#endif
*/
