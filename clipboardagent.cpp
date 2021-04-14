#include "clipboardagent.h"

#include <iostream>

#include <QClipboard>
#include <QApplication>

ClipboardAgent::ClipboardAgent(QObject *parent) : QObject(parent)
{

}


void ClipboardAgent::processClipboardChange()
{
    std::cout << "clipboard changed lul" << std::endl;
    QClipboard * qClipboard = QApplication::clipboard();
    if(qClipboard->ownsClipboard())
    {
        return;
    }
    else
    {
        if(!qClipboard->text().isNull() && !qClipboard->text().isEmpty())
        {
            qClipboard->clear();
        }
    }
}
