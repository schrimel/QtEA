#ifndef NETWORKAGENT_H
#define NETWORKAGENT_H

#include <QThread>
#include <QNetworkInterface>

class NetworkAgent : public QThread
{
    Q_OBJECT
public:
    NetworkAgent();
    void run() override;
private:
    void monitorNetworkInterfaces();
    QList<QHostAddress> mInitialNetworks;
    bool isExamNetworkInList(); //imagine having a student that has not connected to the network KEKW
    bool mIsRunning;

public slots:
    bool startMonitoring();
    void terminate();
signals:
    void networkInterfacesChanged();
    void serverNotReachable();
};

#endif // NETWORKAGENT_H
