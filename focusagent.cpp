//std-c++ includes
#include <iostream>
#include <chrono>
#include <thread>

//qt includes
#include <QMutexLocker>
#include <QMutex>


//user includes
#include "screencapture.h"
#include "focusagent.h"


/**REWORK NEEDED :    QObject::connect(qApp, &QGuiApplication::applicationStateChanged, this, [=](Qt::ApplicationState state){
        qDebug() << state;
    });
*/

FocusAgent::FocusAgent() : mFocusInformation(false), mIsRunning(false), mScreenshotCounter(0)
{
    mutex = new QMutex();
}

void FocusAgent::setFocusInformation(bool iFocusInformation)
{
    mFocusInformation = iFocusInformation;
}

void FocusAgent::workerFunc()
{
    mIsRunning = true;
    std::this_thread::sleep_for(std::chrono::milliseconds(10000));
    while(true)
    {
       if(!mFocusInformation)
       {
           Screencapture sc;
           sc.saveScreenshotToFile("tmp" + std::to_string(mScreenshotCounter) + ".png"); //TODO blur screenshot
           //TODO send Screenshot to Server --> Javascript injection?
           //TODO create Log-entry
           emit focusLost("Application lost focus."); //TODO: add a timestamp - either here or in agentcontroller.h
           mScreenshotCounter++;

       }
       QMutexLocker lock(mutex);
       if(!mIsRunning)
       {
           break;
       }
       std::this_thread::sleep_for(std::chrono::milliseconds(20000)); //wait half a minute before checking again
       emit requestFocusInformation();
    }
}

void FocusAgent::terminate()
{
    mIsRunning = false;
}
