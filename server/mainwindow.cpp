#include "mainwindow.h"
#include "ui_mainwindow.h"
#pragma execution_character_set("utf-8")

database mo;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    // 设置全局编码为 UTF-8
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForLocale(codec);
    ui->setupUi(this);

    //监听套接字，指定父对象
    tcp_server = new QTcpServer(this);

    tcp_server->listen(QHostAddress::Any, 8888);

    setWindowTitle("服务器端口：8888");

    connect(tcp_server, &QTcpServer::newConnection, this, &MainWindow::onNewConnection);
}

void MainWindow::onNewConnection()
{
    tcp_socket = tcp_server->nextPendingConnection();
    socket_list.append(tcp_socket);

    QJsonObject json = {{"define", SOCKET_1}, {"socket_id", socket_id}};
    socket_id++;

    tcp_socket->write(QJsonDocument(json).toJson());
    tcp_socket->flush();

    connect(tcp_socket, &QTcpSocket::readyRead, this, &MainWindow::onReadyRead);
    connect(tcp_socket, &QTcpSocket::disconnected, this, &MainWindow::onClientDisconnected);
}

void MainWindow::onReadyRead()
{
    QTcpSocket *client_socket = qobject_cast<QTcpSocket *>(sender());
    if (!client_socket) {
        return;
    }
    //从通信套接字中取出内容
    QByteArray array = client_socket->readAll();
    QJsonDocument json_document = QJsonDocument::fromJson(array);
    QJsonObject json = json_document.object();

    qDebug() << "请求为" << json["define"] << "操作" << endl;

    if (json["define"] == SOCKET) {
        qDebug() << "发送socket_id请求" << endl;
    }
    //登录请求
    else if (json["define"] == SIGNIN) {
        qDebug() << "登录请求" << endl;
        Sign_in(json);
    }
    //注册请求
    else if (json["define"] == REGISTER) {
        qDebug() << "注册请求" << endl;
        Register(json);
    }
    //登出请求
    else if (json["define"] == SIGNOUT) {
        qDebug() << "登出请求" << endl;
        Sign_out(json);
    }
    //大厅查询信息请求
    else if (json["define"] == LOBBY) {
        qDebug() << "大厅请求" << endl;
        Lobby(json);
    }
    //充值信息请求
    else if (json["define"] == RECHARGE) {
        qDebug() << "充值请求" << endl;
        Recharge(json);
    }
    //用户列表请求
    else if (json["define"] == SWITCH_TO_USERLIST) {
        qDebug() << "用户列表请求" << endl;
        User_list(json);
    }
    //宠物背包请求
    else if (json["define"] == SWITCH_TO_PETBAG) {
        qDebug() << "宠物背包请求" << endl;
        Pet_list(json);
    }
    //添加宠物作弊请求
    else if (json["define"] == ADDPETS) {
        qDebug() << "添加宠物作弊请求" << endl;
        Addpets(json);
    }
    //战斗信息请求
    else if (json["define"] == SWITCH_TO_BATTLE_CHOOSE) {
        qDebug() << "战斗信息请求" << endl;
        Battle_choose(json);
    }
    //刷新服务器宠物请求
    else if (json["define"] == FLUSH_SERVER) {
        qDebug() << "刷新服务器宠物请求" << endl;
        Flush_server_pets(json);
    }
    //增加服务器宠物请求
    else if (json["define"] == ADD_SERVER_PETS) {
        qDebug() << "增加服务器宠物请求" << endl;
        Add_server_pets(json);
    }
    //开始升级战请求
    else if (json["define"] == START_UPGRADE_BATTLE) {
        qDebug() << "开始升级战请求" << endl;
        Start_upgrade_battle(json);
    }
    //开始决斗战请求
    else if (json["define"] == START_VS_BATTLE) {
        qDebug() << "开始决斗战请求" << endl;
        Start_vs_battle(json);
    }
    //升级战胜利请求
    else if (json["define"] == UPGRADE_WIN) {
        qDebug() << "升级战胜利请求" << endl;
        Upgrade_win(json);
    }
    //升级战失败请求
    else if (json["define"] == UPGRADE_LOSE) {
        qDebug() << "升级战失败请求" << endl;
        Upgrade_lose(json);
    }
    //决斗战胜利请求
    else if (json["define"] == VS_WIN) {
        qDebug() << "决斗战胜利请求" << endl;
        Vs_win(json);
    }
    //决斗战失败请求
    else if (json["define"] == VS_LOSE) {
        qDebug() << "决斗战落败请求" << endl;
        Vs_lose(json);
    }
    //失去宠物请求
    else if (json["define"] == ABANDON) {
        qDebug() << "失去宠物请求" << endl;
        Abandon_pet(json);
    }
    //宠物升级请求
    else if (json["define"] == UPGRADE) {
        qDebug() << "宠物升级请求" << endl;
        Upgrade(json);
    } else {
        qDebug() << "无法识别" << endl;
    }
}

