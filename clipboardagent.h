#ifndef CLIPBOARDAGENT_H
#define CLIPBOARDAGENT_H

#include <QObject>

class ClipboardAgent : public QObject
{
    Q_OBJECT
public:
    explicit ClipboardAgent(QObject *parent = nullptr);

public slots:
    void processClipboardChange();
#if defined(QT_OS_LINUX)
    void processSelectionChange();
#endif
signals:
    void externalClipboardContentDetected();


};

#endif // CLIPBOARDAGENT_H
