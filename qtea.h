#ifndef QTEA_H
#define QTEA_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class QtEA; }
QT_END_NAMESPACE

class WebView;
class LockOverlay;
class QStackedWidget;

class QtEA : public QMainWindow
{
    Q_OBJECT

public:
    explicit QtEA(QWidget *parent = nullptr, const QString & iBaseUrl="https://qt.io");
    void closeEvent(QCloseEvent *event);
    ~QtEA();
    void startBrowser();

private:
    Ui::QtEA *ui;
    WebView *m_webview;
    bool _dbg_close;
    LockOverlay* m_overlay;
    QStackedWidget *m_stacked;

public slots:
    void changeUrl();
    void onFocusInformationRequested();
    void onNetworkInterfacesChanged();
    void onLockReceived();
    void onUnlockReceived();
signals:
    void sendFocusInformation(bool);
    void closeRequest();

};
#endif // QTEA_H
