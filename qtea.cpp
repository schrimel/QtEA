//std-c++ includes
#include <iostream>
#include <chrono>

//qt includes
#include <QtWebEngineWidgets>
#include <QKeyEvent>
#include <QStackedWidget>
#include <QInputDialog>
#include <QRegExp>

//user includes
#include "qtea.h"
#include "ui_qtea.h"
#include "webview.h"
#include "lockoverlay.h"

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
    m_stacked->setCurrentIndex(1);
    m_overlay->showFullScreen();
}

void QtEA::onUnlockReceived()
{
    m_stacked->setCurrentIndex(0);
    m_overlay->hide();
}


QtEA::QtEA(QWidget *parent, const QString &iBaseUrl)
    : QMainWindow(parent)
    , ui(new Ui::QtEA)
{
    QUrl w_baseUrl(iBaseUrl);
    QRegExp urlPattern("^https?:\/\/([^\s\.?]+([^\s]*)?$)");
    QRegExp urlPattern2("^https?:\/\/([0-9a-zA-Z\s\-\.?]+):([0-9]+)");
    QRegExpValidator val(urlPattern);
    QRegExpValidator val2(urlPattern2);
    QString temp(iBaseUrl);
    int i = 0;
    qDebug() << val2.validate(temp,i);
    qDebug() << temp;
    while(!val.validate(temp,i) && !val2.validate(temp,i) && !temp.isEmpty())
    {
        bool ok;
        QString title("Incorrect URL!");
        QString message("The url from the config file is not correct. Please enter the URL from the exam server. You might ask your examinator for this.");
        QString url = QInputDialog::getText(this, title, message, QLineEdit::Normal,"",&ok);//,(Qt::WindowFlags)Qt::WA_AlwaysStackOnTop);
        temp = url;
        w_baseUrl = QUrl(temp);
    }
    m_stacked = new QStackedWidget();
    m_webview = new WebView(this, w_baseUrl);
    m_overlay = new LockOverlay();
    m_stacked->addWidget(m_webview);
    m_stacked->addWidget(m_overlay);

#if defined(QT_DEBUG)
    //debug page
    QWidget * backgroundwidget = new QWidget(this);
    QPalette pal;
    pal.setColor(QPalette::Background, Qt::white);
    setPalette(pal);
    QLabel *label = new QLabel(backgroundwidget);
    label->setPixmap(QString("res/background_img.png"));
    label->setGeometry(500,200,320,300);
    label->show();
    QLabel *label2 = new QLabel(backgroundwidget);
    label2->setText("Qt-EApp");
    label2->setGeometry(625,350,400,200);
    label2->show();
    QFont font = label2->font();
    font.setPointSize(20);
    label2->setFont(font);
    m_stacked->addWidget(backgroundwidget);
    backgroundwidget->show();
#endif


//    m_webview->setBaseUrl(new QUrl(iBaseUrl));
    _dbg_close = false;
    ui->setupUi(this);
//    setAcceptDrops(false);
    centralWidget()->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
    //setCentralWidget(m_webview);
    setCentralWidget(m_stacked);
    //m_overlay->show();
    m_stacked->setCurrentIndex(0);
    connect(m_webview->currentPage(), &WebPage::receivedLockFromServer, this, &QtEA::onLockReceived);
    connect(m_webview->currentPage(), &WebPage::receivedUnlockFromServer, this, &QtEA::onUnlockReceived);


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

void QtEA::startBrowser()
{
    m_webview->openView();
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
