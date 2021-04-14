#ifndef WEBPAGE_H
#define WEBPAGE_H

#include <QWebEnginePage>

class WebPage : public QWebEnginePage
{
    Q_OBJECT
public:
    WebPage(QWebEngineProfile *profile, QObject *parent = nullptr);
protected:
    virtual bool certificateError(const QWebEngineCertificateError &error);
    virtual void javaScriptConsoleMessage(JavaScriptConsoleMessageLevel level, const QString &message, int i, const QString &sourceId);

private slots:
    void setRandomCert(QWebEngineClientCertificateSelection selection);
    void onCloseRequestedDummy();
signals:
    void closeOnConsoleMessage();

};

#endif // WEBPAGE_H
