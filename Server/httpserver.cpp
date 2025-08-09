//HttpServer.cpp
#include "httpserver.h"
#include "ui_httpserver.h"


HttpServer::HttpServer(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::HttpServer)
{
    ui->setupUi(this);
    m_server= new Server();
    m_server->listen(QHostAddress::Any,8080);
    connect(m_server,&Server::login_success,this,[=](QString str)
    {
        ui->textEdit->append(str);
    });
    connect(m_server,&Server::login_faild,this,[=](QString str)
    {
        ui->textEdit->append(str);
    });
    connect(m_server,&Server::download_success,this,[=](QString str)
    {
        ui->textEdit->append(str);
    });
    connect(m_server,&Server::download_faild,this,[=](QString str)
    {
        ui->textEdit->append(str);
    });
    connect(m_server,&Server::upload_success,this,[=](QString str)
    {
        ui->textEdit->append(str);
    });
}
HttpServer::~HttpServer()
{
    delete ui;
}
