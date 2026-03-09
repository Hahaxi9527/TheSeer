#ifndef MANAGER_H
#define MANAGER_H

#include "config.h"
#include <QByteArray>
#include <QDebug>
#include <QHostAddress>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QString>
#include <QTcpSocket> //通信套接字
#include <QTextCodec>
#pragma execution_character_set("utf-8")

#include <QObject>
#include <QTimer>

class manager
{
public:
public:
    explicit manager();
    bool is_open;                   //是否打开
    QString username;               //玩家姓名
    int user_id;                    //玩家ID
    int money;
    int client_pet; //玩家方宠物
    int server_pet; //服务器方宠物

    int win = 0;     //是否胜利(0:平局 1：赢 -1：输)
    int socket_id = -1;
};

#endif // MANAGER_H
