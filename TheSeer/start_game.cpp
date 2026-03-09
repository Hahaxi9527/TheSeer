#include "start_game.h"
#include "config.h"
#include "ui_start_game.h"

start_game::start_game(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::start_game)
{
    ui->setupUi(this);

    tcp_socket = NULL;
    //    tcp_socket = new QTcpSocket(this);
    //    //获取服务器ip和端口
    //    QString ip = "127.0.0.1";
    //    qint16 port = 8888;
    //    //主动和服务器建立连接
    //    tcp_socket->connectToHost(QHostAddress(ip), port);

    setFixedSize(1440, 900);
    setWindowIcon(QIcon(":/resource/icon/takagi.jpg"));
    setWindowTitle("洛克传奇");

    QPushButton *start = new QPushButton(this);
    QIcon start_icon(":/resource/button/start_game.png");
    start->setIcon(start_icon);
    start->show();

    // 创建和服务器通信的套接字
    tcp_socket = new QTcpSocket(this);

    //    // 获取服务器 IP 和端口
    //    QString ip = "127.0.0.1";
    //    qint16 port = 8888;

    //    // 主动和服务器建立连接
    //    tcp_socket->connectToHost(QHostAddress(ip), port);

    //    connect(tcp_socket, SIGNAL(readyRead()), this, SLOT(receive_from_server()));
}

//切换到登录界面
//void start_game::switch_to_login()
//{
//    //延时进入
//    QTimer::singleShot(100, this, [=] {
//        this->hide();
//        login->show();
//    });
//}

//void start_game::receive_from_server()
//{
//    //从通信套接字取出内容
//    QByteArray array = tcp_socket->readAll();
//    QJsonDocument json_document = QJsonDocument::fromJson(array);
//    QJsonObject json = json_document.object();
//    mo.socket_id = json["socket_id"].toInt();
//    qDebug() << "socket_id:" << mo.socket_id << endl;
//}

start_game::~start_game()
{
    delete ui;
    delete tcp_socket;
    delete login_1;
}

void start_game::on_pushButton_clicked()
{
    //实例化登录场景
    login_1 = new login(tcp_socket);
    //延时进入
    QTimer::singleShot(100, this, [=] {
        this->hide();
        login_1->show();
    });
}
