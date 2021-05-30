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
#ifdef QT_DEBUG
        if(!qClipboard->text().isNull() && !qClipboard->text().isEmpty())
            qDebug() << qClipboard->text();
#endif
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

#if defined(QT_OS_LINUX)

void ClipboardAgent::processSelectionChange()
{
    QClipboard * qClipboard = QApplication::clipboard();
    if(qClipboard->ownsSelection())
    {
        return;
    }
    else
    {
        if(!qClipboard->text(QClipboard::Mode::Selection).isNull() && !qClipboard->text(QClipboard::Mode::Selection).isEmpty())
        {
            emit externalClipboardContentDetected();
            qClipboard->clear(QClipboard::Mode::Selection);
        }
    }
}
#endif
