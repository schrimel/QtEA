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
    WebView(QWidget *parent, const QUrl &url);
    void setBaseUrl(QUrl *i_base_url);
    void setPage(WebPage *page);
    const WebPage* currentPage();
    void openView();
public slots:
    //void setRandomCert(QWebEngineClientCertificateSelection selection);
private:
    QWebEngineView *m_view;
    QUrl *m_base_url;
    WebPage *m_page;

signals:

};

#endif // WEBVIEW_H
