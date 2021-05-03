#ifndef WEBPAGE_H
#define WEBPAGE_H

#include <QWebEnginePage>
#include <QSslCertificate>

class WebPage : public QWebEnginePage
{
    Q_OBJECT
public:
    WebPage(QWebEngineProfile *profile, QObject *parent = nullptr);
    void executeJavaScript(const QString & iScript);
    void executeJavaScriptFromFile(const QString & iFilename);
protected:
    virtual bool certificateError(const QWebEngineCertificateError &iError);
    virtual void javaScriptConsoleMessage(JavaScriptConsoleMessageLevel iLevel, const QString &iMessage, int iLineNumber, const QString &iSourceId);
private:
    QSslCertificate * mSslCert;

private slots:
    void setRandomCert(QWebEngineClientCertificateSelection selection);
    void onCloseRequestedDummy();
signals:
    void closeOnConsoleMessage();
    void receivedLockFromServer();

};

#endif // WEBPAGE_H
