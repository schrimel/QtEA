#include "networkagent.h"
#include <QDebug>
#include <QApplication>

NetworkAgent::NetworkAgent()
{
    mInitialNetworks = QNetworkInterface::allAddresses();
}

bool NetworkAgent::startMonitoring()
{
    if(isExamNetworkInList())
    {
        mIsRunning = true;
        monitorNetworkInterfaces();
        return true;
    }
    else
    {
        return false;
    }
}

void NetworkAgent::run()
{
    if(!startMonitoring())
    {
        emit serverNotReachable();
    }

    //if it returns false --> emit signal which will cause an error message :-)
}


void NetworkAgent::monitorNetworkInterfaces()
{
    while(mIsRunning)
    {
        QList<QHostAddress> currentNetworkInterfaces = QNetworkInterface::allAddresses();

        for(QHostAddress a : currentNetworkInterfaces)
        {
            if(a.isLoopback());
            if(!mInitialNetworks.contains(a)){
                qDebug() << "new network detected";
                emit networkInterfacesChanged();
                mInitialNetworks.append(a);
            }
        }
        QList<QNetworkInterface> nwis = QNetworkInterface::allInterfaces();
        for(QNetworkInterface n : nwis)
        {
            unsigned int flags = n.flags();
            bool isLoopback = (bool)(flags & QNetworkInterface::IsLoopBack);
            bool isP2P = (bool)(flags & QNetworkInterface::IsPointToPoint);
            bool isRunning = (bool)(flags & QNetworkInterface::IsRunning);

            //ignore all invalid, not running or local network interfaces
            if(!isRunning || isLoopback ||!n.isValid())
                continue;
            //p2p could be a connection between students inside of the exam network
            //analyze ip: check if p2p ip is in the exam network
            if(isP2P)
                continue;
        }
        QThread::sleep(1000);
    }
}

bool NetworkAgent::isExamNetworkInList()
{
    return true;
}

void NetworkAgent::terminate()
{
    mIsRunning = false;
}


//bool isExamNetworkInList(); //imagine having a student that has not connected to the network KEKW
