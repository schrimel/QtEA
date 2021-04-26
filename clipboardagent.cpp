#include "clipboardagent.h"

#include <QClipboard>
#include <QApplication>
#include <QDebug>

ClipboardAgent::ClipboardAgent(QObject *parent) : QObject(parent)
{

}


void ClipboardAgent::processClipboardChange()
{
#ifdef QT_DEBUG
    qDebug() << "clipboard changed";
#endif
    QClipboard * qClipboard = QApplication::clipboard();
    if(qClipboard->ownsClipboard())
    {
        if(!qClipboard->text().isNull() && !qClipboard->text().isEmpty())
            qDebug() << qClipboard->text();
        return;
    }
    else
    {
        if(!qClipboard->text().isNull() && !qClipboard->text().isEmpty())
        {
            emit externalClipboardContentDetected();
            qClipboard->clear();
        }
    }
}
