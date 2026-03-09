#ifndef START_GAME_H
#define START_GAME_H

#pragma execution_character_set("utf-8")
#include "login.h"
#include "manager.h"
#include <QHostAddress>
#include <QIcon>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTcpSocket> //通信套接字
#include <QTimer>
#include <QWidget>

extern manager mo;

namespace Ui {
class start_game;
}

//开始界面类
class start_game : public QWidget
{
    Q_OBJECT

public:
    explicit start_game(QWidget *parent = nullptr);
    ~start_game();

protected:
    void paintEvent(QPaintEvent *event) override
    {
        QPainter painter(this);
        QPixmap pix(":/resource/background/start_game_background.jpg");
        painter.drawPixmap(0, 0, this->width(), this->height(), pix);
    }

private:
    Ui::start_game *ui;
    QTcpSocket *tcp_socket;
    login *login_1 = NULL; //登录窗口指针

private slots:
    //切换到登录界面
    //void switch_to_login();
    //接收到服务器回复
    //    void receive_from_server();
    void on_pushButton_clicked();
};

#endif // START_GAME_H
