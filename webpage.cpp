#include "webpage.h"
#include "ui_certificateerrordialog.h"

#include <QWebEngineCertificateError>
#include <QTimer>
#include <QMessageBox>
#include <QDialog>
#include <QDebug>
#include <QIcon>
#include <QStyle>
#include <QWebEngineScript>
#include <QString>
#include <QWebEngineScriptCollection>

WebPage::WebPage(QWebEngineProfile *profile, QObject *parent) : QWebEnginePage(profile, parent)
{
    connect(this, &QWebEnginePage::selectClientCertificate, this, &WebPage::setRandomCert);
    qDebug()<<"connected";
    QWebEngineScript script;
    QString code = QString::fromStdString("console.log('hello, world')");
    script.setSourceCode(code);
    script.setWorldId(QWebEngineScript::MainWorld);
    script.setInjectionPoint(QWebEngineScript::Deferred);
    this->scripts().insert(script);
    runJavaScript(code, QWebEngineScript::ApplicationWorld);
    connect(this, &WebPage::closeOnConsoleMessage, this, &WebPage::onCloseRequestedDummy);
}

void WebPage::javaScriptConsoleMessage(JavaScriptConsoleMessageLevel level, const QString &message, int i, const QString &sourceId)
{
    qDebug() << message;
    qDebug() << message.compare("close");
    if(message == "close")
    {
        qDebug() << "close requested";
        emit closeOnConsoleMessage();
    }
}

void WebPage::onCloseRequestedDummy()
{
    qDebug() << "app closed";
}

bool WebPage::certificateError(const QWebEngineCertificateError &error)
{
    QWidget *mainWindow = view()->window();

    #if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
        QWebEngineCertificateError deferredError = error;
        deferredError.defer();

        QTimer::singleShot(0, mainWindow, [mainWindow, error = std::move(deferredError)] () mutable {
            if (!error.deferred()) {
                QMessageBox::critical(mainWindow, tr("Certificate Error"), error.errorDescription());
            } else {
    #else
        if (error.isOverridable()) {
    #endif
                QDialog dialog(mainWindow);
                dialog.setModal(true);
                dialog.setWindowFlags(dialog.windowFlags() & ~Qt::WindowContextHelpButtonHint);

                Ui::CertificateErrorDialog certificateDialog;
                certificateDialog.setupUi(&dialog);
                certificateDialog.m_iconLabel->setText(QString());
                QIcon icon(mainWindow->style()->standardIcon(QStyle::SP_MessageBoxWarning, 0, mainWindow));
                certificateDialog.m_iconLabel->setPixmap(icon.pixmap(32, 32));
                certificateDialog.m_errorLabel->setText(error.errorDescription());
                dialog.setWindowTitle(tr("Certificate Error"));

    #if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
                if (dialog.exec() == QDialog::Accepted)
                    error.ignoreCertificateError();
                else
                    error.rejectCertificate();
            }
        });
        return true;
    #else
            return dialog.exec() == QDialog::Accepted;
        }

        QMessageBox::critical(mainWindow, tr("Certificate Error"), error.errorDescription());
        return false;
    #endif
    return true;
}


void WebPage::setRandomCert(QWebEngineClientCertificateSelection selection)
{
    qDebug() << "called";
    selection.select(selection.certificates().at(0));
}
