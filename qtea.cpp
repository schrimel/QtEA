//std-c++ includes
#include <iostream>

//qt includes
#include <QtWebEngineWidgets>
#include <QKeyEvent>
#include <QStackedWidget>

//user includes
#include "qtea.h"
#include "ui_qtea.h"
#include "webview.h"

#if defined(Q_OS_LINUX)
#include <QX11Info>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#endif

void QtEA::onNetworkInterfacesChanged()
{
#ifdef QT_DEBUG
   qDebug() << "network interface changed";
#endif
}

void QtEA::onLockReceived()
{
    //what to do?
}

QtEA::QtEA(QWidget *parent, const std::string &iBaseUrl)
    : QMainWindow(parent)
    , ui(new Ui::QtEA)
{
    m_webview = new WebView(this);
    m_webview->setBaseUrl(new QUrl(QString::fromStdString(iBaseUrl)));
    _dbg_close = false;
    ui->setupUi(this);
//    setAcceptDrops(false);
    centralWidget()->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
    setCentralWidget(m_webview);

#ifdef QT_DEBUG
    QObject::connect(qApp, &QGuiApplication::applicationStateChanged, this, [=](Qt::ApplicationState state){
        qDebug() << state;
    });
#endif


#if defined(Q_OS_LINUX)
    unsigned long data = 0xFFFFFFFF;
    XChangeProperty(QX11Info::display(),
                    winId(),
                    XInternAtom(QX11Info::display(), "_NET_WM_DESKTOP", 1),
                    XA_CARDINAL,
                    32,
                    PropModeReplace,
                    reinterpret_cast<unsigned char*>(&data),
                    1);
    XMapWindow(QX11Info::display(), winId());
#endif
}

QtEA::~QtEA()
{
    delete ui;
}

void QtEA::changeUrl()
{
    m_webview->setBaseUrl(new QUrl("https://google.com"));
}

void QtEA::onFocusInformationRequested()
{
    emit sendFocusInformation(this->isActiveWindow());
}

/**
 * @brief QtEA::closeEvent
 * @param event
 * override der closeEvent-Methode, für benutzerdefiniertes Schliessen der Anwendung.
 */
void QtEA::closeEvent(QCloseEvent *event)
{
    //erster Ansatz: 1st click: setze flag auf true, 2nd click (flag gesetzt): beende Anwendung
    if(!_dbg_close)
    {
        _dbg_close = true;
        event->ignore();
    }
    else
    {
        emit closeRequest();
        event->accept();
    }
}
