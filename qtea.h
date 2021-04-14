#ifndef QTEA_H
#define QTEA_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class QtEA; }
QT_END_NAMESPACE

class WebView;

class QtEA : public QMainWindow
{
    Q_OBJECT

public:
    explicit QtEA(QWidget *parent = nullptr, const std::string & iBaseUrl="https://qt.io");
    void closeEvent(QCloseEvent *event);
    ~QtEA();

private:
    Ui::QtEA *ui;
    WebView *m_webview;
    bool _dbg_close;

protected:
    bool eventFilter(QObject* obj, QEvent* event);

public slots:
    void changeUrl();
    void onFocusInformationRequested();
    void onNetworkInterfacesChanged();
signals:
    void sendFocusInformation(bool);
    void closeRequest();

};
#endif // QTEA_H
