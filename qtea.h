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
    explicit QtEA(QWidget *parent = nullptr);
    void closeEvent(QCloseEvent *event);
    ~QtEA();

private:
    Ui::QtEA *ui;
    WebView *m_webview;
    bool _dbg_close;

public slots:
    void changeUrl();
    void onFocusInformationRequested();
signals:
    void sendFocusInformation(bool);

};
#endif // QTEA_H
