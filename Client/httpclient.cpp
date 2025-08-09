//HttpClient.cpp
#include "httpclient.h"
#include "ui_httpclient.h"

#include <QFileInfo>
#include <QHttpMultiPart>
#include <QNetworkReply>
#include <QFileDialog>


HttpClient::HttpClient(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::HttpClient)
{
    ui->setupUi(this);
    manager=new QNetworkAccessManager;
     connect(ui->pushButton_login,&QPushButton::clicked,this,&HttpClient::login);
//     connect(ui->pushButton_send,&QPushButton::clicked,this,&HttpClient::downloadFile);
//     connect(ui->pushButton_send,&QPushButton::clicked,this,&HttpClient::uploadScore);
     connect(ui->pushButton_send,&QPushButton::clicked,this,[=](){
         HttpClient::uploadScore(filePath);
     });

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

void HttpClient::uploadScore(QString filePath)
{
    QUrl url("http://127.0.0.1:8080/upload");
    QNetworkRequest request(url);

    // 2. 创建multipart请求体
    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    // 3. 添加文件部分
    QHttpPart filePart;
    filePart.setHeader(QNetworkRequest::ContentDispositionHeader,
                       QVariant("form-data; name=\"file\"; filename=\"" + QFileInfo(filePath).fileName() + "\""));

    QFile *file = new QFile(filePath);
        if (!file->open(QIODevice::ReadOnly)) {
            qDebug() << "文件打开失败：" << filePath;
            delete file;
            delete multiPart;
            return;
        }
        filePart.setBodyDevice(file);
        file->setParent(multiPart); // 确保随multiPart自动释放
        multiPart->append(filePart);

    // 5. 发送请求
    QNetworkReply *reply = manager->post(request, multiPart);
    multiPart->setParent(reply); // 确保随reply自动释放

    // 6. 处理响应
    QObject::connect(reply, &QNetworkReply::finished, [reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            qDebug() << "上传成功！响应：" << reply->readAll();
        } else {
            qDebug() << "上传失败：" << reply->errorString();
        }
        reply->deleteLater(); // 释放资源
    });
}
HttpClient::~HttpClient()
{
    delete ui;
}


void HttpClient::on_pushButton_address_clicked()
{
    // 1. 弹出文件选择对话框
    filePath = QFileDialog::getOpenFileName(
        this,                       // 父窗口（当前类实例）
        tr("选择文件"),               // 对话框标题
        "",                         // 默认目录（空表示系统默认）
        tr("所有文件 (*);;文本文件 (*.txt);;图像文件 (*.png *.jpg)")  // 文件过滤器
    );

    // 2. 检查是否选择了有效文件路径
    if (!filePath.isEmpty()) {
        // 3. 将文件路径显示在 lineEdit_address 控件上
        ui->lineEdit_address->setText(filePath);
    } else {
        // 可选：用户取消选择时的提示
        qDebug() << "未选择文件";
    }
}

