#include "webview.h"
#include <QtWebEngineWidgets>
#include <QSslCertificate>
#include <QList>
#include <QSslCertificate>
#include <QSslConfiguration>
#include <QSslKey>
#include <QFile>
#include <QWebEngineClientCertificateStore>
#include <QWebEngineClientCertificateSelection>
#include <QWebEngineProfile>
#include <QSignalSpy>

#include <iostream>
#include "webpage.h"

WebView::WebView(QWidget *parent) : QWidget(parent)
{
    //resize(1000,1000);
    WebView::m_view = new QWebEngineView(this);
    WebPage *page = new WebPage(QWebEngineProfile::defaultProfile(), this);
    setPage(page);

#if !defined(QT_NO_SSL) || QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
    //QFile pKeyFile("C:\\Users\\CSchi\\client.key");
    QFile pKeyFile("C:\\Users\\CSchi\\client-cert1.pfx");

    pKeyFile.open(QIODevice::ReadOnly);

    //QFile certFile("C:\\Users\\CSchi\\client.crt");
    QFile certFile("C:\\Users\\CSchi\\client-key1.pem");
    certFile.open(QIODevice::ReadOnly);

    QSslKey pKey(&pKeyFile, QSsl::Rsa);
    QSslCertificate cert(&certFile);

    QWebEngineClientCertificateStore * store = m_view->page()->profile()->clientCertificateStore();
    //store->add(cert, pKey);
    //std:: cout << store->certificates().at(0).toText().toStdString() << std::endl;
#endif
    //bool rr = connect(m_view->page(), SIGNAL(selectClientCertificate(QWebEngineClientCertificateSelection)), this, SLOT(setRandomCert(QWebEngineClientCertificateSelection)));

    WebView::m_base_url = new QUrl("https://server.cryptomix.com/secure/");
    WebView::m_view->load(*WebView::m_base_url);
    WebView::m_view->setFocus();
    WebView::m_view->resize(1920,1080);
    WebView::m_view->show();
    //    std::cout << sslConfig.localCertificateChain().at(0).toText().toStdString() << std::endl;

}

void WebView::setBaseUrl(QUrl *i_base_url)
{
    if(i_base_url == nullptr)
    {
        //This is not allowed!
        throw 1;
    }
    if(!i_base_url->isEmpty())
    {
        WebView::m_base_url = i_base_url;
    }
    WebView::m_view->load(*WebView::m_base_url);
}

void WebView::setPage(WebPage *page)
{
    m_view->setPage(page);
}
