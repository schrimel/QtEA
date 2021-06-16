#include "focusagentii.h"
#include "screencapture.h"

#include <QDebug>
#include <QApplication>
#include <QWidget>
#include <QString>

#include <iostream>
#include <string>
#include <thread>
#include <chrono>

#if defined(Q_OS_WINDOWS)

#include <windows.h>

bool isOverlapped(HWND hwnd);
void screenshotOverlappingWindows(HWND);

#elif defined(Q_OS_LINUX)

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <QX11Info>
#include <QProcess>
#include <QProcessEnvironment>
#include <QThread>

#endif

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
    WId hwnd = QApplication::activeWindow()->winId();
    qDebug() << hwnd;
    isOverlapped(hwnd);
    //HWND hwnd = (HWND)QApplication::activeWindow()->winId();
    //qDebug() << isOverlapped(hwnd);
    //screenshotOverlappingWindows(hwnd);
    //TODO send Screenshot to Server --> Javascript injection?
    //TODO create Log-entry
    emit focusLost("Application lost focus."); //TODO: add a timestamp - either here or in agentcontroller.h
    mScreenshotCounter++;
}

#if defined(Q_OS_WINDOWS)
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

#endif

#if defined(Q_OS_LINUX)

bool FocusAgentII::isOverlapped(WId)
{
    bool isOverlapped = false;
    QString xprop = "/home/osboxes/Desktop/qtea/scripts/qteawininfo.sh";
    QProcess pro;
    pro.start(xprop);
    pro.waitForFinished();
    QString out(pro.readAllStandardOutput());
    QString cut = out.split("#")[1];
    QStringList winIds= cut.replace(",","").left(cut.size()-1).split(" ", Qt::SkipEmptyParts);

    int myAppIndex = -1;

    for(int i = 0; i < winIds.size(); ++i)
    {
        bool ok = false;
        qDebug() << winIds[i];
        unsigned int winId = winIds[i].toUInt(&ok, 16);
        if(!ok)
            qDebug() << "something went wrong";

        if(QApplication::activeWindow()->winId() == winId)
        {
            myAppIndex = i;
            continue;
        }

        if(myAppIndex != -1)
        {
            QString xprop = "xprop";
            QStringList arguments;
            QProcess retrieveWinInfo;

            arguments << "-id" << winIds[i];

            retrieveWinInfo.start(xprop, arguments);
            retrieveWinInfo.waitForFinished();
            QString out = retrieveWinInfo.readAll();
            QStringList lines = out.split("\n");

            int wmStateNextTwoLines = 0, wmNormalNextTwoLines = 0;
            bool stateNormal = false;
            int width=-1, height=-1, x=-1, y=-1;
            QString name;
            for(QString line : lines)
            {
                if(wmStateNextTwoLines > 0)
                {
                   if(line.contains("window state"))
                   {
                       QString state = line.split(":")[1].trimmed().toLower();
                       if(state == "normal")
                       {
                           //here is something to do --> illegal use
                           stateNormal = true;
                       }
                       wmStateNextTwoLines = 0;
                       continue;
                   }
                   wmStateNextTwoLines--;
                }

//                if(wmNormalNextTwoLines > 0)
//                {
//                    if(line.contains("location"))
//                    {
//                        QStringList pos = line.split(":")[1].trimmed().split(", ");
//                        x = pos[0].toInt(&ok, 10);
//                        if(!ok)
//                            qDebug() << "something went wrong";
//                        y = pos[1].toInt(&ok, 10);
//                        if(!ok)
//                            qDebug() << "something went wrong";
//                    }
//                    else if(line.contains("size"))
//                    {
//                        QStringList size = line.split(":")[1].trimmed().split(" by ");
//                        width = size[0].toInt(&ok, 10);
//                        if(!ok)
//                            qDebug() << "something went wrong";
//                        height = size[1].toInt(&ok, 10);
//                        if(!ok)
//                            qDebug() << "something went wrong";
//                    }
//                    wmNormalNextTwoLines--;
//                }

                if(line.startsWith("WM_STATE"))
                {
                    qDebug() << line;
                    wmStateNextTwoLines = 2;
                }
//                if(line.startsWith("WM_NORMAL_HINTS"))
//                {
//                    wmNormalNextTwoLines = 2;
//                }
                if(line.startsWith("WM_NAME"))
                {
                    name = line.split(" = ")[1].left(20); //take first 20 chars from title
                }
            }
            //check window state
            if(stateNormal)
            {
                if(x == -1 || y == -1 || width == -1 || height == -1)
                {
                    QProcess xwininfoProc;
                    QString xwininfo("xwininfo");
                    QStringList args;
                    args << "-id" << winIds[i];
                    xwininfoProc.start(xwininfo, args);
                    xwininfoProc.waitForFinished();
                    QStringList winInfoRet = QString(xwininfoProc.readAll()).split("\n");
                    for(QString infoLine : winInfoRet)
                    {
                        if(infoLine.contains("Absolute upper-left X"))
                        {
                            x = infoLine.split(":")[1].trimmed().toInt(&ok, 10);
                        }
                        if(infoLine.contains("Absolute upper-left Y"))
                        {
                            y = infoLine.split(":")[1].trimmed().toInt(&ok, 10);
                        }
                        if(infoLine.contains("Width"))
                        {
                            width = infoLine.split(":")[1].trimmed().toInt(&ok, 10);
                        }
                        if(infoLine.contains("Height"))
                        {
                            height = infoLine.split(":")[1].trimmed().toInt(&ok, 10);
                            break; //height comes last...
                        }
                    }
                }
                QString position = "(" + QString::number(x) + "," + QString::number(y) + ")";
                qDebug() << position;
                tagRECT tr;
                tr.top = std::max(0,y);
                tr.left = std::max(0,x);
                tr.right = std::min(1919,x + width); //todo: get screen height/width from CLI
                tr.bottom = std::min(1199,y + height);
                qDebug() << name;
                QString filename = "/home/osboxes/Desktop/screenshot-" + name +".png";
                Screencapture sc;
                sc.screenshotOverlapToFile(&tr, filename.toStdString());
                isOverlapped = true;
                stateNormal = false;
            }
        }
    }
    return isOverlapped;
}

