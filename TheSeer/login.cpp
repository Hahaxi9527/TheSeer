#include "login.h"
//#include "lobby.h"
#include "ui_login.h"
login::login(QTcpSocket *_tcp_socket, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::login)
{
    ui->setupUi(this);
    qDebug() << "创建登录窗口" << endl;
    tcp_socket = _tcp_socket;

    // tcp_socket = new QTcpSocket(this);

    //获取服务器ip和端口
    QString ip = "127.0.0.1";
    qint16 port = 8888;

    //主动和服务器连接
    tcp_socket->connectToHost(QHostAddress(ip), port);
    QJsonObject json = {{"define", SOCKET}};
    tcp_socket->write(QString(QJsonDocument(json).toJson()).toUtf8().data());
    //lobby = new Lobby(tcp_socket);

    setFixedSize(1440, 900);
    setWindowIcon(QIcon(":/resource/icon/takagi.jpg"));
    setWindowTitle("洛克传奇");

    //登录框配置

    ui->password_edit->setEchoMode(QLineEdit::Password); //设置密码不显示

    connect(tcp_socket, SIGNAL(readyRead()), this, SLOT(receive_from_server()));
}

void login::reconnect()
{
    bool flag = connect(tcp_socket, SIGNAL(readyRead()), this, SLOT(receive_from_server()));

    if (flag == true) {
        qDebug() << "login与服务器连接" << endl;
    } else {
        qDebug() << "login未能与服务器连接" << endl;
    }
}

void login::receive_from_server()
{
    //从通信套接字中取出内容
    QByteArray array = tcp_socket->readAll();
    QJsonDocument jsonDocument = QJsonDocument::fromJson(array);
    QJsonObject json = jsonDocument.object();
    //QJsonObject json = ma.receive_from_server();
    qDebug() << "login收到服务器：" << json << endl;
    //    // 检查并保存初始的 socket_id
    //    if (json.contains("socket_id") && mo.socket_id == -1) { // 假设 mo.socket_id 初始值为 0
    //        mo.socket_id = json["socket_id"].toInt();
    //        qDebug() << "保存初始 socket_id:" << mo.socket_id;
    //        return;
    //    }

    QString define = json["define"].toString();
    if (define == SOCKET_1) {
        int receive_socket_id = json["socket_id"].toInt();
        if (mo.socket_id == -1) {
            mo.socket_id = receive_socket_id;
            qDebug() << "mo.sockte_id:" << mo.socket_id << endl;
        }
    }
    if (json["socket_id"] != mo.socket_id) {
        return;
    }
    //密码正确
    if (json["define"] == SIGNIN_SUCCEED) {
        qDebug() << "进入游戏大厅" << endl;
        mo.username = json["username"].toString();
        mo.user_id = json["user_id"].toInt();
        qDebug() << "mo.username:" << mo.username << endl;
        //延时进入
        QTimer::singleShot(100, this, [=] {
            this->close();
            //this->hide();
            qDebug() << "Login释放与服务器连接" << endl;
            /*disconnect(tcp_socket,
                       SIGNAL(readyRead()),
                       this,
                       SLOT(receive_from_server()));*/ //解除连接
            tcp_socket->disconnect(this);
            tcp_socket->connectToHost("127.0.0.1", 8888);
            lobby_1 = new lobby(tcp_socket);

            // 确保 lobby_1 是有效的
            if (lobby_1) {
                lobby_1->reconnect(); // 假设 reconnect 是一个有效的方法
                lobby_1->show();
                //lobby_1->update();
            } else {
                qDebug() << "lobby_1 对象无效";
            }
        });

    }
    //密码错误
    else if (json["define"] == SIGNIN_FAILED) {
        qDebug() << "密码错误" << endl;
        QMessageBox::critical(this, "错误", "密码错误，请重新输入");
    }
    //用户名重复
    else if (json["define"] == REGISTER_FAILED) {
        qDebug() << "用户名已存在" << endl;
        QMessageBox::critical(this, "错误", "用户名已存在");

    }
    //用户未注册
    else if (json["define"] == NOT_REGISTER) {
        QMessageBox::critical(this, "错误", "该用户未注册");

    }
    //注册成功
    else if (json["define"] == REGISTER_SUCCEED) {
        QMessageBox::information(this, "提示", "注册成功");
    }
    //有人已登录
    else if (json["define"] == USER_ONLINE) {
        QMessageBox::information(this, "提示", "有人已登录这个号");
    }
}

/*void login::Signin()
{
    QString name = ui->username_edit->text().toUtf8().data(); //获取用户名
    QString passWord = ui->password_edit->text();             //获取密码
    if (name == "") {
        QMessageBox::critical(this, "错误", "请输入用户名");
        return;
    }
    //如果未输入密码，
    if (passWord == "") {
        QMessageBox::critical(this, "错误", "请输入密码");
        return;
    }
    QJsonObject json = {{"define", SIGNIN},
                        {"socket_id", mo.socket_id},
                        {"username", name},
                        {"password", passWord}};
    //向服务器发送信息
    tcp_socket->write(QString(QJsonDocument(json).toJson()).toUtf8().data());
    qDebug() << json << endl;
}*/

void login::on_sign_in_clicked()
{
    qDebug() << "登录按钮被点击" << endl;

    QString username = ui->username_edit->text().toUtf8().data(); //获取用户名
    QString password = ui->password_edit->text();                 //获取密码
    if (username == "") {
        QMessageBox::critical(this, "错误", "请输入用户名");
        return;
    }
    //如果未输入密码，
    if (password == "") {
        QMessageBox::critical(this, "错误", "请输入密码");
        return;
    }
    QJsonObject json = {{"define", SIGNIN},
                        {"socket_id", mo.socket_id},
                        {"username", username},
                        {"password", password}};
    //向服务器发送信息
    tcp_socket->write(QString(QJsonDocument(json).toJson()).toUtf8().data());
    tcp_socket->flush();
    qDebug() << json << endl;
}

void login::on_login_2_clicked()
{
    qDebug() << "注册按钮被点击" << endl;
    QString name = ui->username_edit->text().toUtf8().data(); //获取用户名
    QString passWord = ui->password_edit->text();             //获取密码
    //如果未输入用户名
    if (name == "") {
        QMessageBox::critical(this, "错误", "请输入用户名");
        return;
    }
    //如果未输入密码，
    if (passWord == "") {
        QMessageBox::critical(this, "错误", "请输入密码");
        return;
    }
    QJsonObject json = {{"define", REGISTER},
                        {"socket_id", mo.socket_id},
                        {"username", name},
                        {"password", passWord}};

    tcp_socket->write(QString(QJsonDocument(json).toJson()).toUtf8().data());
}

login::~login()
{
    delete ui;
    delete tcp_socket;
    //delete lobby;
}
