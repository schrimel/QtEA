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
signals:

};

#endif // CLIPBOARDAGENT_H
