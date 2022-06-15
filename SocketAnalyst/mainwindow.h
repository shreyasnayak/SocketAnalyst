#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include <QWebSocket>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void showMessage(QString message);

private:
    Ui::MainWindow *ui;
    QMap<QString,QString> _messageList;
    QWebSocket *_pSocket;

private slots:
    void browse();
    void connectSocket();
    void sendMessage();
    void messageSelected(const QString &title);
    void clearResponce();

    void onConnected();
    void onDisconnected();
    void onTextMessageReceived(const QString &message);
    void onSslErrors(const QList<QSslError> &errors);
};

#endif // MAINWINDOW_H
