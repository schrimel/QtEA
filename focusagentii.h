#ifndef FOCUSAGENTII_H
#define FOCUSAGENTII_H

#include <QObject>

class FocusAgentII : public QObject
{
    Q_OBJECT
public:
    explicit FocusAgentII(QObject *parent = nullptr);

public slots:
    void onFocusLost(Qt::ApplicationState state);

private:
    int mScreenshotCounter;

signals:
    void focusLost(const QString &info);

};

#endif // FOCUSAGENTII_H
