#ifndef WEBVIEW_H
#define WEBVIEW_H

#include <QWidget>
#include <QWebEngineClientCertificateSelection>
#include "webpage.h"

class QWebEngineView;
class QWebEngineProfile;

class WebView : public QWidget
{
    Q_OBJECT
public:
    explicit WebView(QWidget *parent = nullptr);
    void setBaseUrl(QUrl *i_base_url);
    void setPage(WebPage *page);
public slots:
    //void setRandomCert(QWebEngineClientCertificateSelection selection);
private:
    QWebEngineView *m_view;
    QUrl *m_base_url;

signals:

};

#endif // WEBVIEW_H
