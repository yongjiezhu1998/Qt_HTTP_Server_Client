//HttpClient.cpp
#include "httpclient.h"
#include "ui_httpclient.h"

#include <QNetworkReply>


HttpClient::HttpClient(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::HttpClient)
{
    ui->setupUi(this);
    manager=new QNetworkAccessManager;
     connect(ui->pushButton_login,&QPushButton::clicked,this,&HttpClient::login);
//     connect(ui->pu_down,&QPushButton::clicked,this,&HttpClient::downloadFile);
//     connect(ui->pb_upload,&QPushButton::clicked,this,&HttpClient::uploadScore);
}

/*
 * Qt的QNetworkAccessManager向本地服务器（http://127.0.0.1:8080/login）
 * 发送一个JSON格式的登录请求，并异步处理相应结果。
*/
void HttpClient::login()
{
    // URL与端点
    QUrl url("http://127.0.0.1:8080/login");
    QNetworkRequest request(url);

    // 请求头设置
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    // 请求体构造
    QJsonObject json;
    json["username"] = ui->lineEdit_name->text();//admin
    json["password"] = ui->lineEdit_password->text();//123456
    QByteArray jsonData = QJsonDocument(json).toJson();
    qDebug() << jsonData;
    // POST请求，提交敏感数据，避免URL暴露参数
    request.setHeader(QNetworkRequest::ContentLengthHeader, jsonData.size());
    QNetworkReply *reply = manager->post(request, jsonData);

    // 异步处理，通过QNetworkReply::finished信号实现非阻塞操作，避免主线程卡顿
    QObject::connect(reply, &QNetworkReply::finished, [reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            qDebug() << "Login Response:" << reply->readAll();
        } else {
            qDebug() << "Login Error:" << reply->errorString();
        }
        reply->deleteLater();
    });
}

void HttpClient::downloadFile()
{
    QUrl url("http://127.0.0.1:8080/download");
    QNetworkRequest request(url);

    QNetworkReply *reply = manager->get(request);
    QObject::connect(reply, &QNetworkReply::finished, [reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray data = reply->readAll();
            QFile file("D:\\Project\\qtcreator\\HttpClient\\downloaded.txt");
            if (file.open(QIODevice::WriteOnly)) {
                file.write(data);
                file.close();
                qDebug() << "File downloaded successfully!";
            }
        } else {
            qDebug() << "Download Error:" << reply->errorString();
        }
        reply->deleteLater();
    });
}
void HttpClient::uploadScore()
{
    QUrl url("http://127.0.0.1:8080/upload");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QJsonObject json;
//    json["chinese"] = ui->li_chinese->text();
//    json["math"] =ui->li_math->text();
//    json["english"] = ui->li_english->text();

    QNetworkReply *reply = manager->post(request, QJsonDocument(json).toJson());
    QObject::connect(reply, &QNetworkReply::finished, [reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            qDebug() << "Upload Response:" << reply->readAll();
        } else {
            qDebug() << "Upload Error:" << reply->errorString();
        }
        reply->deleteLater();
    });
}
HttpClient::~HttpClient()
{
    delete ui;
}

