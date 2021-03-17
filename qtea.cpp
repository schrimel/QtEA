//qt includes
#include <QtWebEngineWidgets>

//user includes
#include "qtea.h"
#include "ui_qtea.h"
#include "webview.h"

QtEA::QtEA(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::QtEA)
{
    m_webview = new WebView(this);
    _dbg_close = false;
    ui->setupUi(this);
    setCentralWidget(m_webview);
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
        event->accept();
    }
}
