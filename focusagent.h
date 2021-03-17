#ifndef FOCUSAGENT_H
#define FOCUSAGENT_H

#include <QThread>

class QMutex;
class QPixmap;

class FocusAgent : public QThread
{
    Q_OBJECT
public:
    explicit FocusAgent();
    void run() override
    {
        workerFunc();
    };

private:
    bool mFocusInformation;
    bool mIsRunning;
    QMutex *mutex;
    int mScreenshotCounter;
    QPixmap grabScreens();
    void workerFunc();

signals:
    void requestFocusInformation();

public slots:
    void setFocusInformation(bool);
    void terminate();



};

#endif // FOCUSAGENT_H
