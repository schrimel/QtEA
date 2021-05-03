#include "agentcontroller.h"
#include <QDebug>

AgentController::AgentController(QObject *parent) : QObject(parent)
{

}

void AgentController::handleReportRequest(const QString &report)
{
    qDebug() << report;
}
