#ifndef AGENTCONTROLLER_H
#define AGENTCONTROLLER_H

#include <QObject>

class AgentController : public QObject
{
    Q_OBJECT
public:
    explicit AgentController(QObject *parent = nullptr);

public slots:
    void handleReportRequest(const QString &report);
signals:

};

#endif // AGENTCONTROLLER_H
