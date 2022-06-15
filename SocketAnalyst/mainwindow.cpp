#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDir>
#include <QFileDialog>
#include <QDebug>
#include <QJsonDocument>
#include <QComboBox>
#include <QJsonArray>
#include <QJsonObject>
#include <iostream>
#include <stdio.h>

MainWindow::MainWindow(QWidget *parent) :QMainWindow(parent),ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->sendMessageButton->setEnabled(false);
    _pSocket =new QWebSocket(QApplication::applicationName().toLower(),QWebSocketProtocol::Version::VersionLatest,this);
    connect(ui->selectProject,&QAction::triggered,this,&MainWindow::browse);
    connect(ui->connectButton,&QPushButton::clicked,this,&MainWindow::connectSocket);
    connect(ui->sendMessageButton,&QPushButton::clicked,this,&MainWindow::sendMessage);
    connect(ui->comboBox,&QComboBox::currentTextChanged,this,&MainWindow::messageSelected);
    connect(ui->pushButton,&QPushButton::clicked,this,&MainWindow::clearResponce);
    connect(_pSocket,&QWebSocket::connected,this,&MainWindow::onConnected);
    connect(_pSocket,&QWebSocket::disconnected,this,&MainWindow::onDisconnected);
    connect(_pSocket,&QWebSocket::textMessageReceived,this,&MainWindow::onTextMessageReceived);
    connect(_pSocket,&QWebSocket::sslErrors,this,&MainWindow::onSslErrors);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showMessage(QString message)
{
    ui->respBody->append("-------------------------------------------------------------------------------\n");
    ui->respBody->append(message+"\n");
}

void MainWindow::browse()
{
    QStringList list = QFileDialog::getOpenFileNames(this,"Select project file",QDir::homePath(),"File (*.json)");
    QFile file;
    file.setFileName(list[0]);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString val = file.readAll();
    QJsonDocument jsonData = QJsonDocument::fromJson(val.toUtf8());
    ui->urlTextEdit->setText(jsonData["url"].toString());
    QJsonArray requestArray = jsonData["request"].toArray();

    ui->comboBox->clear();
    foreach(const QJsonValue &request, requestArray)
    {
        QJsonObject value = request.toObject();
        ui->comboBox->addItem(value["name"].toString());
        _messageList[value["name"].toString()]= QString(QJsonDocument(value["message"].toObject()).toJson());
    }
    file.close();
}

void MainWindow::connectSocket()
{
    const QUrl url = ui->urlTextEdit->text().trimmed();
    if(!url.isEmpty() && !_pSocket->isValid()) // If the socket is not connect
    {
        std::cout << "Info  : Connecting to the URL " << url.toString().toStdString() << std::endl;
        _pSocket->open(url);
    }
    else                                       // If the socket is connect
    {
         _pSocket->close();
    }
}

void MainWindow::sendMessage()
{
    if(_pSocket->isValid())
    {
        _pSocket->sendTextMessage(ui->bodyTextEdit->toPlainText());
    }
}

void MainWindow::messageSelected(const QString &title)
{
    ui->bodyTextEdit->setText(_messageList[title]);
}

void MainWindow::clearResponce()
{
    ui->respBody->clear();
}

void MainWindow::onConnected()
{
    ui->sendMessageButton->setEnabled(true);
    ui->connectButton->setText("Disconnect");
    ui->urlTextEdit->setEnabled(false);
    ui->connectStateLable->setText("Connected");
}

void MainWindow::onDisconnected()
{
    ui->sendMessageButton->setEnabled(false);
    ui->connectButton->setText("Connect");
    ui->urlTextEdit->setEnabled(true);
    ui->connectStateLable->setText("Disconnected");
    std::cout << "Error : " << _pSocket->errorString().toStdString() << " Error Code : " << _pSocket->error() << std::endl;
}

void MainWindow::onTextMessageReceived(const QString &message)
{
    showMessage(message);
    //showMessage(QJsonDocument::fromJson(message.toUtf8()).toJson(QJsonDocument::JsonFormat::Indented));
}

void MainWindow::onSslErrors(const QList<QSslError> &errors)
{
    foreach (QSslError err, errors)
    {
        std::cout << "SSL Error : " << err.errorString().toStdString() << std::endl;
    }
    _pSocket->ignoreSslErrors();
}
