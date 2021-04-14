#ifndef KEYBOARDFILTER_H
#define KEYBOARDFILTER_H

#include <QObject>

class KeyboardFilter : public QObject
{
    Q_OBJECT
public:
    explicit KeyboardFilter(QObject *parent = nullptr);
protected:
    bool eventFilter(QObject*, QEvent* event);
signals:

};

#endif // KEYBOARDFILTER_H
