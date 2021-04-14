#include "keyboardfilter.h"
#include <QKeyEvent>
#include <iostream>

KeyboardFilter::KeyboardFilter(QObject *parent) : QObject(parent)
{

}

bool KeyboardFilter::eventFilter(QObject* obj, QEvent* event)
{
    if(event->type() == QEvent::KeyPress)
    {
        QKeyEvent* key = static_cast<QKeyEvent*>(event);
        if(key->modifiers() & Qt::AltModifier)
        {
            if(key->key() == Qt::Key_Tab || key->key() == Qt::Key_Backtab)
            {
                std::cout << "nice" << std::endl;
                return true;
            }
        }
    }
    return false;
}
