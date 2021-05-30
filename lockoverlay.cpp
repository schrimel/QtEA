#include "lockoverlay.h"

#include <QLabel>

LockOverlay::LockOverlay(QWidget *parent) : QWidget(parent)
{
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_PaintOnScreen);


    setAttribute(Qt::WA_TransparentForMouseEvents);

    label = new QLabel(this);
    label->setObjectName(QString::fromUtf8("label"));
    label->setGeometry(QRect(170, 450, 771, 121));


    QFont font;
    font.setPointSize(14);
    label->setFont(font);
    label->setAlignment(Qt::AlignCenter);
    label->setWordWrap(true);
    label->setText("Qt-EApp is locked due to a suspicious user event.                              Please contact your examinator immediately!");
    label_2 = new QLabel(this);
    label_2->setObjectName(QString::fromUtf8("label_2"));
    label_2->setGeometry(350,150,320,300);
    label_2->setPixmap(QString("./res/lock.png"));
}
