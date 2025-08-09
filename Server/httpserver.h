//HttpServer.h
#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QTcpServer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QString>
#include <QCoreApplication>
#include <QDir>
#include <qDebug>


class Server : public QTcpServer
{
//http服务器类
    Q_OBJECT
public:
    Server(QObject *parent = nullptr) : QTcpServer(parent) {}
signals:
//信号函数
//自定义信号函数，发送信号，将信息显示在界面类上的textedit中
    void login_success(QString str);
    void login_faild(QString str);
    void download_success(QString str);
    void download_faild(QString str);
    void upload_success(QString str);
protected:
    void incomingConnection(qintptr socketDescriptor) override {
        QTcpSocket *socket = new QTcpSocket(this);
        socket->setSocketDescriptor(socketDescriptor);
        connect(socket, &QTcpSocket::readyRead, this, [this, socket]() {
            handleRequest(socket);
        });
    }
private:
     void handleRequest(QTcpSocket *socket)
    {
         // 1. 读取请求头（直到 \r\n\r\n）
         QByteArray headerData;
         while (socket->canReadLine()) {
             QByteArray line = socket->readLine();
             headerData += line;
             if (line == "\r\n") break; // 空行标志头结束
         }

         // 2. 解析Content-Length
         int contentLength = -1;
         foreach (const QByteArray &line, headerData.split('\n')) {
             if (line.startsWith("Content-Length:")) {
                 contentLength = line.split(':')[1].trimmed().toInt();
                 break;
             }
         }

         // 3. 按长度读取请求体（解决拆包问题）
         QByteArray body;
         if (contentLength > 0) {
             while (socket->bytesAvailable() < contentLength) {
                 socket->waitForReadyRead(500); // 阻塞等待剩余数据
             }
             body = socket->read(contentLength);
         }

         // 4. 组合完整请求
         QByteArray fullRequest = headerData + body;
         qDebug() << "Full request:" << fullRequest;


        QString request = QString::fromUtf8(fullRequest);
        QStringList lines = request.split("\r\n");
        QString requestLine = lines[0];
        QStringList requestParts = requestLine.split(" ");
        QString method = requestParts[0];
        QString path = requestParts[1];

        if (method == "POST" && path == "/login") {
            handleLogin(socket, request);
        } else if (method == "GET" && path == "/download") {
            handleDownload(socket);
        } else if (method == "POST" && path == "/upload") {
            handleUpload(socket, request);
        } else {
            sendResponse(socket, 404, "Not Found");
        }
    }
     void handleLogin(QTcpSocket *socket, const QString &request)
     {
         // 解析请求体
         QString body = request.split("\r\n\r\n").last();
         QJsonDocument doc = QJsonDocument::fromJson(body.toUtf8());
         QJsonObject json = doc.object();
         if(json.isEmpty())
         {
             return;
         }
         QString username = json["username"].toString();
         QString password = json["password"].toString();

         // 简单验证
         if (username == "admin" && password == "123456") {
             sendResponse(socket, 200, "Login Success");
             emit login_success(username+":"+"Login Success!");
         } else {
             sendResponse(socket, 401, "Login Failed");
              emit login_faild(username+":"+"Login Failed!");
         }
     }
     void handleDownload(QTcpSocket *socket)
     {
         // 获取应用程序的当前目录
         QString appDir = QCoreApplication::applicationDirPath();

         // 构建文件的完整路径
         QString filePath = QDir(appDir).filePath("test.txt");
         QFile file(filePath);
         if (file.open(QIODevice::ReadOnly)) {
             QByteArray data = file.readAll();
             sendResponse(socket, 200, data);
             file.close();
             emit upload_success("download Success!");
         } else {
              emit upload_success("download Failed!");
             sendResponse(socket, 404, "File Not Found");
         }
     }
     void handleUpload(QTcpSocket *socket, const QString &request)
     {
         QString body = request.split("\r\n\r\n").last();
         QJsonDocument doc = QJsonDocument::fromJson(body.toUtf8());
         QJsonObject json = doc.object();

         QString chinese = json["chinese"].toString();
         QString math = json["math"].toString();
         QString english = json["english"].toString();

         // 简单处理，打印上传的数据
         emit upload_success("语文:"+chinese+";"+"数学:"+math+";"+"英语:"+english);
         qDebug() << "语文:" << chinese << "数学:" << math << "英语:" << english;
         sendResponse(socket, 200, "Upload Success");
     }
     void sendResponse(QTcpSocket *socket, int statusCode, const QByteArray &body)
     {
         QString response = QString("HTTP/1.1 %1 OK\r\n"
                                    "Content-Type: text/plain\r\n"
                                    "Content-Length: %2\r\n"
                                    "\r\n"
                                    "%3")
                                .arg(statusCode)
                                .arg(body.size())
                                .arg(QString(body));
         socket->write(response.toUtf8());
         socket->disconnectFromHost();
     }

};


QT_BEGIN_NAMESPACE
namespace Ui {
class HttpServer;
}
QT_END_NAMESPACE

class HttpServer : public QMainWindow
{
//http服务器界面类
    Q_OBJECT

public:
    HttpServer(QWidget *parent = nullptr);
    ~HttpServer();
 private:
    Ui::HttpServer *ui;
    Server* m_server;
};
#endif // HTTPSERVER_H
