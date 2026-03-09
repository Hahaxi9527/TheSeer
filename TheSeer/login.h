#ifndef LOGIN_H
#define LOGIN_H

#include "config.h"
#include "lobby.h"
#include "manager.h"
#include <QByteArray>
#include <QDebug>
#include <QIcon>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLineEdit>
#include <QMessageBox>
#include <QPainter>
#include <QPalette>
#include <QTcpSocket> //通信套接字
#include <QTimer>

#include <QWidget>
#pragma execution_character_set("utf-8")
extern manager mo;
namespace Ui {
class login;
}

class login : public QWidget
{
    Q_OBJECT

public:
    explicit login(QTcpSocket *_tcp_socket, QWidget *parent = nullptr);

    ~login();

protected:
    void paintEvent(QPaintEvent *event) override
    {
        QPainter painter(this);
        QPixmap pix(":/resource/background/sign_in.png");
        painter.drawPixmap(0, 0, this->width(), this->height(), pix);
    }

public slots:
    //连接服务器，发送登录信息
    //void Signin();
    //处理服务器发来的消息
    void receive_from_server();
    //可以接受服务器来的消息
    void reconnect();

private slots:
    void on_sign_in_clicked();

    void on_login_2_clicked();

private:
    Ui::login *ui;
    QTcpSocket *tcp_socket;
    //游戏大厅指针
    lobby *lobby_1 = nullptr;
};

#endif // LOGIN_H
