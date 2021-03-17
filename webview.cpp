#include "webview.h"
#include <QtWebEngineWidgets>

WebView::WebView(QWidget *parent) : QWidget(parent)
{
    //resize(1000,1000);
    WebView::m_view = new QWebEngineView(this);
    WebView::m_base_url = new QUrl("http://qt.io");
    WebView::m_view->load(*WebView::m_base_url);
    WebView::m_view->setFocus();
    WebView::m_view->resize(1920,1080);
    WebView::m_view->show();
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
