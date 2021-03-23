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
       std::cout << "Has focus: " << mFocusInformation << std::endl;
       if(!mFocusInformation)
       {
           Screencapture sc;
           sc.saveScreenshotToFile("tmp" + std::to_string(mScreenshotCounter) + ".png"); //TODO blur screenshot
           //TODO send Screenshot to Server --> Javascript injection?
           //TODO create Log-entry


           mScreenshotCounter++;

       }
       QMutexLocker lock(mutex);
       if(!mIsRunning)
       {
           break;
       }
       std::this_thread::sleep_for(std::chrono::milliseconds(2000)); //wait half a minute before checking again
       emit requestFocusInformation();
    }
}

void FocusAgent::terminate()
{
    QMutexLocker lock(mutex);
    mIsRunning = false;
}