void MainWindow::onClientDisconnected()
{
    QTcpSocket *client_socket = qobject_cast<QTcpSocket *>(sender());
    if (!client_socket) {
        return;
    }
    socket_list.removeAll(client_socket);
    client_socket->deleteLater();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::Sign_in(QJsonObject json)
{
    QSqlQuery query;
    QString username = json["username"].toString();
    QString password = json["password"].toString();
    QString result;
    int is_online;
    int user_id = -1;
    int _socket_id = json["socket_id"].toInt();
    qDebug() << "username:" << username << endl;

    QString sql = QString("SELECT password, user_id,is_online FROM users WHERE username = '%1';")
                      .arg(username);
    qDebug() << "sql:" << sql << endl;

    if (query.exec(sql)) {
        if (query.next()) {
            result = query.value(0).toString(); // 获取查询到的密码
            user_id = query.value(1).toInt();   // 获取查询到的用户ID
            is_online = query.value(2).toInt();
            qDebug() << "数据库中存储的密码是：" << result << endl;

            // 假如密码正确
            if (result == password) {
                if (is_online == 0) {
                    qDebug() << "密码正确" << endl;
                    json = {{"define", SIGNIN_SUCCEED},
                            {"socket_id", _socket_id},
                            {"username", username},
                            {"user_id", user_id}};
                    qDebug() << QString(QJsonDocument(json).toJson()) << endl;

                    // 更新数据库，设置用户登录状态为登录
                    sql = QString("UPDATE users SET is_online = 1 WHERE username = '%1';")
                              .arg(username);
                    if (!query.exec(sql)) {
                        qDebug() << "更新用户登录状态失败：" << query.lastError().text();
                    }
                } else {
                    qDebug() << "密码正确，有人已登录" << endl;
                    json = {{"define", USER_ONLINE},
                            {"socket_id", _socket_id},
                            {"username", username},
                            {"user_id", user_id}};
                    qDebug() << QString(QJsonDocument(json).toJson()) << endl;
                }

            } else {
                // 密码错误
                json = {{"define", SIGNIN_FAILED},
                        {"socket_id", _socket_id},
                        {"reason", "Incorrect password"}};
            }
        } else {
            // 用户名不存在
            json = {{"define", NOT_REGISTER},
                    {"socket_id", _socket_id},
                    {"reason", "User not registered"}};
            qDebug() << "用户未注册: " << username;
        }
    } else {
        qDebug() << "查询用户名时出错: " << query.lastError().text();
        json = {{"define", SIGNIN_FAILED},
                {"socket_id", _socket_id},
                {"reason", "Database query error"}};
    }

    qDebug() << "服务器传回：" << json << endl;
    send_message(json);
}

void MainWindow::Register(QJsonObject json)
{
    // 打开数据库连接
    mo.open_database();
    if (!mo.is_open) {
        qDebug() << "数据库连接失败，无法进行注册操作";
        return;
    }
    qDebug() << "注册第一步" << endl;
    QSqlQuery query(mo.db); // 指定数据库连接

    QString username = json["username"].toString();
    QString password = json["password"].toString();
    int _socket_id = json["socket_id"].toInt();
    QString sql;

    qDebug() << "注册第二步" << endl;
    //判断用户名是否存在
    sql = QString("SELECT username FROM users WHERE username = '%1';").arg(username);
    query.exec(sql);
    if (!query.exec(sql)) {
        qDebug() << "查询用户名执行失败:" << query.lastError().text();
    }
    if (query.next()) {
        qDebug() << "用户名已存在，注册失败" << endl;
        json = {
            {"define", REGISTER_FAILED},
            {"socket_id", _socket_id},
        };
    } else {
        //在数据库中插入新用户信息
        qDebug() << "插入新用户信息" << endl;
        sql = QString("INSERT INTO users "
                      "(username, password, is_online, matches_played, matches_won, win_rate, "
                      "num_pets, num_high_level_pets, money) "
                      "VALUES ('%1', '%2', 0, 0, 0, 0, 3, 0, 0);")
                  .arg(username)
                  .arg(password);
        if (!query.exec(sql)) {
            qDebug() << "注册信息插入失败:" << query.lastError().text();
            return;
        }
        qDebug() << "注册信息插入成功" << endl;

        //提交事务
        if (!mo.db.commit()) {
            qDebug() << "事务提交失败:" << mo.db.lastError().text();
            return;
        }

        for (int i = 0; i < 3; i++) {
            add_pet();
        }

        //查找用户id
        sql = QString("SELECT user_id FROM users WHERE username = '%1';").arg(username);
        if (!query.exec(sql)) {
            qDebug() << "查询用户id失败:" << query.lastError().text();
            return;
        }
        // 检查是否有结果行并提取 user_id
        int user_id = -1; // 初始化为一个无效的 ID 值，以便在没有找到用户时使用
        if (query.next()) {
            user_id = query.value(0).toInt(); // 获取结果集的第一列值，也就是 user_id
            qDebug() << "用户ID为:" << user_id;
        } else {
            qDebug() << "未找到对应的用户";
        }
        assign_pets_to_user(user_id);
        json = {{"define", REGISTER_SUCCEED}, {"socket_id", _socket_id}};
    }
    qDebug() << "注册第三步" << endl;
    send_message(json);
}

void MainWindow::Sign_out(QJsonObject json)
{
    qDebug() << "登出" << endl;

    // 获取 user_id 和 socket_id
    int user_id = json["user_id"].toInt();
    int _socket_id = json["socket_id"].toInt();

    // 创建 SQL 查询对象
    QSqlQuery query;

    // 构建 SQL 更新语句，将用户的在线状态设为 0
    QString sql = QString("UPDATE users SET is_online = 0 WHERE user_id = %1;").arg(user_id);

    // 执行 SQL 查询并检查结果
    if (!query.exec(sql)) {
        // 如果更新操作失败，记录错误信息
        qDebug() << "更新用户在线状态失败:" << query.lastError();
        return;
    }

    qDebug() << "用户" << user_id << "的在线状态已更新为离线";

    // 构建登出成功的 JSON 消息
    QJsonObject responseJson;
    responseJson["define"] = SIGNOUT_SUCCEED; // 你可能需要根据你的协议定义这个标识
    responseJson["socket_id"] = _socket_id;
    responseJson["user_id"] = user_id;

    // 发送登出成功的消息
    send_message(responseJson);

    // 关闭与客户端的 TCP 连接（假设有一个 socket 列表在管理连接）
    QTcpSocket *socket = getSocketById(_socket_id); // 你可能有这样一个函数来获取 socket
    if (socket) {
        socket->close();
        qDebug() << "socket_id" << _socket_id << "的连接已关闭";
        // 从 socket 列表中移除该连接
        socket_list.removeOne(socket); // 假设有一个 socket_list 保存所有连接
        socket->deleteLater();         // 安排在合适的时间点删除 socket 对象
    } else {
        qDebug() << "未找到对应的 socket ID" << _socket_id;
    }
}

// 假设有一个根据 socket_id 获取 socket 的函数
QTcpSocket *MainWindow::getSocketById(int socket_id)
{
    // 遍历 socket_list，找到匹配的 socket
    for (QTcpSocket *socket : socket_list) {
        if (socket->property("socket_id").toInt() == socket_id) {
            return socket;
        }
    }
    return nullptr;
}

void MainWindow::Recharge(QJsonObject json)
{
    qDebug() << "充值" << endl;
    QString username = json["username"].toString();
    int _socket_id = json["socket_id"].toInt();
    QSqlQuery query;
    QString sql;
    sql = QString("UPDATE users SET money=money+500 WHERE username = '%1';").arg(username);
    if (query.exec(sql)) {
        sql = QString("SELECT money FROM users WHERE username = '%1'").arg(username);
        query.exec(sql);
        if (query.next()) { // 进行了查询之后需要移动查询结果的游标到第一行
            int money = query.value(0).toInt();
            json = {{"define", RECHARGE_SUCCESSED}, {"socket_id", _socket_id}, {"money", money}};
            send_message(json); // 在充值成功后发送新的金额给客户端
        } else {
            qDebug() << "查询用户金额失败" << endl;
        }
    } else {
        qDebug() << "充值失败" << endl;
        json = {{"define", RECHARGE_FAILED}, {"socket_id", _socket_id}};
        send_message(json);
    }
}

void MainWindow::User_list(QJsonObject json)
{
    // 打开数据库连接
    mo.open_database();
    if (!mo.is_open) {
        qDebug() << "数据库连接失败，无法进行用户列表查询操作";
        return;
    }
    qDebug() << "处理用户列表查询请求";

    int _socket_id = json["socket_id"].toInt();
    // 查询所有用户信息
    QSqlQuery query(mo.db); // 指定数据库连接
    QString sql = "SELECT username, is_online, num_pets, num_high_level_pets, win_rate FROM users";

    QJsonArray userlist_array;
    if (query.exec(sql)) {
        while (query.next()) {
            QJsonObject user_json;
            user_json["username"] = query.value("username").toString();
            user_json["is_online"] = query.value("is_online").toInt();
            user_json["num_pets"] = query.value("num_pets").toInt();
            user_json["num_high_level_pets"] = query.value("num_high_level_pets").toInt();
            user_json["win_rate"] = query.value("win_rate").toFloat();
            userlist_array.append(user_json);
        }

        // 构建响应消息
        QJsonObject response_json = {{"define", USERLIST_INFO},
                                     {"socket_id", _socket_id},
                                     {"userlist", userlist_array}};
        send_message(response_json);

        //        // 将响应消息发送回客户端
        //        sendJsonToClient(clientSocket, responseJson);

        qDebug() << "发送用户列表信息:" << response_json;

    } else {
        qDebug() << "查询用户列表失败: " << query.lastError().text();
    }
}

void MainWindow::Pet_list(QJsonObject json)
{
    // 打开数据库连接
    mo.open_database();
    if (!mo.is_open) {
        qDebug() << "数据库连接失败，无法进行宠物信息查询操作";
        return;
    }
    qDebug() << "处理宠物信息查询请求";

    int _socket_id = json["socket_id"].toInt();
    int user_id = json["user_id"].toInt();
    int money;

    // 查询数据库获取用户的宠物信息
    QSqlQuery query(mo.db);
    QString sql = QString("SELECT * FROM pets WHERE user_id = '%1'").arg(user_id);

    QJsonArray user_pets;
    if (query.exec(sql)) {
        while (query.next()) {
            QJsonObject pet;
            pet["pet_id"] = query.value("pet_id").toInt();
            pet["pet_name"] = query.value("pet_name").toString();
            pet["pet_attribute"] = query.value("pet_attribute").toInt();
            pet["level"] = query.value("level").toInt();
            pet["is_evolved"] = query.value("is_evolved").toInt();
            pet["exp"] = query.value("exp").toInt();
            pet["damage_point"] = query.value("damage_point").toDouble();
            pet["defense_point"] = query.value("defense_point").toDouble();
            pet["hp"] = query.value("hp").toDouble();
            pet["attack_interval"] = query.value("attack_interval").toDouble();
            // 其他宠物信息字段类似地添加到 pet 对象中

            user_pets.append(pet);
        }
    } else {
        qDebug() << "查询用户宠物信息失败: " << query.lastError().text();
    }

    sql = QString("SELECT money FROM users WHERE user_id = '%1'").arg(user_id);

    if (query.exec(sql)) {
        while (query.next()) {
            money = query.value("money").toInt();
        }
    } else {
        qDebug() << "查询用户金币信息失败: " << query.lastError().text();
    }

    // 构建响应消息
    QJsonObject response = {{"define", PET_INFO},
                            {"socket_id", _socket_id},
                            {"user_pets", user_pets},
                            {"money", money}};

    send_message(response);
}

void MainWindow::Addpets(QJsonObject json)
{
    qDebug() << "作弊添加宠物的实现" << endl;
    // 打开数据库连接
    mo.open_database();
    if (!mo.is_open) {
        qDebug() << "数据库连接失败，无法进行添加宠物操作";
        return;
    }

    QSqlQuery query(mo.db); // 指定数据库连接

    QString username = json["username"].toString();
    int user_id = json["user_id"].toInt();
    int _socket_id = json["socket_id"].toInt();

    QString pet_name;
    int pet_attribute;
    user_id = json["user_id"].toInt();
    int level = 15;
    int is_evolved = 1;
    int exp = 0;
    double damage_point;
    double defense_point;
    double hp;
    double attack_interval;
    double fight_damage_point;
    double fight_defense_point;
    double fight_hp;
    double fight_attack_interval;

    int rand_num1 = QRandomGenerator::global()->bounded(0, 8);
    if (rand_num1 == SONIC_DOG) {
        pet_name = "音速犬";
        pet_attribute = 0;
        damage_point = 400;
        fight_damage_point = damage_point;
        defense_point = 148;
        fight_defense_point = defense_point;
        hp = 670;
        fight_hp = hp;
        attack_interval = 1.16;
        fight_attack_interval = attack_interval;
    } else if (rand_num1 == BLAZE_GOD_OF_WAR) {
        pet_name = "烈火战神";
        pet_attribute = 0;
        damage_point = 400;
        fight_damage_point = damage_point;
        defense_point = 148;
        fight_defense_point = defense_point;
        hp = 670;
        fight_hp = hp;
        attack_interval = 1.16;
        fight_attack_interval = attack_interval;
    } else if (rand_num1 == BLACK_BEAR_BERSERK) {
        pet_name = "黑熊狂战";
        pet_attribute = 1;
        damage_point = 152;
        fight_damage_point = damage_point;
        defense_point = 200;
        fight_defense_point = defense_point;
        hp = 1400;
        fight_hp = hp;
        attack_interval = 1.58;
        fight_attack_interval = attack_interval;
    } else if (rand_num1 == ARES) {
        pet_name = "阿瑞斯";
        pet_attribute = 1;
        damage_point = 152;
        fight_damage_point = damage_point;
        defense_point = 200;
        fight_defense_point = defense_point;
        hp = 1400;
        fight_hp = hp;
        attack_interval = 1.58;
        fight_attack_interval = attack_interval;
    } else if (rand_num1 == ROCK_ARMOR_LORD) {
        pet_name = "岩铠领主";
        pet_attribute = 2;
        damage_point = 138;
        fight_damage_point = damage_point;
        defense_point = 225;
        fight_defense_point = defense_point;
        hp = 1140;
        fight_hp = hp;
        attack_interval = 1.58;
        fight_attack_interval = attack_interval;
    } else if (rand_num1 == AMBER_KNIGHT) {
        pet_name = "琥珀骑士";
        pet_attribute = 2;
        damage_point = 138;
        fight_damage_point = damage_point;
        defense_point = 225;
        fight_defense_point = defense_point;
        hp = 1140;
        fight_hp = hp;
        attack_interval = 1.58;
        fight_attack_interval = attack_interval;
    } else if (rand_num1 == SWIFT_QUINN) {
        pet_name = "疾驰奎因";
        pet_attribute = 3;
        damage_point = 225;
        fight_damage_point = damage_point;
        defense_point = 110;
        fight_defense_point = defense_point;
        hp = 670;
        fight_hp = hp;
        attack_interval = 0.8;
        fight_attack_interval = attack_interval;
    } else if (rand_num1 == DIMO) {
        pet_name = "迪莫";
        pet_attribute = 3;
        damage_point = 225;
        fight_damage_point = damage_point;
        defense_point = 110;
        fight_defense_point = defense_point;
        hp = 670;
        fight_hp = hp;
        attack_interval = 0.8;
        fight_attack_interval = attack_interval;
    }

    qDebug() << "插入作弊宠物到数据库" << endl;
    // 构建 SQL 插入语句
    QString sql
        = QString("INSERT INTO pets "
                  "(pet_name, pet_attribute, user_id, level, is_evolved, exp, damage_point, "
                  "defense_point, hp, attack_interval, fight_damage_point, fight_defense_point, "
                  "fight_hp, fight_attack_interval) "
                  "VALUES ('%1', '%2', %3, %4, %5, %6, %7, %8, %9, %10, %11, %12, %13, %14);")
              .arg(pet_name)
              .arg(pet_attribute)
              .arg(user_id)
              .arg(level)
              .arg(is_evolved) // 转换布尔值为整数（0 或 1）
              .arg(exp)
              .arg(damage_point)
              .arg(defense_point)
              .arg(hp)
              .arg(attack_interval)
              .arg(fight_damage_point)
              .arg(fight_defense_point)
              .arg(fight_hp)
              .arg(fight_attack_interval);

    // 执行 SQL 语句
    if (!query.exec(sql)) {
        qDebug() << "添加宠物信息失败:" << query.lastError().text();
        return;
    }

    qDebug() << "宠物信息添加成功";

    // 查询用户的宠物数量和高级宠物数量
    QString countQueryStr
        = QString("SELECT COUNT(*) as pet_count, "
                  "SUM(CASE WHEN level >= 15 THEN 1 ELSE 0 END) as high_level_pet_count "
                  "FROM pets WHERE user_id = %1;")
              .arg(user_id);

    if (!query.exec(countQueryStr)) {
        qDebug() << "查询宠物数量失败:" << query.lastError().text();
        return;
    }

    int pet_count = 0;
    int high_level_pet_count = 0;

    if (query.next()) {
        pet_count = query.value("pet_count").toInt();
        high_level_pet_count = query.value("high_level_pet_count").toInt();
    }

    // 更新用户表中的宠物数量和高级宠物数量
    QString updateQueryStr
        = QString("UPDATE users SET num_pets = %1, num_high_level_pets = %2 WHERE user_id = %3;")
              .arg(pet_count)
              .arg(high_level_pet_count)
              .arg(user_id);

    if (!query.exec(updateQueryStr)) {
        qDebug() << "更新用户宠物数量失败:" << query.lastError().text();
        return;
    }

    qDebug() << "用户宠物数量和高级宠物数量更新成功";
}

void MainWindow::add_pet()
{
    // 打开数据库连接
    mo.open_database();
    if (!mo.is_open) {
        qDebug() << "数据库连接失败，无法进行添加宠物操作";
        return;
    }

    QSqlQuery query(mo.db); // 指定数据库连接

    QString pet_name;
    int pet_attribute;
    int user_id = 0;
    int level = 1;
    int is_evolved = 0;
    int exp = 0;
    double damage_point;
    double defense_point;
    double hp;
    double attack_interval;
    double fight_damage_point;
    double fight_defense_point;
    double fight_hp;
    double fight_attack_interval;

    int rand_num1 = QRandomGenerator::global()->bounded(0, 8);
    if (rand_num1 == SONIC_DOG) {
        pet_name = "音速犬";
        pet_attribute = 0;
        damage_point = 50;
        fight_damage_point = damage_point;
        defense_point = 50;
        fight_defense_point = defense_point;
        hp = 250;
        fight_hp = hp;
        attack_interval = 2;
        fight_attack_interval = attack_interval;
    } else if (rand_num1 == BLAZE_GOD_OF_WAR) {
        pet_name = "烈火战神";
        pet_attribute = 0;
        damage_point = 50;
        fight_damage_point = damage_point;
        defense_point = 50;
        fight_defense_point = defense_point;
        hp = 250;
        fight_hp = hp;
        attack_interval = 2;
        fight_attack_interval = attack_interval;
    } else if (rand_num1 == BLACK_BEAR_BERSERK) {
        pet_name = "黑熊狂战";
        pet_attribute = 1;
        damage_point = 40;
        fight_damage_point = damage_point;
        defense_point = 60;
        fight_defense_point = defense_point;
        hp = 350;
        fight_hp = hp;
        attack_interval = 2;
        fight_attack_interval = attack_interval;
    } else if (rand_num1 == ARES) {
        pet_name = "阿瑞斯";
        pet_attribute = 1;
        damage_point = 40;
        fight_damage_point = damage_point;
        defense_point = 60;
        fight_defense_point = defense_point;
        hp = 350;
        fight_hp = hp;
        attack_interval = 2;
        fight_attack_interval = attack_interval;
    } else if (rand_num1 == ROCK_ARMOR_LORD) {
        pet_name = "岩铠领主";
        pet_attribute = 2;
        damage_point = 40;
        fight_damage_point = damage_point;
        defense_point = 70;
        fight_defense_point = defense_point;
        hp = 300;
        fight_hp = hp;
        attack_interval = 2;
        fight_attack_interval = attack_interval;
    } else if (rand_num1 == AMBER_KNIGHT) {
        pet_name = "琥珀骑士";
        pet_attribute = 2;
        damage_point = 40;
        fight_damage_point = damage_point;
        defense_point = 70;
        fight_defense_point = defense_point;
        hp = 300;
        fight_hp = hp;
        attack_interval = 2;
        fight_attack_interval = attack_interval;
    } else if (rand_num1 == SWIFT_QUINN) {
        pet_name = "疾驰奎因";
        pet_attribute = 3;
        damage_point = 45;
        fight_damage_point = damage_point;
        defense_point = 40;
        fight_defense_point = defense_point;
        hp = 250;
        fight_hp = hp;
        attack_interval = 1.5;
        fight_attack_interval = attack_interval;
    } else if (rand_num1 == DIMO) {
        pet_name = "迪莫";
        pet_attribute = 3;
        damage_point = 45;
        fight_damage_point = damage_point;
        defense_point = 40;
        fight_defense_point = defense_point;
        hp = 250;
        fight_hp = hp;
        attack_interval = 1.5;
        fight_attack_interval = attack_interval;
    }

    qDebug() << "插入宠物到数据库" << endl;
    // 构建 SQL 插入语句
    QString sql
        = QString("INSERT INTO pets "
                  "(pet_name, pet_attribute, user_id, level, is_evolved, exp, damage_point, "
                  "defense_point, hp, attack_interval, fight_damage_point, fight_defense_point, "
                  "fight_hp, fight_attack_interval) "
                  "VALUES ('%1', '%2', %3, %4, %5, %6, %7, %8, %9, %10, %11, %12, %13, %14);")
              .arg(pet_name)
              .arg(pet_attribute)
              .arg(user_id)
              .arg(level)
              .arg(is_evolved) // 转换布尔值为整数（0 或 1）
              .arg(exp)
              .arg(damage_point)
              .arg(defense_point)
              .arg(hp)
              .arg(attack_interval)
              .arg(fight_damage_point)
              .arg(fight_defense_point)
              .arg(fight_hp)
              .arg(fight_attack_interval);

    // 执行 SQL 语句
    if (!query.exec(sql)) {
        qDebug() << "添加宠物信息失败:" << query.lastError().text();
        return;
    }

    qDebug() << "宠物信息添加成功";
}

void MainWindow::send_message(QJsonObject json)
{
    int size = socket_list.size();
    qDebug() << "向" << size << "个客户发送信息" << endl;
    //给所有的客户端发送信息
    for (int i = 0; i < size; i++) {
        QTcpSocket *tem = socket_list[i];
        tem->write(QString(QJsonDocument(json).toJson()).toUtf8().data());
    }
}

void MainWindow::assign_pets_to_user(int user_id)
{
    // 打开数据库连接
    mo.open_database();
    if (!mo.is_open) {
        qDebug() << "数据库连接失败，无法进行操作";
        return;
    }

    QSqlQuery query(mo.db); // 使用数据库连接

    // 查询 `user_id` 为 `0` 的宠物，限制结果数量为3
    QString sql = QString("SELECT pet_id FROM pets WHERE user_id = 0 LIMIT 3;");
    if (!query.exec(sql)) {
        qDebug() << "查询可分配宠物失败:" << query.lastError().text();
        return;
    }

    // 用于存储要更新的宠物 ID
    QVector<int> pet_ids;

    // 获取查询结果
    while (query.next()) {
        int pet_id = query.value(0).toInt(); // 获取当前行的 `pet_id`
        pet_ids.append(pet_id);
    }

    // 如果查询结果为空，说明没有可分配的宠物
    if (pet_ids.isEmpty()) {
        qDebug() << "没有可分配的宠物";
        return;
    }

    // 更新这些宠物的 `user_id` 为指定的 `userId`
    for (int pet_id : pet_ids) {
        QString updateSql
            = QString("UPDATE pets SET user_id = %1 WHERE pet_id = %2;").arg(user_id).arg(pet_id);
        if (!query.exec(updateSql)) {
            qDebug() << "更新宠物信息失败:" << query.lastError().text();
            return;
        }
    }

    qDebug() << "已成功分配宠物给用户, user_id:" << user_id << ", pet_ids:" << pet_ids;
}

void MainWindow::Lobby(QJsonObject json)
{
    // 打开数据库连接
    mo.open_database();
    if (!mo.is_open) {
        qDebug() << "数据库连接失败，无法进行大厅查询操作";
        return;
    }
    qDebug() << "查询大厅信息第一步" << endl;
    QSqlQuery query(mo.db); // 指定数据库连接

    qDebug() << "Lobby传来：" << json << endl;
    QString username = json["username"].toString();
    int _socket_id = json["socket_id"].toInt();

    QString sql
        = QString("SELECT num_pets, num_high_level_pets, money FROM users WHERE username = '%1';")
              .arg(username);
    if (query.exec(sql) && query.next()) {
        int money = query.value(2).toInt();               //用户金钱
        int num_pets = query.value(0).toInt();            //宠物个数
        int num_high_level_pets = query.value(1).toInt(); //高级宠物个数
        json = {{"define", LOBBY_SUCCEED},
                {"socket_id", _socket_id},
                {"money", money},
                {"num_pets", num_pets},
                {"num_high_level_pets", num_high_level_pets}};
        qDebug() << "传给Lobby：" << json << endl;
        //tcpSocket->write(QString(QJsonDocument(json).toJson()).toUtf8().data());
        send_message(json);
    } else {
        qDebug() << "查询大厅失败" << endl;
    }
}

void MainWindow::Battle_choose(QJsonObject json)
{
    int user_id = json["user_id"].toInt();
    int _socket_id = json["socket_id"].toInt();

    // 打开数据库连接
    mo.open_database();
    if (!mo.is_open) {
        qDebug() << "数据库连接失败，无法获取宠物信息";
        return;
    }

    QSqlQuery query(mo.db);

    // 查询该用户的所有宠物信息
    QString sql = QString("SELECT pet_id, pet_name, pet_attribute, level, is_evolved, exp, "
                          "damage_point, defense_point, hp, attack_interval, fight_damage_point, "
                          "fight_defense_point, fight_hp, fight_attack_interval "
                          "FROM pets WHERE user_id = %1")
                      .arg(user_id);

    if (!query.exec(sql)) {
        qDebug() << "查询宠物信息失败:" << query.lastError().text();
        return;
    }

    // 构建 JSON 数组
    QJsonArray pets_array;

    while (query.next()) {
        QJsonObject petObject;
        petObject["pet_id"] = query.value("pet_id").toInt();
        petObject["pet_name"] = query.value("pet_name").toString();
        petObject["pet_attribute"] = query.value("pet_attribute").toInt();
        petObject["level"] = query.value("level").toInt();
        petObject["is_evolved"] = query.value("is_evolved").toInt();
        petObject["exp"] = query.value("exp").toInt();
        petObject["damage_point"] = query.value("damage_point").toDouble();
        petObject["defense_point"] = query.value("defense_point").toDouble();
        petObject["hp"] = query.value("hp").toDouble();
        petObject["attack_interval"] = query.value("attack_interval").toDouble();
        petObject["fight_damage_point"] = query.value("fight_damage_point").toDouble();
        petObject["fight_defense_point"] = query.value("fight_defense_point").toDouble();
        petObject["fight_hp"] = query.value("fight_hp").toDouble();
        petObject["fight_attack_interval"] = query.value("fight_attack_interval").toDouble();

        pets_array.append(petObject);
    }

    // SQL 查询语句，限制获取10只宠物
    QString sql_1 = QString("SELECT pet_id, pet_name, pet_attribute, level, is_evolved, exp, "
                            "damage_point, defense_point, hp, attack_interval, fight_damage_point, "
                            "fight_defense_point, fight_hp, fight_attack_interval "
                            "FROM server_pets "
                            "ORDER BY RAND() "
                            "LIMIT 10");

    // 执行 SQL 查询
    if (!query.exec(sql_1)) {
        qDebug() << "从服务器宠物数据库获取信息失败:" << query.lastError().text();
        return;
    }

    // 构建 JSON 数组
    QJsonArray server_pets_array;

    // 遍历查询结果
    while (query.next()) {
        QJsonObject petObject;
        petObject["pet_id"] = query.value("pet_id").toInt();
        petObject["pet_name"] = query.value("pet_name").toString();
        petObject["pet_attribute"] = query.value("pet_attribute").toInt();
        petObject["level"] = query.value("level").toInt();
        petObject["is_evolved"] = query.value("is_evolved").toInt();
        petObject["exp"] = query.value("exp").toInt();
        petObject["damage_point"] = query.value("damage_point").toDouble();
        petObject["defense_point"] = query.value("defense_point").toDouble();
        petObject["hp"] = query.value("hp").toDouble();
        petObject["attack_interval"] = query.value("attack_interval").toDouble();
        petObject["fight_damage_point"] = query.value("fight_damage_point").toDouble();
        petObject["fight_defense_point"] = query.value("fight_defense_point").toDouble();
        petObject["fight_hp"] = query.value("fight_hp").toDouble();
        petObject["fight_attack_interval"] = query.value("fight_attack_interval").toDouble();

        // 将宠物信息添加到 JSON 数组中
        server_pets_array.append(petObject);
    }
    // 构建响应 JSON 对象
    QJsonObject responseJson;
    responseJson["define"] = BATTLE_CHOOSE;
    responseJson["socket_id"] = _socket_id;
    responseJson["user_id"] = user_id;
    responseJson["pets_array"] = pets_array;
    responseJson["server_pets_array"] = server_pets_array;

    send_message(responseJson);
}

void MainWindow::Flush_server_pets(QJsonObject json)
{
    // 打开数据库连接
    mo.open_database();
    if (!mo.is_open) {
        qDebug() << "数据库连接失败，无法进行查询操作";
        return;
    }
    qDebug() << "刷新服务器宠物第一步" << endl;
    QSqlQuery query(mo.db); // 指定数据库连接

    int _socket_id = json["socket_id"].toInt();

    // SQL 查询语句，限制获取10只宠物
    QString sql_1 = QString("SELECT pet_id, pet_name, pet_attribute, level, is_evolved, exp, "
                            "damage_point, defense_point, hp, attack_interval, fight_damage_point, "
                            "fight_defense_point, fight_hp, fight_attack_interval "
                            "FROM server_pets "
                            "ORDER BY RAND() "
                            "LIMIT 10");

    // 执行 SQL 查询
    if (!query.exec(sql_1)) {
        qDebug() << "从服务器宠物数据库获取信息失败:" << query.lastError().text();
        return;
    }

    // 构建 JSON 数组
    QJsonArray server_pets_array;

    // 遍历查询结果
    while (query.next()) {
        QJsonObject petObject;
        petObject["pet_id"] = query.value("pet_id").toInt();
        petObject["pet_name"] = query.value("pet_name").toString();
        petObject["pet_attribute"] = query.value("pet_attribute").toInt();
        petObject["level"] = query.value("level").toInt();
        petObject["is_evolved"] = query.value("is_evolved").toInt();
        petObject["exp"] = query.value("exp").toInt();
        petObject["damage_point"] = query.value("damage_point").toDouble();
        petObject["defense_point"] = query.value("defense_point").toDouble();
        petObject["hp"] = query.value("hp").toDouble();
        petObject["attack_interval"] = query.value("attack_interval").toDouble();
        petObject["fight_damage_point"] = query.value("fight_damage_point").toDouble();
        petObject["fight_defense_point"] = query.value("fight_defense_point").toDouble();
        petObject["fight_hp"] = query.value("fight_hp").toDouble();
        petObject["fight_attack_interval"] = query.value("fight_attack_interval").toDouble();

        // 将宠物信息添加到 JSON 数组中
        server_pets_array.append(petObject);
    }
    // 构建响应 JSON 对象
    QJsonObject responseJson;
    responseJson["define"] = FLUSH_SERVER_SUCCESSED;
    responseJson["socket_id"] = _socket_id;
    responseJson["server_pets_array"] = server_pets_array;

    send_message(responseJson);
}

void MainWindow::Add_server_pets(QJsonObject json)
{
    // 打开数据库连接
    mo.open_database();
    if (!mo.is_open) {
        qDebug() << "数据库连接失败，无法进行查询操作";
        return;
    }
    qDebug() << "增加服务器宠物第一步" << endl;
    QSqlQuery query(mo.db); // 指定数据库连接

    int _socket_id = json["socket_id"].toInt();

    QString pet_name;
    int pet_attribute;
    int user_id = 0;
    int level;
    int is_evolved = 0;
    int exp = 0;
    double damage_point;
    double defense_point;
    double hp;
    double attack_interval;
    double fight_damage_point;
    double fight_defense_point;
    double fight_hp;
    double fight_attack_interval;

    for (int i = 0; i < 3; i++) {
        int rand_num1 = QRandomGenerator::global()->bounded(0, 8);
        level = QRandomGenerator::global()->bounded(1, 16);
        if (rand_num1 == SONIC_DOG) {
            pet_name = "音速犬";
            pet_attribute = 0;
            damage_point = 50;
            defense_point = 50;
            hp = 250;
            attack_interval = 2;
            if (level >= 6) {
                is_evolved = 1;
            }
            for (int i = 1; i < level; i++) {
                damage_point = damage_point + 25;
                defense_point = defense_point + 7;
                attack_interval = attack_interval - 0.06;
            }
            fight_damage_point = damage_point;
            fight_defense_point = defense_point;
            fight_attack_interval = attack_interval;
            fight_hp = hp;
        } else if (rand_num1 == BLAZE_GOD_OF_WAR) {
            pet_name = "烈火战神";
            pet_attribute = 0;
            damage_point = 50;
            defense_point = 50;
            hp = 250;
            attack_interval = 2;
            if (level >= 6) {
                is_evolved = 1;
            }
            for (int i = 1; i < level; i++) {
                hp += 30;
                damage_point = damage_point + 25;
                defense_point = defense_point + 7;
                attack_interval = attack_interval - 0.06;
            }
            fight_damage_point = damage_point;
            fight_defense_point = defense_point;
            fight_attack_interval = attack_interval;
            fight_hp = hp;
        } else if (rand_num1 == BLACK_BEAR_BERSERK) {
            pet_name = "黑熊狂战";
            pet_attribute = 1;
            damage_point = 40;
            defense_point = 60;
            hp = 350;
            attack_interval = 2;
            if (level >= 6) {
                is_evolved = 1;
            }
            for (int i = 1; i < level; i++) {
                hp += 75;
                damage_point = damage_point + 8;
                defense_point = defense_point + 10;
                attack_interval = attack_interval - 0.03;
            }
            fight_damage_point = damage_point;
            fight_defense_point = defense_point;
            fight_attack_interval = attack_interval;
            fight_hp = hp;
        } else if (rand_num1 == ARES) {
            pet_name = "阿瑞斯";
            pet_attribute = 1;
            damage_point = 40;
            defense_point = 60;
            hp = 350;
            attack_interval = 2;
            if (level >= 6) {
                is_evolved = 1;
            }
            for (int i = 1; i < level; i++) {
                hp += 75;
                damage_point = damage_point + 8;
                defense_point = defense_point + 10;
                attack_interval = attack_interval - 0.03;
            }
            fight_damage_point = damage_point;
            fight_defense_point = defense_point;
            fight_attack_interval = attack_interval;
            fight_hp = hp;
        } else if (rand_num1 == ROCK_ARMOR_LORD) {
            pet_name = "岩铠领主";
            pet_attribute = 2;
            damage_point = 40;
            defense_point = 70;
            hp = 300;
            attack_interval = 2;
            if (level >= 6) {
                is_evolved = 1;
            }
            for (int i = 1; i < level; i++) {
                hp += 60;
                damage_point = damage_point + 7;
                defense_point = defense_point + 15;
                attack_interval = attack_interval - 0.03;
            }
            fight_damage_point = damage_point;
            fight_defense_point = defense_point;
            fight_attack_interval = attack_interval;
            fight_hp = hp;
        } else if (rand_num1 == AMBER_KNIGHT) {
            pet_name = "琥珀骑士";
            pet_attribute = 2;
            damage_point = 40;
            defense_point = 70;
            hp = 300;
            attack_interval = 2;
            if (level >= 6) {
                is_evolved = 1;
            }
            for (int i = 1; i < level; i++) {
                hp += 60;
                damage_point = damage_point + 7;
                defense_point = defense_point + 15;
                attack_interval = attack_interval - 0.03;
            }
            fight_damage_point = damage_point;
            fight_defense_point = defense_point;
            fight_attack_interval = attack_interval;
            fight_hp = hp;
        } else if (rand_num1 == SWIFT_QUINN) {
            pet_name = "疾驰奎因";
            pet_attribute = 3;
            damage_point = 45;
            defense_point = 40;
            hp = 250;
            attack_interval = 1.5;
            if (level >= 6) {
                is_evolved = 1;
            }
            for (int i = 1; i < level; i++) {
                hp += 30;
                damage_point = damage_point + 15;
                defense_point = defense_point + 5;
                attack_interval = attack_interval - 0.05;
            }
            fight_damage_point = damage_point;
            fight_defense_point = defense_point;
            fight_attack_interval = attack_interval;
            fight_hp = hp;
        } else if (rand_num1 == DIMO) {
            pet_name = "迪莫";
            pet_attribute = 3;
            damage_point = 45;
            defense_point = 40;
            hp = 250;
            attack_interval = 1.5;
            if (level >= 6) {
                is_evolved = 1;
            }
            for (int i = 1; i < level; i++) {
                hp += 30;
                damage_point = damage_point + 15;
                defense_point = defense_point + 5;
                attack_interval = attack_interval - 0.05;
            }
            fight_damage_point = damage_point;
            fight_defense_point = defense_point;
            fight_attack_interval = attack_interval;
            fight_hp = hp;
        }

        qDebug() << "插入宠物到数据库" << endl;
        // 构建 SQL 插入语句
        QString sql
            = QString(
                  "INSERT INTO server_pets "
                  "(pet_name, pet_attribute, user_id, level, is_evolved, exp, damage_point, "
                  "defense_point, hp, attack_interval, fight_damage_point, fight_defense_point, "
                  "fight_hp, fight_attack_interval) "
                  "VALUES ('%1', '%2', %3, %4, %5, %6, %7, %8, %9, %10, %11, %12, %13, %14);")
                  .arg(pet_name)
                  .arg(pet_attribute)
                  .arg(user_id)
                  .arg(level)
                  .arg(is_evolved) // 转换布尔值为整数（0 或 1）
                  .arg(exp)
                  .arg(damage_point)
                  .arg(defense_point)
                  .arg(hp)
                  .arg(attack_interval)
                  .arg(fight_damage_point)
                  .arg(fight_defense_point)
                  .arg(fight_hp)
                  .arg(fight_attack_interval);

        // 执行 SQL 语句
        if (!query.exec(sql)) {
            qDebug() << "添加宠物信息失败:" << query.lastError().text();
            return;
        }

        qDebug() << "宠物信息添加成功";
    }
}

void MainWindow::Start_upgrade_battle(QJsonObject json)
{
    // 打开数据库连接
    mo.open_database();
    if (!mo.is_open) {
        qDebug() << "数据库连接失败，无法进行查询操作";
        return;
    }
    QSqlQuery query(mo.db); // 指定数据库连接

    int _socket_id = json["socket_id"].toInt();
    int user_pet_id = json["user_pet_id"].toInt();
    int server_pet_id = json["server_pet_id"].toInt();

    QJsonObject user_pet_object;
    QJsonObject server_pet_object;

    // 查询用户宠物信息
    QString userPetQuery = QString("SELECT * FROM pets WHERE pet_id = %1").arg(user_pet_id);
    if (query.exec(userPetQuery) && query.next()) {
        user_pet_object["pet_id"] = query.value("pet_id").toInt();
        user_pet_object["pet_name"] = query.value("pet_name").toString();
        user_pet_object["pet_attribute"] = query.value("pet_attribute").toInt();
        user_pet_object["level"] = query.value("level").toInt();
        user_pet_object["exp"] = query.value("exp").toInt();
        user_pet_object["damage_point"] = query.value("damage_point").toDouble();
        user_pet_object["defense_point"] = query.value("defense_point").toDouble();
        user_pet_object["hp"] = query.value("hp").toDouble();
        user_pet_object["attack_interval"] = query.value("attack_interval").toDouble();
        user_pet_object["is_evolved"] = query.value("is_evolved").toInt();

    } else {
        qDebug() << "查询用户宠物信息失败:" << query.lastError().text();
        return;
    }

    // 查询服务器宠物信息
    QString serverPetQuery = QString("SELECT * FROM server_pets WHERE pet_id = %1")
                                 .arg(server_pet_id);
    if (query.exec(serverPetQuery) && query.next()) {
        server_pet_object["pet_id"] = query.value("pet_id").toInt();
        server_pet_object["pet_name"] = query.value("pet_name").toString();
        server_pet_object["pet_attribute"] = query.value("pet_attribute").toInt();
        server_pet_object["level"] = query.value("level").toInt();
        server_pet_object["exp"] = query.value("exp").toInt();
        server_pet_object["damage_point"] = query.value("damage_point").toDouble();
        server_pet_object["defense_point"] = query.value("defense_point").toDouble();
        server_pet_object["hp"] = query.value("hp").toDouble();
        server_pet_object["attack_interval"] = query.value("attack_interval").toDouble();
        server_pet_object["is_evolved"] = query.value("is_evolved").toInt();
    } else {
        qDebug() << "查询服务器宠物信息失败:" << query.lastError().text();
        return;
    }

    QJsonObject response = {{"define", START_UPGRADE_BATTLE_SUCCESSED},
                            {"socket_id", _socket_id},
                            {"user_pet_object", user_pet_object},
                            {"server_pet_object", server_pet_object}};
    send_message(response);
}

void MainWindow::Start_vs_battle(QJsonObject json)
{
    // 打开数据库连接
    mo.open_database();
    if (!mo.is_open) {
        qDebug() << "数据库连接失败，无法进行查询操作";
        return;
    }
    QSqlQuery query(mo.db); // 指定数据库连接

    int _socket_id = json["socket_id"].toInt();
    int user_pet_id = json["user_pet_id"].toInt();
    int server_pet_id = json["server_pet_id"].toInt();

    QJsonObject user_pet_object;
    QJsonObject server_pet_object;

    // 查询用户宠物信息
    QString userPetQuery = QString("SELECT * FROM pets WHERE pet_id = %1").arg(user_pet_id);
    if (query.exec(userPetQuery) && query.next()) {
        user_pet_object["pet_id"] = query.value("pet_id").toInt();
        user_pet_object["pet_name"] = query.value("pet_name").toString();
        user_pet_object["pet_attribute"] = query.value("pet_attribute").toInt();
        user_pet_object["level"] = query.value("level").toInt();
        user_pet_object["exp"] = query.value("exp").toInt();
        user_pet_object["damage_point"] = query.value("damage_point").toDouble();
        user_pet_object["defense_point"] = query.value("defense_point").toDouble();
        user_pet_object["hp"] = query.value("hp").toDouble();
        user_pet_object["attack_interval"] = query.value("attack_interval").toDouble();
        user_pet_object["is_evolved"] = query.value("is_evolved").toInt();

    } else {
        qDebug() << "查询用户宠物信息失败:" << query.lastError().text();
        return;
    }

    // 查询服务器宠物信息
    QString serverPetQuery = QString("SELECT * FROM server_pets WHERE pet_id = %1")
                                 .arg(server_pet_id);
    if (query.exec(serverPetQuery) && query.next()) {
        server_pet_object["pet_id"] = query.value("pet_id").toInt();
        server_pet_object["pet_name"] = query.value("pet_name").toString();
        server_pet_object["pet_attribute"] = query.value("pet_attribute").toInt();
        server_pet_object["level"] = query.value("level").toInt();
        server_pet_object["exp"] = query.value("exp").toInt();
        server_pet_object["damage_point"] = query.value("damage_point").toDouble();
        server_pet_object["defense_point"] = query.value("defense_point").toDouble();
        server_pet_object["hp"] = query.value("hp").toDouble();
        server_pet_object["attack_interval"] = query.value("attack_interval").toDouble();
        server_pet_object["is_evolved"] = query.value("is_evolved").toInt();
    } else {
        qDebug() << "查询服务器宠物信息失败:" << query.lastError().text();
        return;
    }

    QJsonObject response = {{"define", START_VS_BATTLE_SUCCESSED},
                            {"socket_id", _socket_id},
                            {"user_pet_object", user_pet_object},
                            {"server_pet_object", server_pet_object}};
    send_message(response);
}

void MainWindow::Upgrade_win(QJsonObject json)
{
    // 打开数据库连接
    mo.open_database();
    if (!mo.is_open) {
        qDebug() << "数据库连接失败，无法进行查询操作";
        return;
    }
    QSqlQuery query(mo.db); // 指定数据库连接

    int _socket_id = json["socket_id"].toInt();
    int pet_id = json["pet_id"].toInt();
    int new_exp = json["exp"].toInt();

    QString sql = QString("SELECT * FROM pets WHERE  pet_id =%1").arg(pet_id);

    if (!query.exec(sql)) {
        qDebug() << "查询宠物经验和等级时出错:" << query.lastError();
        return;
    }

    if (!query.next()) {
        qDebug() << "未找到指定的宠物记录";
        return;
    }

    int current_exp = query.value("exp").toInt();
    int current_level = query.value("level").toInt();
    int total_exp = current_exp + new_exp;
    int attribute = query.value("pet_attribute").toInt();
    int is_evolved = query.value("is_evolved").toInt();
    double hp = query.value("hp").toDouble();
    double damage_point = query.value("damage_point").toDouble();
    double defense_point = query.value("defense_point").toDouble();
    double attack_inteval = query.value("attack_interval").toDouble();

    // 假设每次升级所需的经验
    int exp_needed_for_upgrade = 1; // 你可以替换为你自己的升级所需经验的逻辑
    for (int i = 0; i < current_level; i++) {
        exp_needed_for_upgrade *= 2;
    }
    exp_needed_for_upgrade = exp_needed_for_upgrade * 2;

    int left_exp = total_exp - exp_needed_for_upgrade;
    if (left_exp >= 0 && current_level < 15) {
        current_level += 1;
        if (current_level >= 6) {
            is_evolved = 1;
        }
        if (attribute == 0) {
            damage_point += 25;
            hp += 30;
            defense_point += 7;
            attack_inteval -= 0.06;
            sql = QString(
                      "UPDATE pets SET exp =%1,is_evolved =%2, hp=%3,fight_hp "
                      "=%4,damage_point=%5,fight_damage_point=%6,defense_point=%7,fight_defense_"
                      "point=%8,attack_interval=%9,fight_attack_interval=%10,level=%11 WHERE "
                      "pet_id =%12")
                      .arg(left_exp)
                      .arg(is_evolved)
                      .arg(hp)
                      .arg(hp)
                      .arg(damage_point)
                      .arg(damage_point)
                      .arg(defense_point)
                      .arg(defense_point)
                      .arg(attack_inteval)
                      .arg(attack_inteval)
                      .arg(current_level)
                      .arg(pet_id);
        } else if (attribute == 1) {
            damage_point += 8;
            hp += 75;
            defense_point += 10;
            attack_inteval -= 0.03;
            sql = QString(
                      "UPDATE pets SET exp =%1,is_evolved =%2, hp=%3,fight_hp "
                      "=%4,damage_point=%5,fight_damage_point=%6,defense_point=%7,fight_defense_"
                      "point=%8,attack_interval=%9,fight_attack_interval=%10,level=%11 WHERE "
                      "pet_id =%12")
                      .arg(left_exp)
                      .arg(is_evolved)
                      .arg(hp)
                      .arg(hp)
                      .arg(damage_point)
                      .arg(damage_point)
                      .arg(defense_point)
                      .arg(defense_point)
                      .arg(attack_inteval)
                      .arg(attack_inteval)
                      .arg(current_level)
                      .arg(pet_id);
        } else if (attribute == 2) {
            damage_point += 7;
            hp += 60;
            defense_point += 15;
            attack_inteval -= 0.03;
            sql = QString(
                      "UPDATE pets SET exp =%1,is_evolved =%2, hp=%3,fight_hp "
                      "=%4,damage_point=%5,fight_damage_point=%6,defense_point=%7,fight_defense_"
                      "point=%8,attack_interval=%9,fight_attack_interval=%10,level=%11 WHERE "
                      "pet_id =%12")
                      .arg(left_exp)
                      .arg(is_evolved)
                      .arg(hp)
                      .arg(hp)
                      .arg(damage_point)
                      .arg(damage_point)
                      .arg(defense_point)
                      .arg(defense_point)
                      .arg(attack_inteval)
                      .arg(attack_inteval)
                      .arg(current_level)
                      .arg(pet_id);
        } else if (attribute == 3) {
            damage_point += 15;
            hp += 30;
            defense_point += 5;
            attack_inteval -= 0.05;
            sql = QString(
                      "UPDATE pets SET exp =%1,is_evolved =%2, hp=%3,fight_hp "
                      "=%4,damage_point=%5,fight_damage_point=%6,defense_point=%7,fight_defense_"
                      "point=%8,attack_interval=%9,fight_attack_interval=%10,level=%11 WHERE "
                      "pet_id =%12")
                      .arg(left_exp)
                      .arg(is_evolved)
                      .arg(hp)
                      .arg(hp)
                      .arg(damage_point)
                      .arg(damage_point)
                      .arg(defense_point)
                      .arg(defense_point)
                      .arg(attack_inteval)
                      .arg(attack_inteval)
                      .arg(current_level)
                      .arg(pet_id);
        }
    } else {
        sql = QString("UPDATE pets SET exp =%1 WHERE pet_id =%2").arg(total_exp).arg(pet_id);
    }

    if (!query.exec(sql)) {
        qDebug() << "更新宠物经验和等级时出错:" << query.lastError();
    } else {
        qDebug() << "宠物经验和等级更新成功";
    }

    int user_id = json["user_id"].toInt();
    // 查询当前 matches_played 和 matches_won 值
    qDebug() << "查询当前 matches_played 和 matches_won 值" << endl;
    QString selectSql = QString("SELECT matches_played,matches_won FROM users WHERE user_id = %1")
                            .arg(user_id);
    if (query.exec(selectSql)) {
        if (query.next()) {
            int matches_played = query.value("matches_played").toInt();
            int matches_won = query.value("matches_won").toInt();
            double win_rate;

            // 更新 matches_played 和 matches_won 列
            matches_played++;
            matches_won++; // 或者根据实际情况更新 matches_won
            if (matches_won == 0 || matches_played == 0) {
                win_rate = 0;
            } else {
                win_rate = static_cast<double>(matches_won) / matches_played;
            }
            QString updateSql = QString("UPDATE users SET matches_played = %1, matches_won = "
                                        "%2,win_rate=%3 WHERE user_id = %4")
                                    .arg(matches_played)
                                    .arg(matches_won)
                                    .arg(win_rate)
                                    .arg(user_id);

            if (query.exec(updateSql)) {
                qDebug() << "成功更新用户的 matches_played 和 matches_won 值";
            } else {
                qDebug() << "更新用户的 matches_played 和 matches_won 值失败:" << query.lastError();
            }

            QString updateSql_1 = QString("UPDATE users SET win_rate=%1 WHERE user_id = %2")
                                      .arg(win_rate)
                                      .arg(user_id);
            if (query.exec(updateSql_1)) {
                qDebug() << "成功更新用户的win_rate 值";
            } else {
                qDebug() << "更新用户的 win_rate 值失败:" << query.lastError();
            }
        } else {
            qDebug() << "未找到用户的信息";
        }
    } else {
        qDebug() << "查询用户信息失败:" << query.lastError();
    }
}

void MainWindow::Vs_win(QJsonObject json)
{
    // 打开数据库连接
    mo.open_database();
    if (!mo.is_open) {
        qDebug() << "数据库连接失败，无法进行查询操作";
        return;
    }
    QSqlQuery query(mo.db); // 指定数据库连接

    int _socket_id = json["socket_id"].toInt();
    int user_pet_id = json["user_pet_id"].toInt();
    int new_exp = json["exp"].toInt();
    int server_pet_id = json["server_pet_id"].toInt();
    int user_id = json["user_id"].toInt();

    QString sql = QString("SELECT * FROM pets WHERE  pet_id =%1").arg(user_pet_id);

    if (!query.exec(sql)) {
        qDebug() << "查询宠物经验和等级时出错:" << query.lastError();
        return;
    }

    if (!query.next()) {
        qDebug() << "未找到指定的宠物记录";
        return;
    }

    int current_exp = query.value("exp").toInt();
    int current_level = query.value("level").toInt();
    int total_exp = current_exp + new_exp;
    int attribute = query.value("pet_attribute").toInt();
    int is_evolved = query.value("is_evolved").toInt();
    double hp = query.value("hp").toDouble();
    double damage_point = query.value("damage_point").toDouble();
    double defense_point = query.value("defense_point").toDouble();
    double attack_inteval = query.value("attack_interval").toDouble();

    // 假设每次升级所需的经验
    int exp_needed_for_upgrade = 1; // 你可以替换为你自己的升级所需经验的逻辑
    for (int i = 0; i < current_level; i++) {
        exp_needed_for_upgrade *= 2;
    }
    exp_needed_for_upgrade = exp_needed_for_upgrade * 2;

    int left_exp = total_exp - exp_needed_for_upgrade;
    if (left_exp >= 0 && current_level < 15) {
        current_level += 1;
        if (current_level >= 6) {
            is_evolved = 1;
        }
        if (attribute == 0) {
            damage_point += 25;
            hp += 30;
            defense_point += 7;
            attack_inteval -= 0.06;
            sql = QString(
                      "UPDATE pets SET exp =%1,is_evolved =%2, hp=%3,fight_hp "
                      "=%4,damage_point=%5,fight_damage_point=%6,defense_point=%7,fight_defense_"
                      "point=%8,attack_interval=%9,fight_attack_interval=%10,level=%11 WHERE "
                      "pet_id =%12")
                      .arg(left_exp)
                      .arg(is_evolved)
                      .arg(hp)
                      .arg(hp)
                      .arg(damage_point)
                      .arg(damage_point)
                      .arg(defense_point)
                      .arg(defense_point)
                      .arg(attack_inteval)
                      .arg(attack_inteval)
                      .arg(current_level)
                      .arg(user_pet_id);
        } else if (attribute == 1) {
            damage_point += 8;
            hp += 75;
            defense_point += 10;
            attack_inteval -= 0.03;
            sql = QString(
                      "UPDATE pets SET exp =%1,is_evolved =%2, hp=%3,fight_hp "
                      "=%4,damage_point=%5,fight_damage_point=%6,defense_point=%7,fight_defense_"
                      "point=%8,attack_interval=%9,fight_attack_interval=%10,level=%11 WHERE "
                      "pet_id =%12")
                      .arg(left_exp)
                      .arg(is_evolved)
                      .arg(hp)
                      .arg(hp)
                      .arg(damage_point)
                      .arg(damage_point)
                      .arg(defense_point)
                      .arg(defense_point)
                      .arg(attack_inteval)
                      .arg(attack_inteval)
                      .arg(current_level)
                      .arg(user_pet_id);
        } else if (attribute == 2) {
            damage_point += 7;
            hp += 60;
            defense_point += 15;
            attack_inteval -= 0.03;
            sql = QString(
                      "UPDATE pets SET exp =%1,is_evolved =%2, hp=%3,fight_hp "
                      "=%4,damage_point=%5,fight_damage_point=%6,defense_point=%7,fight_defense_"
                      "point=%8,attack_interval=%9,fight_attack_interval=%10,level=%11 WHERE "
                      "pet_id =%12")
                      .arg(left_exp)
                      .arg(is_evolved)
                      .arg(hp)
                      .arg(hp)
                      .arg(damage_point)
                      .arg(damage_point)
                      .arg(defense_point)
                      .arg(defense_point)
                      .arg(attack_inteval)
                      .arg(attack_inteval)
                      .arg(current_level)
                      .arg(user_pet_id);
        } else if (attribute == 3) {
            damage_point += 15;
            hp += 30;
            defense_point += 5;
            attack_inteval -= 0.05;
            sql = QString(
                      "UPDATE pets SET exp =%1,is_evolved =%2, hp=%3,fight_hp "
                      "=%4,damage_point=%5,fight_damage_point=%6,defense_point=%7,fight_defense_"
                      "point=%8,attack_interval=%9,fight_attack_interval=%10,level=%11 WHERE "
                      "pet_id =%12")
                      .arg(left_exp)
                      .arg(is_evolved)
                      .arg(hp)
                      .arg(hp)
                      .arg(damage_point)
                      .arg(damage_point)
                      .arg(defense_point)
                      .arg(defense_point)
                      .arg(attack_inteval)
                      .arg(attack_inteval)
                      .arg(current_level)
                      .arg(user_pet_id);
        }
    } else {
        sql = QString("UPDATE pets SET exp =%1 WHERE pet_id =%2").arg(total_exp).arg(user_pet_id);
    }

    if (!query.exec(sql)) {
        qDebug() << "更新宠物经验和等级时出错:" << query.lastError();
    } else {
        qDebug() << "宠物经验和等级更新成功";
    }

    qDebug() << "服务器宠物归自己" << endl;

    transfer_pet_to_user(server_pet_id, user_id);

    // 查询当前 matches_played 和 matches_won 值
    QString selectSql = QString("SELECT matches_played, matches_won FROM users WHERE user_id = %1")
                            .arg(user_id);
    if (query.exec(selectSql)) {
        if (query.next()) {
            int matches_played = query.value("matches_played").toInt();
            int matches_won = query.value("matches_won").toInt();
            double win_rate;

            // 更新 matches_played 和 matches_won 列
            matches_played++;
            matches_won++; // 或者根据实际情况更新 matches_won
            if (matches_won == 0 || matches_played == 0) {
                win_rate = 0;
            } else {
                win_rate = static_cast<double>(matches_won) / matches_played;
            }
            QString updateSql = QString("UPDATE users SET matches_played = %1,matches_won = "
                                        "%2,win_rate=%3 WHERE user_id = %4")
                                    .arg(matches_played)
                                    .arg(matches_won)
                                    .arg(win_rate)
                                    .arg(user_id);

            if (query.exec(updateSql)) {
                qDebug() << "成功更新用户的 matches_played 和 matches_won 值";
            } else {
                qDebug() << "更新用户的 matches_played 和 matches_won 值失败:" << query.lastError();
            }
        } else {
            qDebug() << "未找到用户的信息";
        }
    } else {
        qDebug() << "查询用户信息失败:" << query.lastError();
    }
}

void MainWindow::transfer_pet_to_user(int server_pet_id, int user_id)
{
    // 打开数据库连接
    mo.open_database();
    if (!mo.is_open) {
        qDebug() << "数据库连接失败，无法进行查询操作";
        return;
    }
    QSqlQuery query(mo.db); // 指定数据库连接

    // 1. 获取服务器宠物的所有信息
    QString selectSql = QString("SELECT * FROM server_pets WHERE pet_id = %1").arg(server_pet_id);
    if (!query.exec(selectSql)) {
        qDebug() << "查询服务器宠物信息时出错:" << query.lastError();
        return;
    }

    if (!query.next()) {
        qDebug() << "未找到指定的服务器宠物记录";
        return;
    }

    // 提取宠物信息
    int pet_id = query.value("pet_id").toInt();
    QString pet_name = query.value("pet_name").toString();
    int pet_level = query.value("level").toInt();
    int pet_exp = query.value("exp").toInt();
    int pet_attribute = query.value("pet_attribute").toInt();
    int is_evolved = query.value("is_evolved").toInt();
    double hp = query.value("hp").toDouble();
    double fight_hp = query.value("fight_hp").toDouble();
    double damage_point = query.value("damage_point").toDouble();
    double fight_damage_point = query.value("fight_damage_point").toDouble();
    double defense_point = query.value("defense_point").toDouble();
    double fight_defense_point = query.value("fight_defense_point").toDouble();
    double attack_interval = query.value("attack_interval").toDouble();
    double fight_attack_interval = query.value("fight_attack_interval").toDouble();

    // 生成新的唯一 pet_id
    int new_pet_id = generateUniquePetId();
    // 2. 插入到 `pets` 表中
    QString insertSql
        = QString("INSERT INTO pets ( pet_id,pet_name, level, exp, pet_attribute, is_evolved, hp, "
                  "fight_hp, "
                  "damage_point, fight_damage_point, defense_point, fight_defense_point, "
                  "attack_interval, fight_attack_interval, user_id) "
                  "VALUES (%1, '%2', %3, %4, %5, %6, %7, %8, %9, %10, %11, %12, %13, %14,%15)")
              .arg(new_pet_id)
              .arg(pet_name)
              .arg(pet_level)
              .arg(pet_exp)
              .arg(pet_attribute)
              .arg(is_evolved)
              .arg(hp)
              .arg(fight_hp)
              .arg(damage_point)
              .arg(fight_damage_point)
              .arg(defense_point)
              .arg(fight_defense_point)
              .arg(attack_interval)
              .arg(fight_attack_interval)
              .arg(user_id); // 这里指定新的 user_id

    if (!query.exec(insertSql)) {
        qDebug() << "插入宠物到 pets 表时出错:" << query.lastError();
        return;
    }

    qDebug() << "宠物成功插入到 pets 表";

    // 3. 从 `server_pets` 表中删除记录
    QString deleteSql = QString("DELETE FROM server_pets WHERE pet_id = %1").arg(server_pet_id);

    if (!query.exec(deleteSql)) {
        qDebug() << "从 server_pets 表中删除宠物时出错:" << query.lastError();
        return;
    }

    qDebug() << "宠物成功从 server_pets 表中删除";
}

void MainWindow::Vs_lose(QJsonObject json)
{
    // 打开数据库连接
    mo.open_database();
    if (!mo.is_open) {
        qDebug() << "数据库连接失败，无法进行查询操作";
        return;
    }
    QSqlQuery query(mo.db); // 指定数据库连接

    int _socket_id = json["socket_id"].toInt();
    int user_id = json["user_id"].toInt();

    // 从pets表中根据user_id随机选出三个宠物
    QString sql
        = QString("SELECT pet_id, pet_name FROM pets WHERE user_id = %1 ORDER BY RAND() LIMIT 3")
              .arg(user_id);

    if (!query.exec(sql)) {
        qDebug() << "查询用户宠物时出错:" << query.lastError();
        return;
    }

    QJsonArray pet_array;

    while (query.next()) {
        QJsonObject petObject;

        petObject["pet_id"] = query.value("pet_id").toInt();
        petObject["pet_name"] = query.value("pet_name").toString();
        pet_array.append(petObject);
    }

    // 检查是否没有足够的宠物
    if (pet_array.isEmpty()) {
        qDebug() << "用户没有宠物";
        add_pet();
        assign_pets_to_user(user_id);
        // 从pets表中根据user_id随机选出三个宠物
        QString sql
            = QString(
                  "SELECT pet_id, pet_name FROM pets WHERE user_id = %1 ORDER BY RAND() LIMIT 3")
                  .arg(user_id);

        if (!query.exec(sql)) {
            qDebug() << "查询用户宠物时出错:" << query.lastError();
            return;
        }

        while (query.next()) {
            QJsonObject petObject;

            petObject["pet_id"] = query.value("pet_id").toInt();
            petObject["pet_name"] = query.value("pet_name").toString();
            pet_array.append(petObject);
        }
    }

    // 构建返回给客户端的JSON对象
    QJsonObject responseJson;
    responseJson["define"] = SHOW_ABANDON;
    responseJson["socket_id"] = _socket_id;
    responseJson["pets_array"] = pet_array;
    send_message(responseJson);

    // 查询当前 matches_played 和 matches_won 值
    QString selectSql = QString("SELECT matches_played, matches_won FROM users WHERE user_id = %1")
                            .arg(user_id);
    if (query.exec(selectSql)) {
        if (query.next()) {
            int matches_played = query.value("matches_played").toInt();
            int matches_won = query.value("matches_won").toInt();
            double win_rate;
            // 更新 matches_played 和 matches_won 列
            matches_played++;
            if (matches_won == 0 || matches_played == 0) {
                win_rate = 0;
            } else {
                win_rate = static_cast<double>(matches_won) / matches_played;
            }

            QString updateSql
                = QString("UPDATE users SET matches_played = %1,win_rate =%2 WHERE user_id = %3")
                      .arg(matches_played)
                      .arg(win_rate)
                      .arg(user_id);

            if (query.exec(updateSql)) {
                qDebug() << "成功更新用户的 matches_played 和 matches_won 值";
            } else {
                qDebug() << "更新用户的 matches_played 和 matches_won 值失败:" << query.lastError();
            }
        } else {
            qDebug() << "未找到用户的信息";
        }
    } else {
        qDebug() << "查询用户信息失败:" << query.lastError();
    }
}

void MainWindow::Abandon_pet(QJsonObject json)
{
    int _socket_id = json["socket_id"].toInt();
    int pet_id = json["pet_id"].toInt();

    // 打开数据库连接
    mo.open_database();
    if (!mo.is_open) {
        qDebug() << "数据库连接失败，无法进行查询操作";
        return;
    }

    QSqlQuery query(mo.db); // 指定数据库连接

    // 1. 检查宠物是否存在于pets表中
    QString checkPetSql = QString("SELECT * FROM pets WHERE pet_id = %1").arg(pet_id);
    if (!query.exec(checkPetSql)) {
        qDebug() << "查询宠物时出错:" << query.lastError();
        return;
    }

    if (!query.next()) {
        qDebug() << "未找到指定的宠物记录，无法进行移除操作";
        return;
    }

    // 提取宠物的所有信息
    int user_id = query.value("user_id").toInt();
    int pet_attribute = query.value("pet_attribute").toInt();
    double hp = query.value("hp").toDouble();
    double damage_point = query.value("damage_point").toDouble();
    double defense_point = query.value("defense_point").toDouble();
    double attack_interval = query.value("attack_interval").toDouble();
    int is_evolved = query.value("is_evolved").toInt();
    int level = query.value("level").toInt();
    int exp = query.value("exp").toInt();
    QString pet_name = query.value("pet_name").toString();

    // 开启事务
    mo.db.transaction();

    // 2. 将宠物信息插入到server_pets表中
    QString insertPetSql
        = QString("INSERT INTO server_pets (pet_id, user_id, pet_attribute, hp, "
                  "damage_point, defense_point, attack_interval, "
                  "is_evolved, level, exp, "
                  "pet_name,fight_damage_point,fight_defense_point,fight_hp,fight_attack_interval) "
                  "VALUES (%1, %2, %3, %4, %5, %6, %7, %8, %9, %10,'%11',%12,%13,%14,%15)")
              .arg(pet_id)
              .arg(0)
              .arg(pet_attribute)
              .arg(hp)
              .arg(damage_point)
              .arg(defense_point)
              .arg(attack_interval)
              .arg(is_evolved)
              .arg(level)
              .arg(exp)
              .arg(pet_name)
              .arg(damage_point)
              .arg(defense_point)
              .arg(hp)
              .arg(attack_interval);

    if (!query.exec(insertPetSql)) {
        qDebug() << "将宠物插入server_pets表时出错:" << query.lastError();
        mo.db.rollback(); // 发生错误时回滚事务
        return;
    }

    // 3. 从pets表中移除指定的宠物
    QString deletePetSql = QString("DELETE FROM pets WHERE pet_id = %1").arg(pet_id);

    if (!query.exec(deletePetSql)) {
        qDebug() << "从pets表移除宠物时出错:" << query.lastError();
        mo.db.rollback(); // 发生错误时回滚事务
        return;
    }

    // 提交事务
    if (!mo.db.commit()) {
        qDebug() << "提交事务时出错:" << mo.db.lastError();
    } else {
        qDebug() << "宠物成功移至server_pets表并从pets表中删除";
    }
}
// 生成唯一的 `pet_id` 函数（示例实现）
int MainWindow::generateUniquePetId()
{
    QSqlQuery query(mo.db);

    int new_pet_id = 0;
    bool unique = false;

    while (!unique) {
        // 生成随机的 `pet_id`
        new_pet_id = qrand() % 100000; // 示例: 生成 0 到 99999 之间的随机数

        // 检查 `pets` 表和 `server_pets` 表中是否存在相同的 `pet_id`
        QString checkSql = QString("SELECT COUNT(*) FROM pets WHERE pet_id = :pet_id");
        query.prepare(checkSql);
        query.bindValue(":pet_id", new_pet_id);

        if (!query.exec()) {
            qDebug() << "检查 pets 表时出错:" << query.lastError();
            continue; // 发生错误时，继续生成新的 ID
        }

        query.next();
        int count = query.value(0).toInt();

        if (count == 0) {
            // 检查 `server_pets` 表
            checkSql = QString("SELECT COUNT(*) FROM server_pets WHERE pet_id = :pet_id");
            query.prepare(checkSql);
            query.bindValue(":pet_id", new_pet_id);

            if (!query.exec()) {
                qDebug() << "检查 server_pets 表时出错:" << query.lastError();
                continue; // 发生错误时，继续生成新的 ID
            }

            query.next();
            count = query.value(0).toInt();

            if (count == 0) {
                unique = true;
            }
        }
    }

    return new_pet_id;
}

void MainWindow::send_socket_id()
{
    QJsonObject json = {{"define", SOCKET_1}, {"socket_id", socket_id}};
    send_message(json);
    socket_id++;
}

void MainWindow::Upgrade_lose(QJsonObject json)
{
    int user_id = json["user_id"].toInt();

    // 查询当前 matches_played 和 matches_won 值
    QString selectSql = QString("SELECT matches_played, matches_won FROM users WHERE user_id = %1")
                            .arg(user_id);
    QSqlQuery query;
    if (query.exec(selectSql)) {
        if (query.next()) {
            int matches_played = query.value("matches_played").toInt();
            int matches_won = query.value("matches_won").toInt();
            double win_rate;
            // 更新 matches_played 和 matches_won 列
            matches_played++;

            if (matches_won == 0 || matches_played == 0) {
                win_rate = 0;
            } else {
                win_rate = static_cast<double>(matches_won) / matches_played;
            }
            QString updateSql
                = QString("UPDATE users SET matches_played = %1,win_rate=%2 WHERE user_id = %3")
                      .arg(matches_played)
                      .arg(win_rate)
                      .arg(user_id);

            if (query.exec(updateSql)) {
                qDebug() << "成功更新用户的 matches_played 和 matches_won 值";
            } else {
                qDebug() << "更新用户的 matches_played 和 matches_won 值失败:" << query.lastError();
            }
        } else {
            qDebug() << "未找到用户的信息";
        }
    } else {
        qDebug() << "查询用户信息失败:" << query.lastError();
    }
}

void MainWindow::Upgrade(QJsonObject json)
{
    int _socket_id = json["socket_id"].toInt();
    int pet_id = json["pet_id"].toInt();
    int user_id = json["user_id"].toInt();

    QSqlQuery query;
    QString sql = QString("SELECT * FROM pets WHERE  pet_id =%1").arg(pet_id);

    if (!query.exec(sql)) {
        qDebug() << "查询宠物经验和等级时出错:" << query.lastError();
        return;
    }

    if (!query.next()) {
        qDebug() << "未找到指定的宠物记录";
        return;
    }

    int current_level = query.value("level").toInt();
    int attribute = query.value("pet_attribute").toInt();
    int is_evolved = query.value("is_evolved").toInt();
    double hp = query.value("hp").toDouble();
    double damage_point = query.value("damage_point").toDouble();
    double defense_point = query.value("defense_point").toDouble();
    double attack_inteval = query.value("attack_interval").toDouble();

    current_level = current_level + 1;

    if (current_level >= 6) {
        is_evolved = 1;
    }
    if (attribute == 0) {
        damage_point += 25;
        hp += 30;
        defense_point += 7;
        attack_inteval -= 0.06;
        sql = QString("UPDATE pets SET is_evolved =%1, hp=%2,fight_hp "
                      "=%3,damage_point=%4,fight_damage_point=%5,defense_point=%6,fight_defense_"
                      "point=%7,attack_interval=%8,fight_attack_interval=%9,level=%10 WHERE "
                      "pet_id =%11")

                  .arg(is_evolved)
                  .arg(hp)
                  .arg(hp)
                  .arg(damage_point)
                  .arg(damage_point)
                  .arg(defense_point)
                  .arg(defense_point)
                  .arg(attack_inteval)
                  .arg(attack_inteval)
                  .arg(current_level)
                  .arg(pet_id);
        if (!query.exec(sql)) {
            qDebug() << "给宠物升级出错:" << query.lastError();
            return;
        }
    } else if (attribute == 1) {
        damage_point += 8;
        hp += 75;
        defense_point += 10;
        attack_inteval -= 0.03;
        sql = QString("UPDATE pets SET is_evolved =%1, hp=%2,fight_hp "
                      "=%3,damage_point=%4,fight_damage_point=%5,defense_point=%6,fight_defense_"
                      "point=%7,attack_interval=%8,fight_attack_interval=%9,level=%10 WHERE "
                      "pet_id =%11")

                  .arg(is_evolved)
                  .arg(hp)
                  .arg(hp)
                  .arg(damage_point)
                  .arg(damage_point)
                  .arg(defense_point)
                  .arg(defense_point)
                  .arg(attack_inteval)
                  .arg(attack_inteval)
                  .arg(current_level)
                  .arg(pet_id);
        if (!query.exec(sql)) {
            qDebug() << "给宠物升级出错:" << query.lastError();
            return;
        }
    } else if (attribute == 2) {
        damage_point += 7;
        hp += 60;
        defense_point += 15;
        attack_inteval -= 0.03;
        sql = QString("UPDATE pets SET is_evolved =%1, hp=%2,fight_hp "
                      "=%3,damage_point=%4,fight_damage_point=%5,defense_point=%6,fight_defense_"
                      "point=%7,attack_interval=%8,fight_attack_interval=%9,level=%10 WHERE "
                      "pet_id =%11")

                  .arg(is_evolved)
                  .arg(hp)
                  .arg(hp)
                  .arg(damage_point)
                  .arg(damage_point)
                  .arg(defense_point)
                  .arg(defense_point)
                  .arg(attack_inteval)
                  .arg(attack_inteval)
                  .arg(current_level)
                  .arg(pet_id);
        if (!query.exec(sql)) {
            qDebug() << "给宠物升级出错:" << query.lastError();
            return;
        }
    } else if (attribute == 3) {
        damage_point += 15;
        hp += 30;
        defense_point += 5;
        attack_inteval -= 0.05;
        sql = QString("UPDATE pets SET is_evolved =%1, hp=%2,fight_hp "
                      "=%3,damage_point=%4,fight_damage_point=%5,defense_point=%6,fight_defense_"
                      "point=%7,attack_interval=%8,fight_attack_interval=%9,level=%10 WHERE "
                      "pet_id =%11")

                  .arg(is_evolved)
                  .arg(hp)
                  .arg(hp)
                  .arg(damage_point)
                  .arg(damage_point)
                  .arg(defense_point)
                  .arg(defense_point)
                  .arg(attack_inteval)
                  .arg(attack_inteval)
                  .arg(current_level)
                  .arg(pet_id);
        if (!query.exec(sql)) {
            qDebug() << "给宠物升级出错:" << query.lastError();
            return;
        }
    }

    // Step 1: Get the current money from the users table
    QString selectSql = QString("SELECT money FROM users WHERE user_id = %1").arg(user_id);
    if (!query.exec(selectSql)) {
        qDebug() << "查询用户金钱信息时出错:" << query.lastError();
        return;
    }

    if (!query.next()) {
        qDebug() << "未找到指定的用户";
        return;
    }

    int current_money = query.value("money").toInt();

    // Step 2: Deduct 250 from the current money
    int new_money = current_money - 250;

    // Step 3: Update the new money back to the database
    QString updateSql
        = QString("UPDATE users SET money = %1 WHERE user_id = %2").arg(new_money).arg(user_id);

    // 开始事务
    QSqlDatabase::database().transaction();

    if (!query.exec(updateSql)) {
        qDebug() << "更新用户金钱信息时出错:" << query.lastError();
        QSqlDatabase::database().rollback(); // 回滚事务
        return;
    }

    // 提交事务
    QSqlDatabase::database().commit();
    qDebug() << "用户金钱更新成功，已更新数据库";

    QJsonObject upgrade_successed_json = {{"define", UPGRADE_SUCCESSED},
                                          {"socket_id", _socket_id},
                                          {"money", new_money}};
    send_message(upgrade_successed_json);
}
