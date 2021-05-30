#ifndef LOCKOVERLAY_H
#define LOCKOVERLAY_H

#include <QWidget>

class QLabel;

class LockOverlay : public QWidget
{
    Q_OBJECT
public:
    explicit LockOverlay(QWidget *parent = nullptr);
private:
    QLabel *label;
    QLabel *label_2;
signals:

};

#endif // LOCKOVERLAY_H
