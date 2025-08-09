//HttpClient.h
#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H

#include <QMainWindow>
#include<QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QDebug>
#include <QPushButton>

QT_BEGIN_NAMESPACE
namespace Ui {
class HttpClient;
}
QT_END_NAMESPACE

class HttpClient : public QMainWindow
{
    Q_OBJECT

public:
    HttpClient(QWidget *parent = nullptr);
    ~HttpClient();
private:
     Ui::HttpClient *ui;
    QNetworkAccessManager *manager;
    QString filePath;
private slots:
    void login( );
    void downloadFile( );
    void uploadScore(QString filePath);
    void on_pushButton_address_clicked();
};
#endif // HTTPCLIENT_H
