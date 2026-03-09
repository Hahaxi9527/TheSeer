#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#pragma execution_character_set("utf-8")
#include "config.h"
#include "database.h"
#include "pets.h"
#include "time.h"
#include <QByteArray>
#include <QException>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QList>
#include <QMainWindow>
#include <QRandomGenerator>
#include <QSqlRecord>
#include <QTcpServer> //监听套接字
#include <QTcpSocket> //通信套接字
#include <QTextCodec>
#include <QVector>

extern database mo;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void Sign_in(QJsonObject);  //登陆
    void Register(QJsonObject); //注册
    void Sign_out(QJsonObject); //登出
    void Lobby(QJsonObject);    //大厅查询
    void Recharge(QJsonObject); //充值
    void User_list(QJsonObject); //用户列表信息查询
    void Pet_list(QJsonObject);  //宠物列表查询
    void Battle_choose(QJsonObject);
    void Flush_server_pets(QJsonObject);
    void Add_server_pets(QJsonObject);
    void Start_upgrade_battle(QJsonObject);
    void Start_vs_battle(QJsonObject);
    void Upgrade_win(QJsonObject);
    void Upgrade_lose(QJsonObject);
    void Vs_win(QJsonObject);
    void Vs_lose(QJsonObject);
    void Abandon_pet(QJsonObject);
    void Upgrade(QJsonObject);

    void Addpets(QJsonObject); //作弊添加宠物
    void add_pet(); //添加一个宠物到数据库
    void assign_pets_to_user(int userId);
    void transfer_pet_to_user(int server_pet_id, int user_id);
    int generateUniquePetId();
    QTcpSocket *getSocketById(int socket_id);
    void send_socket_id();
    void send_message(QJsonObject); //群发信息
    void onNewConnection();
    void onReadyRead();
    void onClientDisconnected();

private:
    Ui::MainWindow *ui;

    //监听套接字
    QTcpServer *tcp_server;

    QTcpSocket *tcp_socket;

    //存放所有客户端socket
    QList<QTcpSocket *> socket_list;
    int socket_id = 0;
};
#endif // MAINWINDOW_H
