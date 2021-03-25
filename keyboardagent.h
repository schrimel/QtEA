#ifndef KEYBOARDAGENT_H
#define KEYBOARDAGENT_H

#include <QObject>
#include <QtGlobal>

#if defined(Q_OS_WIN32)
#include <Windows.h>
#endif

namespace std
{
    class thread;
}
class KeyboardAgent : public QObject
{
    Q_OBJECT
public:
    KeyboardAgent();
    void start();
private:
    volatile bool mIsRunning;
#if defined(Q_OS_WIN32)
    HHOOK setHook();
    void unsetHook(HHOOK hOld);
#elif defined(Q_OS_LINUX)
    void setHook();
    void unsetHook();
    //void workerFunc();
    //std::thread *mythread;
#endif
public slots:
    void terminate();
};


#endif // KEYBOARDAGENT_H
