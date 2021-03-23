#ifndef KEYBOARDAGENT_H
#define KEYBOARDAGENT_H

#include <QObject>
#include <Windows.h>
class KeyboardAgent : public QObject
{
    Q_OBJECT
public:
    KeyboardAgent();
    void start();
private:
    volatile bool mIsRunning;
    HHOOK setHook();
    void unsetHook(HHOOK hOld);
public slots:
    void terminate();
};
#endif // KEYBOARDAGENT_H
