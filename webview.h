#ifndef WEBVIEW_H
#define WEBVIEW_H

#include <QWidget>

class QWebEngineView;
class QWebEngineProfile;

class WebView : public QWidget
{
    Q_OBJECT
public:
    explicit WebView(QWidget *parent = nullptr);
    void setBaseUrl(QUrl *i_base_url);

private:
    QWebEngineView *m_view;
    QUrl *m_base_url;

signals:

};

#endif // WEBVIEW_H