//bool FocusAgentII::isOverlapped()
//{
//    Display* display = QX11Info::display();
//    auto root = XDefaultRootWindow(display);
//    Window parent;
//    Window *children;
//    unsigned nchildren;

//    qDebug() << "'HWND'";
//    qDebug() << QApplication::activeWindow()->winId();
//    qDebug() << "END 'HWND'";
//    if(!XQueryTree(display, root, &root, &parent, &children, &nchildren))
//    {
//        qDebug() << "query failed";
//    }


//    qDebug() << "root";
//    qDebug() << parent;
//    qDebug() << "end root";

//    int myAppIndex = -1;
//    int myAppX = 0;
//    int myAppY = 0;
//    int myAppWidth = 0;
//    int myAppHeight = 0;
//    qDebug() << nchildren;
//    for(size_t i = 0; i < nchildren; ++i)
//    {
//        auto window = children[i];
//        Window p;
//        Window* windowsChilds;
//        unsigned noOfChilds = 0;
//        if(!XQueryTree(display, window, &window, &p, &windowsChilds, &noOfChilds))
//        {
//            qDebug() << "??";
//        }
//        for(size_t j = 0; j < noOfChilds; ++j)
//        {
//            if(QApplication::activeWindow()->winId() == windowsChilds[j])
//            {
//                XWindowAttributes * window_attributes_ret = new XWindowAttributes();
//                myAppIndex = i;
//                XGetWindowAttributes(display, windowsChilds[j], window_attributes_ret);
//                myAppHeight = window_attributes_ret->height;
//                myAppWidth = window_attributes_ret->width;
//                myAppX = window_attributes_ret->x;
//                myAppY = window_attributes_ret->y;
//                delete window_attributes_ret;
//            }
//        }

//        if(myAppIndex != -1 && i > myAppIndex)
//        {
//            qDebug() << QString::number(window);
//            QString xwininfo = "xwininfo";
//            QStringList arguments;
//            arguments << "-id" << QString::number(window);
//            QProcess pro;
//            pro.start(xwininfo, arguments);
//            pro.waitForFinished();
//            QString out(pro.readAllStandardOutput());
//            qDebug() << out;
//            QString out2(pro.readAll());
//            qDebug() << out2;
//        }
//    }

//    XFree(children);
//    return true;
//}

#endif

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
