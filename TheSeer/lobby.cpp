#include "lobby.h"
#include "login.h"
#include "ui_lobby.h"

lobby::lobby(QTcpSocket *_tcp_socket, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::lobby)
    , user_pet_selected(false)
    , server_pet_selected(false)
{
    ui->setupUi(this);
    qDebug() << "创建大厅界面" << endl;
    tcp_socket = _tcp_socket;

    //配置场景
    setFixedSize(1440, 900);
    setWindowIcon(QIcon(":/resource/icon/takagi.jpg"));
    setWindowTitle("洛克传奇");

    QPixmap lobby_background_image;
    lobby_background_image.load(":/resource/background/lobby.jpg");
    //    ui->lobby_backgound->setPixmap(lobby_background_image);
    ui->lobby_backgound->setPixmap(lobby_background_image.scaled(this->size(),
                                                                 Qt::IgnoreAspectRatio,
                                                                 Qt::SmoothTransformation));

    lobby_page = ui->stackedWidget;
    lobby_page->setCurrentWidget(ui->lobby_2);

    QJsonObject json = {
        {"define", LOBBY},
        {"socket_id", mo.socket_id},
        {"username", mo.username},
    };

    tcp_socket->write(QString(QJsonDocument(json).toJson()).toUtf8().data());

    ui->username_lobby->setText(mo.username);

    // 连接用户宠物列表的选择变化信号
    connect(ui->user_pets_list,
            &QListWidget::itemSelectionChanged,
            this,
            &lobby::updateSelectionStatus);

    // 连接服务器宠物列表的选择变化信号
    connect(ui->server_pets_list,
            &QListWidget::itemSelectionChanged,
            this,
            &lobby::updateSelectionStatus);

    // 初始化按钮状态
    ui->start_upgrade_battle->setEnabled(false);
    ui->start_vs_battle->setEnabled(false);

    connect(tcp_socket, SIGNAL(readyRead()), this, SLOT(receive_from_server()));
}

lobby::~lobby()
{
    delete ui;
}

void lobby::receive_from_server()
{
    //从通信套接字中取出内容
    QByteArray array = tcp_socket->readAll();
    QJsonDocument jsonDocument = QJsonDocument::fromJson(array);
    QJsonObject json = jsonDocument.object();
    qDebug() << "lobby收到服务器：" << json << endl;
    if (json["socket_id"] != mo.socket_id) {
        return;
    }
    if (json["define"] == LOBBY_SUCCEED) {
        //获取用户信息
        money = json["money"].toInt();
        mo.money = money;
        num_pets = json["num_pets"].toInt();
        num_high_level_pets = json["num_high_level_pets"].toInt();
        qDebug() << money << num_pets << num_high_level_pets << endl;
        ui->money->setText(QString::number(money));
    } else if (json["define"] == RECHARGE_SUCCESSED) {
        int new_money = json["money"].toInt();
        ui->money->setText(QString::number(new_money));
    } else if (json["define"] == USERLIST_INFO) {
        qDebug() << "构建用户列表" << endl;
        QJsonArray userlist_info = json["userlist"].toArray();
        handle_userlist_info(userlist_info);
    } else if (json["define"] == PET_INFO) {
        qDebug() << "构建宠物图鉴" << endl;
        QJsonArray user_pets = json["user_pets"].toArray();
        mo.money = json["money"].toInt();
        handle_pet_info(user_pets);
    } else if (json["define"] == BATTLE_CHOOSE) {
        qDebug() << "战斗信息准备" << endl;
        QJsonArray user_pets = json["pets_array"].toArray();
        QJsonArray server_user_pets = json["server_pets_array"].toArray();
        handle_battle_choose(user_pets, server_user_pets);
    } else if (json["define"] == FLUSH_SERVER_SUCCESSED) {
        qDebug() << "刷新服务器宠物" << endl;
        QJsonArray server_user_pets = json["server_pets_array"].toArray();
        handle_flush_server_pets(server_user_pets);
    } else if (json["define"] == START_UPGRADE_BATTLE_SUCCESSED) {
        qDebug() << "开始初始化升级战" << endl;
        QJsonObject user_pet = json["user_pet_object"].toObject();
        QJsonObject server_pet = json["server_pet_object"].toObject();
        Upgrade_battle(user_pet, server_pet);
    } else if (json["define"] == START_VS_BATTLE_SUCCESSED) {
        qDebug() << "开始初始化决斗战" << endl;
        QJsonObject user_pet = json["user_pet_object"].toObject();
        QJsonObject server_pet = json["server_pet_object"].toObject();
        Vs_battle(user_pet, server_pet);
    } else if (json["define"] == SHOW_ABANDON) {
        qDebug() << "选中放弃的宠物" << endl;
        QJsonArray pet_array = json["pets_array"].toArray();
        Abandon_pet(pet_array);
    } else if (json["define"] == UPGRADE_SUCCESSED) {
        qDebug() << "宠物升级成功" << endl;
        int money = json["money"].toInt();
        ui->money->setText(QString::number(money));
    } else {
        qDebug() << "未识别的消息类型:" << json["define"];
    }
    update();
}

void lobby::reconnect()
{
    bool flag = connect(tcp_socket, SIGNAL(readyRead()), this, SLOT(receive_from_server()));
    if (flag == true) {
        qDebug() << "lobby与服务器连接" << endl;
    } else {
        qDebug() << "lobby未能与服务器连接" << endl;
    }
    /*consult();*/ //查询信息
    update();
}

void lobby::on_switch_to_userlist_clicked()
{
    qDebug() << "切换到用户列表页面";
    lobby_page->setCurrentWidget(ui->userlist);

    // 构建请求消息
    QJsonObject requestJson = {
        {"define", SWITCH_TO_USERLIST}, {"socket_id", mo.socket_id}
        // 添加其他必要的参数
    };

    // 向服务器发送请求消息
    tcp_socket->write(QJsonDocument(requestJson).toJson());
}

void lobby::handle_userlist_info(const QJsonArray &userlist_array)
{
    qDebug() << "处理用户列表信息";
    qDebug() << "收到的用户列表信息:" << userlist_array;

    // 清空之前的数据
    ui->userlist_table->clear(); // 清空 QTextBrowser 的内容

    // 创建一个新的 QTextDocument 对象
    QTextDocument *document = new QTextDocument(ui->userlist_table);

    // 创建一个 QTextCursor 对象
    QTextCursor cursor(document);

    // 设置全局字体大小
    QTextBlockFormat blockFormat;
    blockFormat.setLineHeight(150, QTextBlockFormat::ProportionalHeight);
    cursor.setBlockFormat(blockFormat);

    // 创建字体格式对象
    QTextCharFormat defaultFormat;
    defaultFormat.setFontPointSize(16); // 设置整体字体大小为16pt

    // 遍历 JSON 数组
    for (int i = 0; i < userlist_array.size(); ++i) {
        QJsonObject userObject = userlist_array[i].toObject();

        QString username = userObject["username"].toString();
        int is_online = userObject["is_online"].toInt();
        int num_pets = userObject["num_pets"].toInt();
        int num_high_level_pets = userObject["num_high_level_pets"].toInt();
        double win_rate = userObject["win_rate"].toDouble();

        // 插入用户名文本
        cursor.insertText("用户名: " + username + ", 状态: ", defaultFormat);

        // 设置状态文本的颜色
        QTextCharFormat statusFormat = defaultFormat;
        statusFormat.setForeground(is_online ? Qt::green : Qt::red);
        cursor.insertText(is_online ? "在线" : "离线", statusFormat);

        // 插入宠物数量文本
        cursor.insertText(", 宠物数量: " + QString::number(num_pets), defaultFormat);

        // 添加宠物徽章
        if (num_pets >= 10) {
            cursor.insertImage(QImage(":/resource/icon/gold_1.jpeg").scaled(120, 120));
        } else if (num_pets >= 5) {
            cursor.insertImage(QImage(":/resource/icon/silver_1.jpeg").scaled(120, 120));
        } else if (num_pets >= 1) {
            cursor.insertImage(QImage(":/resource/icon/blonde_1.jpeg").scaled(120, 120));
        }

        // 插入高级宠物数量文本
        cursor.insertText(", 高级宠物数量: " + QString::number(num_high_level_pets), defaultFormat);

        // 添加高级宠物徽章
        if (num_high_level_pets >= 10) {
            cursor.insertImage(QImage(":/resource/icon/gold_2.jpeg").scaled(120, 120));
        } else if (num_high_level_pets >= 5) {
            cursor.insertImage(QImage(":/resource/icon/silver_2.jpeg").scaled(120, 120));
        } else if (num_high_level_pets >= 1) {
            cursor.insertImage(QImage(":/resource/icon/blonde_2.jpeg").scaled(120, 120));
        }

        // 插入胜率文本
        cursor.insertText(", 胜率: " + QString::number(win_rate) + "\n", defaultFormat);

        // 插入一个换行符以分隔用户信息
        cursor.insertBlock();
    }

    // 将 QTextDocument 设置到 QTextBrowser
    ui->userlist_table->setDocument(document);
}

void lobby::on_switch_to_petbag_clicked()
{
    qDebug() << "切换到宠物背包" << endl;
    lobby_page->setCurrentWidget(ui->petbag);

    // 构建请求消息
    QJsonObject requestJson = {
        {"define", SWITCH_TO_PETBAG}, {"socket_id", mo.socket_id}, {"user_id", mo.user_id}
        // 添加其他必要的参数
    };
    qDebug() << requestJson << endl;

    // 向服务器发送请求消息
    tcp_socket->write(QJsonDocument(requestJson).toJson());
}

void lobby::handle_pet_info(const QJsonArray &userpets)
{
    qDebug() << "处理用户宠物信息";

    // 创建一个 QGridLayout 布局
    QGridLayout *gridLayout = new QGridLayout();

    // 遍历 JSON 数组，动态添加宠物控件
    for (int i = 0; i < userpets.size(); ++i) {
        QJsonObject pet_object = userpets[i].toObject();

        int pet_id = pet_object["pet_id"].toInt();
        QString pet_name = pet_object["pet_name"].toString();
        int is_evolved = pet_object["is_evolved"].toInt();
        QString image_path;
        QString pet_image_path;

        if (pet_name == "音速犬" || pet_name == "sonic_dog") {
            pet_image_path = "sonic_dog";
        } else if (pet_name == "烈火战神" || pet_name == "blaze_god_of_war") {
            pet_image_path = "blaze_god_of_war";
        } else if (pet_name == "黑熊狂战" || pet_name == "black_bear_berserk") {
            pet_image_path = "black_bear_berserk";
        } else if (pet_name == "阿瑞斯" || pet_name == "ares") {
            pet_image_path = "ares";
        } else if (pet_name == "岩铠领主" || pet_name == "rock_armor_lord") {
            pet_image_path = "rock_armor_lord";
        } else if (pet_name == "琥珀骑士" || pet_name == "amber_knight") {
            pet_image_path = "amber_knight";
        } else if (pet_name == "疾驰奎因" || pet_name == "swift_quinn") {
            pet_image_path = "swift_quinn";
        } else if (pet_name == "迪莫" || pet_name == "dimo") {
            pet_image_path = "dimo";
        }
        // 假设有 pet_image 字段存放图片路径
        if (is_evolved == 0) {
            image_path = ":/resource/pet/" + pet_image_path + "_0" + ".svg";
        } else {
            image_path = ":/resource/pet/" + pet_image_path + "_1" + ".svg";
        }

        // 创建一个宠物按钮，显示宠物图片
        QPushButton *petButton = new QPushButton();
        QPixmap pet_image(image_path);
        petButton->setIcon(QIcon(pet_image));
        petButton->setIconSize(QSize(200, 200)); // 调整图片显示大小
        petButton->setFlat(true);                // 使按钮外观平面化，只显示图片

        // 为宠物按钮添加点击事件，显示详细信息
        connect(petButton, &QPushButton::clicked, this, [=]() { show_pet_details(pet_object); });

        // 添加宠物名字标签
        QLabel *nameLabel = new QLabel(pet_name);
        nameLabel->setAlignment(Qt::AlignCenter);

        // 创建一个垂直布局容器，包含按钮和标签
        QVBoxLayout *petLayout = new QVBoxLayout();
        petLayout->addWidget(petButton);
        petLayout->addWidget(nameLabel);

        // 创建一个容器Widget来包含petLayout
        QWidget *petWidget = new QWidget();
        petWidget->setLayout(petLayout);

        // 将宠物Widget添加到gridLayout中
        int row = i / 5; // 假设每行放5个宠物
        int col = i % 5;
        gridLayout->addWidget(petWidget, row, col);
    }

    // 创建一个QWidget来包含gridLayout
    QWidget *container = new QWidget();
    container->setLayout(gridLayout);

    // 将container添加到scrollArea中
    ui->scrollArea->setWidget(container);
    ui->scrollArea->setWidgetResizable(true);
}

void lobby::show_pet_details(QJsonObject pet_object)
{
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("宠物详细信息");
    dialog->setFixedSize(600, 600); // 设置固定大小

    QVBoxLayout *layout = new QVBoxLayout(dialog);

    QString pet_name = pet_object["pet_name"].toString();
    int pet_attribute = pet_object["pet_attribute"].toInt();
    QString pet_type;
    if (pet_attribute == 0) {
        pet_type = "攻击型";
    } else if (pet_attribute == 1) {
        pet_type = "肉盾型";
    } else if (pet_attribute == 2) {
        pet_type = "防御型";
    } else if (pet_attribute == 3) {
        pet_type = "敏捷型";
    } else {
        pet_type = "错误";
    }
    int level = pet_object["level"].toInt();
    int is_evolved = pet_object["is_evolved"].toInt();
    QString image_path;
    QString show_evolved;
    QString pet_image_path;

    if (pet_name == "音速犬" || pet_name == "sonic_dog") {
        pet_image_path = "sonic_dog";
    } else if (pet_name == "烈火战神" || pet_name == "blaze_god_of_war") {
        pet_image_path = "blaze_god_of_war";
    } else if (pet_name == "黑熊狂战" || pet_name == "black_bear_berserk") {
        pet_image_path = "black_bear_berserk";
    } else if (pet_name == "阿瑞斯" || pet_name == "ares") {
        pet_image_path = "ares";
    } else if (pet_name == "岩铠领主" || pet_name == "rock_armor_lord") {
        pet_image_path = "rock_armor_lord";
    } else if (pet_name == "琥珀骑士" || pet_name == "amber_knight") {
        pet_image_path = "amber_knight";
    } else if (pet_name == "疾驰奎因" || pet_name == "swift_quinn") {
        pet_image_path = "swift_quinn";
    } else if (pet_name == "迪莫" || pet_name == "dimo") {
        pet_image_path = "dimo";
    }

    if (is_evolved == 0) {
        image_path = ":/resource/pet/" + pet_image_path + "_0" + ".svg";
        show_evolved = "未进化";
    } else {
        image_path = ":/resource/pet/" + pet_image_path + "_1" + ".svg";
        show_evolved = "已进化";
    }
    int exp = pet_object["exp"].toInt();
    double damage_point = pet_object["damage_point"].toDouble();
    double defense_point = pet_object["defense_point"].toDouble();
    double hp = pet_object["hp"].toDouble();
    double attack_interval = pet_object["attack_interval"].toDouble();

    // 设置图片
    QLabel *imageLabel = new QLabel(dialog);
    QPixmap petImage(image_path);
    if (!petImage.isNull()) {
        imageLabel->setPixmap(petImage.scaled(200, 200, Qt::KeepAspectRatio)); // 调整图像大小
    } else {
        imageLabel->setText("图片无法加载");
    }
    imageLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(imageLabel);

    // 设置其他详细信息
    QLabel *pet_name_label = new QLabel("名字: " + pet_name, dialog);
    QLabel *pet_type_label = new QLabel("属性: " + pet_type, dialog);
    QLabel *level_label = new QLabel("等级: " + QString::number(level), dialog);
    QLabel *show_evolved_label = new QLabel("是否进化: " + show_evolved, dialog);
    QLabel *exp_label = new QLabel("经验值: " + QString::number(exp), dialog);
    QLabel *damage_point_label = new QLabel("攻击力: " + QString::number(damage_point), dialog);
    QLabel *defense_point_label = new QLabel("护甲: " + QString::number(defense_point), dialog);
    QLabel *hp_label = new QLabel("生命值: " + QString::number(hp), dialog);
    QLabel *attack_interval_label = new QLabel("攻击间隔: " + QString::number(attack_interval)
                                                   + "秒",
                                               dialog);

    layout->addWidget(pet_name_label);
    layout->addWidget(pet_type_label);
    layout->addWidget(level_label);
    layout->addWidget(show_evolved_label);
    layout->addWidget(exp_label);
    layout->addWidget(damage_point_label);
    layout->addWidget(defense_point_label);
    layout->addWidget(hp_label);
    layout->addWidget(attack_interval_label);

    // 添加升级按钮
    QPushButton *upgradeButton = new QPushButton("升级", dialog);
    connect(upgradeButton, &QPushButton::clicked, this, [=]() {
        int user_id = mo.user_id;
        int current_money = mo.money;

        // Step 2: 检查money是否足够
        if (current_money >= 250 && level < 15) {
            // 构建升级请求
            QJsonObject upgradeRequest;
            upgradeRequest["define"] = UPGRADE;
            upgradeRequest["socket_id"] = mo.socket_id;
            upgradeRequest["pet_id"] = pet_object["pet_id"].toInt();
            upgradeRequest["user_id"] = user_id;

            QJsonDocument doc(upgradeRequest);
            QString jsonString = doc.toJson(QJsonDocument::Compact);

            if (tcp_socket->isOpen()) {
                tcp_socket->write(jsonString.toUtf8());
                tcp_socket->flush();
                qDebug() << "发送升级请求:" << jsonString;

                // 显示成功升级的弹窗
                QMessageBox::information(dialog, "升级成功", "成功花费250金币给宠物升一级！");
            } else {
                qDebug() << "套接字未连接，无法发送升级请求";
            }
        } else if (level == 15) {
            QMessageBox::warning(dialog, "升级失败", "宠物已满级，无法升级！");
        } else {
            // money不足，显示失败消息
            QMessageBox::warning(dialog, "升级失败", "用户钱不够，无法升级！");
        }
    });
    layout->addWidget(upgradeButton);

    // 添加关闭按钮
    QPushButton *closeButton = new QPushButton("关闭", dialog);
    connect(closeButton, &QPushButton::clicked, dialog, &QDialog::accept);
    layout->addWidget(closeButton);

    dialog->setLayout(layout);
    dialog->exec();
}

void lobby::on_switch_to_battle_clicked()
{
    qDebug() << "切换到战斗选择界面" << endl;
    lobby_page->setCurrentWidget(ui->battle_choose);

    // 构建请求消息
    QJsonObject requestJson = {
        {"define", SWITCH_TO_BATTLE_CHOOSE}, {"socket_id", mo.socket_id}, {"user_id", mo.user_id}
        // 添加其他必要的参数
    };

    // 向服务器发送请求消息
    tcp_socket->write(QJsonDocument(requestJson).toJson());
}

void lobby::on_userlist_backto_lobby_clicked()
{
    qDebug() << "切换到大厅界面" << endl;
    lobby_page->setCurrentWidget(ui->lobby_2);
}

void lobby::on_petbag_backto_lobby_clicked()
{
    qDebug() << "切换到大厅界面" << endl;
    lobby_page->setCurrentWidget(ui->lobby_2);
}

void lobby::on_battle_choose_backto_lobby_clicked()
{
    qDebug() << "切换到大厅界面" << endl;
    lobby_page->setCurrentWidget(ui->lobby_2);
}

// 重载 closeEvent 函数
void lobby::closeEvent(QCloseEvent *event)
{
    qDebug() << "窗口即将关闭，执行登出操作";
    Signout();       // 执行登出操作
    event->accept(); // 继续关闭窗口
}

// 登出操作的实现
void lobby::Signout()
{
    // 构建登出请求的 JSON 对象
    QJsonObject json = {{"define", SIGNOUT}, {"socket_id", mo.socket_id}, {"user_id", mo.user_id}};

    // 通过套接字发送登出请求到服务器
    if (tcp_socket->isOpen()) {
        tcp_socket->write(QString(QJsonDocument(json).toJson()).toUtf8().data());
        tcp_socket->flush();
        qDebug() << "已发送登出请求:" << json;
    } else {
        qDebug() << "套接字未连接，无法发送登出请求";
    }

    // 关闭套接字连接
    tcp_socket->disconnectFromHost();
}

void lobby::on_addpets_clicked()
{
    qDebug() << "添加满级宠物作弊窗口" << endl;

    // 创建一个弹出窗口来输入作弊码
    QDialog dialog;
    dialog.setWindowTitle("输入作弊码"); // 设置窗口标题
    QVBoxLayout layout(&dialog);
    QLineEdit addpets_code_edit(&dialog);
    QPushButton confirmButton("确认", &dialog);

    layout.addWidget(&addpets_code_edit);
    layout.addWidget(&confirmButton);

    dialog.setLayout(&layout);

    // 连接确认按钮的点击信号到一个匿名函数，用于处理确认操作
    QObject::connect(&confirmButton, &QPushButton::clicked, [&]() {
        QString addpets_code = addpets_code_edit.text();
        if (addpets_code == "flowerwood") {
            // 构建充值请求的 JSON 对象
            QJsonObject json = {{"define", ADDPETS},
                                {"socket_id", mo.socket_id},
                                {"user_id", mo.user_id},
                                {"username", mo.username}};

            // 通过套接字发送充值请求到服务器
            if (tcp_socket->isOpen()) {
                tcp_socket->write(QString(QJsonDocument(json).toJson()).toUtf8().data());
                tcp_socket->flush();
                qDebug() << "已发送充值请求:" << json;
            } else {
                qDebug() << "套接字未连接，无法发送添加宠物作弊请求";
            }
        } else {
            QMessageBox::critical(this, "错误", "请输入正确的作弊代码");
        }

        // 关闭窗口
        dialog.close();
    });

    // 显示窗口
    dialog.exec();
}

void lobby::on_recharge_button_clicked()
{
    qDebug() << "充值" << endl;
    // 创建一个弹出窗口来输入充值码
    QDialog dialog;
    dialog.setWindowTitle("输入充值码"); // 设置窗口标题
    QVBoxLayout layout(&dialog);
    QLineEdit recharge_code_edit(&dialog);
    QPushButton confirmButton("确认", &dialog);

    layout.addWidget(&recharge_code_edit);
    layout.addWidget(&confirmButton);

    dialog.setLayout(&layout);

    // 连接确认按钮的点击信号到一个匿名函数，用于处理确认操作
    QObject::connect(&confirmButton, &QPushButton::clicked, [&]() {
        QString recharge_code = recharge_code_edit.text();
        if (recharge_code == "takagi") {
            // 构建充值请求的 JSON 对象
            QJsonObject json = {{"define", RECHARGE},
                                {"socket_id", mo.socket_id},
                                {"username", mo.username},
                                {"recharge_code", recharge_code}};

            // 通过套接字发送充值请求到服务器
            if (tcp_socket->isOpen()) {
                tcp_socket->write(QString(QJsonDocument(json).toJson()).toUtf8().data());
                tcp_socket->flush();
                qDebug() << "已发送充值请求:" << json;
            } else {
                qDebug() << "套接字未连接，无法发送充值请求";
            }
        } else {
            QMessageBox::critical(this, "错误", "请输入正确的充值代码");
        }

        // 关闭窗口
        dialog.close();
    });

    // 显示窗口
    dialog.exec();
}

void lobby::handle_battle_choose(const QJsonArray &user_pets, const QJsonArray &server_user_pets)
{
    qDebug() << "处理战斗选择，加载用户和服务器的宠物列表";

    // 清空之前的数据
    ui->user_pets_list->clear();
    ui->server_pets_list->clear();

    // 设置字体属性
    QFont itemFont;
    itemFont.setPointSize(12); // 设置字体大小为12，可以根据需要调整

    // 遍历 JSON 数组
    for (const QJsonValue &value : user_pets) {
        QJsonObject petObject = value.toObject();

        // 获取宠物的相关信息
        int user_pet_id = petObject["pet_id"].toInt();
        QString user_pet_name = petObject["pet_name"].toString();
        int user_pet_level = petObject["level"].toInt();
        double user_pet_hp = petObject["hp"].toDouble();
        double user_pet_damage_point = petObject["damage_point"].toDouble();
        double user_pet_defense_point = petObject["defense_point"].toDouble();
        double user_pet_attack_interval = petObject["attack_interval"].toDouble();

        // 创建一个列表项，显示宠物的ID、名字和等级
        QListWidgetItem *item = new QListWidgetItem(
            QString(
                "宠物id: %1, 名字: %2, 等级: %3, 生命值：%4, 攻击力：%5， 护甲：%6， 攻击间隔：%7")
                .arg(user_pet_id)
                .arg(user_pet_name)
                .arg(user_pet_level)
                .arg(user_pet_hp)
                .arg(user_pet_damage_point)
                .arg(user_pet_defense_point)
                .arg(user_pet_attack_interval));

        // 将宠物ID存储在列表项的data中，以便后续使用
        item->setData(Qt::UserRole, user_pet_id);

        // 设置列表项的字体
        item->setFont(itemFont);

        // 将列表项添加到用户宠物列表中
        ui->user_pets_list->addItem(item);
    }

    // 遍历服务器的 JSON 数组
    for (const QJsonValue &value : server_user_pets) {
        QJsonObject petObject = value.toObject();

        // 获取宠物的相关信息
        int server_pet_id = petObject["pet_id"].toInt();
        QString server_pet_name = petObject["pet_name"].toString();
        int server_pet_level = petObject["level"].toInt();
        double server_pet_hp = petObject["hp"].toDouble();
        double server_pet_damage_point = petObject["damage_point"].toDouble();
        double server_pet_defense_point = petObject["defense_point"].toDouble();
        double server_pet_attack_interval = petObject["attack_interval"].toDouble();

        // 创建一个列表项，显示宠物的ID、名字和等级
        QListWidgetItem *item = new QListWidgetItem(
            QString(
                "宠物id: %1, 名字: %2, 等级: %3, 生命值：%4, 攻击力：%5， 护甲：%6, 攻击间隔：%7")
                .arg(server_pet_id)
                .arg(server_pet_name)
                .arg(server_pet_level)
                .arg(server_pet_hp)
                .arg(server_pet_damage_point)
                .arg(server_pet_defense_point)
                .arg(server_pet_attack_interval));

        // 将宠物ID存储在列表项的data中，以便后续使用
        item->setData(Qt::UserRole, server_pet_id);

        // 设置列表项的字体
        item->setFont(itemFont);

        // 将列表项添加到服务器宠物列表中
        ui->server_pets_list->addItem(item);
    }

    // 连接用户宠物列表的选择变化信号到相应的槽函数
    connect(ui->user_pets_list, &QListWidget::itemClicked, this, &lobby::choose_user_pet);

    // 连接服务器宠物列表的选择变化信号到相应的槽函数
    connect(ui->server_pets_list, &QListWidget::itemClicked, this, &lobby::choose_server_pet);

    qDebug() << "用户宠物列表是否可见: " << ui->user_pets_list->isVisible();
    qDebug() << "服务器宠物列表是否可见: " << ui->server_pets_list->isVisible();
}

// 用户宠物被选中时的处理函数
void lobby::choose_user_pet(QListWidgetItem *item)
{
    int selectedPetId = item->data(Qt::UserRole).toInt();
    qDebug() << "用户选择了宠物，ID:" << selectedPetId;

    mo.client_pet = selectedPetId;
}

// 服务器宠物被选中时的处理函数
void lobby::choose_server_pet(QListWidgetItem *item)
{
    int selected_pet_id = item->data(Qt::UserRole).toInt();
    qDebug() << "服务器宠物被选中，ID:" << selected_pet_id;
    mo.server_pet = selected_pet_id;
}

void lobby::on_flush_serve_pets_clicked()
{
    qDebug() << "刷新服务器宠物" << endl;
    QJsonObject json = {{"define", FLUSH_SERVER}, {"socket_id", mo.socket_id}};
    if (tcp_socket->isOpen()) {
        tcp_socket->write(QString(QJsonDocument(json).toJson()).toUtf8().data());
        tcp_socket->flush();
        qDebug() << "已发送刷新服务器宠物请求:" << json;
    } else {
        qDebug() << "套接字未连接，无法发送刷新服务器宠物请求";
    }
}

void lobby::handle_flush_server_pets(const QJsonArray &server_user_pets)
{
    ui->server_pets_list->clear();

    // 设置字体属性
    QFont itemFont;
    itemFont.setPointSize(12); // 设置字体大小为12，可以根据需要调整

    // 遍历服务器的 JSON 数组
    for (const QJsonValue &value : server_user_pets) {
        QJsonObject petObject = value.toObject();

        // 获取宠物的相关信息
        int server_pet_id = petObject["pet_id"].toInt();
        QString server_pet_name = petObject["pet_name"].toString();
        int server_pet_level = petObject["level"].toInt();
        double server_pet_hp = petObject["hp"].toDouble();
        double server_pet_damage_point = petObject["damage_point"].toDouble();
        double server_pet_defense_point = petObject["defense_point"].toDouble();
        double server_pet_attack_interval = petObject["attack_interval"].toDouble();

        // 创建一个列表项，显示宠物的ID、名字和等级
        QListWidgetItem *item = new QListWidgetItem(
            QString(
                "宠物id: %1, 名字: %2, 等级: %3, 生命值：%4, 攻击力：%5， 护甲：%6, 攻击间隔：%7")
                .arg(server_pet_id)
                .arg(server_pet_name)
                .arg(server_pet_level)
                .arg(server_pet_hp)
                .arg(server_pet_damage_point)
                .arg(server_pet_defense_point)
                .arg(server_pet_attack_interval));

        // 将宠物ID存储在列表项的data中，以便后续使用
        item->setData(Qt::UserRole, server_pet_id);

        // 设置列表项的字体
        item->setFont(itemFont);

        // 将列表项添加到服务器宠物列表中
        ui->server_pets_list->addItem(item);
    }
}

void lobby::on_add_server_pets_clicked()
{
    qDebug() << "增加服务器宠物" << endl;
    QJsonObject json = {{"define", ADD_SERVER_PETS}, {"socket_id", mo.socket_id}};
    if (tcp_socket->isOpen()) {
        tcp_socket->write(QString(QJsonDocument(json).toJson()).toUtf8().data());
        tcp_socket->flush();
        qDebug() << "已发送刷新服务器宠物请求:" << json;
    } else {
        qDebug() << "套接字未连接，无法发送刷新服务器宠物请求";
    }
}

void lobby::updateSelectionStatus()
{
    // 检查用户宠物列表是否有选中项
    user_pet_selected = (ui->user_pets_list->selectedItems().count() > 0);

    // 检查服务器宠物列表是否有选中项
    server_pet_selected = (ui->server_pets_list->selectedItems().count() > 0);

    // 如果两个列表都有选中项，启用开始战斗按钮
    if (user_pet_selected && server_pet_selected) {
        ui->start_vs_battle->setEnabled(true);
        ui->start_upgrade_battle->setEnabled(true);
    } else {
        ui->start_vs_battle->setEnabled(false);
        ui->start_upgrade_battle->setEnabled(false);
    }
}

void lobby::on_start_upgrade_battle_clicked()
{
    qDebug() << "开始升级战信号" << endl;

    QJsonObject json = {{"define", START_UPGRADE_BATTLE},
                        {"socket_id", mo.socket_id},
                        {"user_pet_id", mo.client_pet},
                        {"server_pet_id", mo.server_pet}};
    if (tcp_socket->isOpen()) {
        tcp_socket->write(QString(QJsonDocument(json).toJson()).toUtf8().data());
        tcp_socket->flush();
        qDebug() << "已发送开始升级战请求:" << json;
    } else {
        qDebug() << "套接字未连接，无法发送开始升级战请求";
    }
    qDebug() << "切换到战斗选择界面" << endl;
    lobby_page->setCurrentWidget(ui->upgrade_battle);
}

void lobby::Upgrade_battle(QJsonObject user_pet, QJsonObject server_pet)
{
    // 提取用户宠物信息
    int user_pet_id = user_pet["pet_id"].toInt();
    QString user_pet_name = user_pet["pet_name"].toString();
    int user_pet_level = user_pet["level"].toInt();
    int user_pet_is_evolved = user_pet["is_evolved"].toInt();
    double user_pet_hp = user_pet["hp"].toDouble();
    // 创建用户宠物对象
    if (user_pet_name == "音速犬" || user_pet_name == "sonic_dog") {
        p1 = new sonic_dog(user_pet_level);
    } else if (user_pet_name == "烈火战神" || user_pet_name == "blaze_god_of_war") {
        p1 = new blaze_god_of_war(user_pet_level);
    } else if (user_pet_name == "黑熊狂战" || user_pet_name == "black_bear_berserk") {
        p1 = new black_bear_berserk(user_pet_level);
    } else if (user_pet_name == "阿瑞斯" || user_pet_name == "ares") {
        p1 = new ares(user_pet_level);
    } else if (user_pet_name == "岩铠领主" || user_pet_name == "rock_armor_lord") {
        p1 = new rock_armor_lord(user_pet_level);
    } else if (user_pet_name == "琥珀骑士" || user_pet_name == "amber_knight") {
        p1 = new amber_knight(user_pet_level);
    } else if (user_pet_name == "疾驰奎因" || user_pet_name == "swift_quinn") {
        p1 = new swift_quinn(user_pet_level);
    } else if (user_pet_name == "迪莫" || user_pet_name == "dimo") {
        p1 = new dimo(user_pet_level);
    }
    qDebug() << "p1:" << p1->get_hp() << p1->get_fight_hp() << p1->get_level()
             << p1->get_damage_point() << p1->get_fight_damage_point()
             << p1->get_fight_damage_gain() << p1->get_defense_point()
             << p1->get_fight_defense_point() << p1->get_fight_defense_gain() << endl;

    QString user_pet_image_path;
    QString pet_image_path;
    if (user_pet_name == "音速犬" || user_pet_name == "sonic_dog") {
        pet_image_path = "sonic_dog";
    } else if (user_pet_name == "烈火战神" || user_pet_name == "blaze_god_of_war") {
        pet_image_path = "blaze_god_of_war";
    } else if (user_pet_name == "黑熊狂战" || user_pet_name == "black_bear_berserk") {
        pet_image_path = "black_bear_berserk";
    } else if (user_pet_name == "阿瑞斯" || user_pet_name == "ares") {
        pet_image_path = "ares";
    } else if (user_pet_name == "岩铠领主" || user_pet_name == "rock_armor_lord") {
        pet_image_path = "rock_armor_lord";
    } else if (user_pet_name == "琥珀骑士" || user_pet_name == "amber_knight") {
        pet_image_path = "amber_knight";
    } else if (user_pet_name == "疾驰奎因" || user_pet_name == "swift_quinn") {
        pet_image_path = "swift_quinn";
    } else if (user_pet_name == "迪莫" || user_pet_name == "dimo") {
        pet_image_path = "dimo";
    }
    // 假设有 pet_image 字段存放图片路径
    if (user_pet_is_evolved == 0) {
        user_pet_image_path = ":/resource/pet/" + pet_image_path + "_0" + ".svg";
    } else {
        user_pet_image_path = ":/resource/pet/" + pet_image_path + "_1" + ".svg";
    }

    // 提取服务器宠物信息
    int server_pet_id = server_pet["pet_id"].toInt();
    QString server_pet_name = server_pet["pet_name"].toString();
    int server_pet_level = server_pet["level"].toInt();
    int server_pet_is_evolved = server_pet["is_evolved"].toInt();

    // 创建用户宠物对象
    if (server_pet_name == "音速犬" || user_pet_name == "sonic_dog") {
        p2 = new sonic_dog(server_pet_level);
    } else if (server_pet_name == "烈火战神" || server_pet_name == "blaze_god_of_war") {
        p2 = new blaze_god_of_war(server_pet_level);
    } else if (server_pet_name == "黑熊狂战" || server_pet_name == "black_bear_berserk") {
        p2 = new black_bear_berserk(server_pet_level);
    } else if (server_pet_name == "阿瑞斯" || server_pet_name == "ares") {
        p2 = new ares(server_pet_level);
    } else if (server_pet_name == "岩铠领主" || server_pet_name == "rock_armor_lord") {
        p2 = new rock_armor_lord(server_pet_level);
    } else if (server_pet_name == "琥珀骑士" || server_pet_name == "amber_knight") {
        p2 = new amber_knight(server_pet_level);
    } else if (server_pet_name == "疾驰奎因" || server_pet_name == "swift_quinn") {
        p2 = new swift_quinn(server_pet_level);
    } else if (server_pet_name == "迪莫" || server_pet_name == "dimo") {
        p2 = new dimo(server_pet_level);
    }

    qDebug() << "p2:" << p1->get_hp() << p2->get_fight_hp() << p2->get_level()
             << p2->get_damage_point() << p2->get_fight_damage_point()
             << p2->get_fight_damage_gain() << p2->get_defense_point()
             << p2->get_fight_defense_point() << p2->get_fight_defense_gain() << endl;

    QString server_pet_image_path;
    QString pet_image_path1;
    if (server_pet_name == "音速犬" || server_pet_name == "sonic_dog") {
        pet_image_path1 = "sonic_dog";
    } else if (server_pet_name == "烈火战神" || server_pet_name == "blaze_god_of_war") {
        pet_image_path1 = "blaze_god_of_war";
    } else if (server_pet_name == "黑熊狂战" || server_pet_name == "black_bear_berserk") {
        pet_image_path1 = "black_bear_berserk";
    } else if (server_pet_name == "阿瑞斯" || server_pet_name == "ares") {
        pet_image_path1 = "ares";
    } else if (server_pet_name == "岩铠领主" || server_pet_name == "rock_armor_lord") {
        pet_image_path1 = "rock_armor_lord";
    } else if (server_pet_name == "琥珀骑士" || server_pet_name == "amber_knight") {
        pet_image_path1 = "amber_knight";
    } else if (server_pet_name == "疾驰奎因" || server_pet_name == "swift_quinn") {
        pet_image_path1 = "swift_quinn";
    } else if (server_pet_name == "迪莫" || server_pet_name == "dimo") {
        pet_image_path1 = "dimo";
    }
    // 假设有 pet_image 字段存放图片路径
    if (server_pet_is_evolved == 0) {
        server_pet_image_path = ":/resource/pet/" + pet_image_path1 + "_0" + ".svg";
    } else {
        server_pet_image_path = ":/resource/pet/" + pet_image_path1 + "_1" + ".svg";
    }

    // 将宠物的图片设置到 QLabel 中
    displayPetImage(ui->user_pet_attack, user_pet_image_path);
    displayPetImage(ui->server_pet_attack, server_pet_image_path);

    // 初始化用户宠物的血量进度条
    ui->user_pet_hp->setMinimum(0);
    ui->user_pet_hp->setMaximum(p1->get_hp());
    ui->user_pet_hp->setValue(p1->get_fight_hp());
    ui->user_pet_name_b->setText(user_pet_name);
    ui->user_pet_hp_d->setText(QString::number(p1->get_fight_hp()) + "/"
                               + QString::number(p1->get_hp()));

    // 初始化服务器宠物的血量进度条
    ui->server_pet_hp->setMinimum(0);
    ui->server_pet_hp->setMaximum(p2->get_hp());
    ui->server_pet_hp->setValue(p2->get_fight_hp());
    ui->server_pet_name_b->setText(server_pet_name);
    ui->server_pet_hp_d->setText(QString::number(p2->get_fight_hp()) + "/"
                                 + QString::number(p2->get_hp()));
    on_upgrade_battle();
}

void lobby::on_upgrade_battle()
{
    ui->upgrade_battle_log->clear();
    update_user_pet_hp(p1->get_fight_hp(), p1->get_hp());
    update_server_pet_hp(p2->get_fight_hp(), p2->get_hp());

    attack_timer_user = new QTimer(this);
    attack_timer_server = new QTimer(this);

    connect(attack_timer_user, &QTimer::timeout, this, &lobby::user_attack);
    connect(attack_timer_server, &QTimer::timeout, this, &lobby::server_attack);

    // 连接战斗日志信号和槽
    connect(p1, &Pet::logGenerated, this, &lobby::onBattleLogReceived);
    connect(p2, &Pet::logGenerated, this, &lobby::onBattleLogReceived);

    attack_timer_user->start(static_cast<int>(p1->get_fight_attack_interval() * 1000));
    attack_timer_server->start(static_cast<int>(p2->get_fight_attack_interval() * 1000));
}

void lobby::update_user_pet_hp(int current_hp, int total_hp)
{
    ui->user_pet_hp->setMinimum(0);
    ui->user_pet_hp->setMaximum(total_hp);
    if (current_hp <= 0) {
        ui->user_pet_hp->setValue(0);
    }
    ui->user_pet_hp->setValue(current_hp);
    ui->user_pet_hp_d->setText(QString::number(current_hp) + "/" + QString::number(total_hp));
}

void lobby::update_user_pet_hp_vs(int current_hp, int total_hp)
{
    ui->user_pet_hp_2->setMinimum(0);
    ui->user_pet_hp_2->setMaximum(total_hp);
    if (current_hp <= 0) {
        ui->user_pet_hp_2->setValue(0);
    }
    ui->user_pet_hp_2->setValue(current_hp);
    ui->user_pet_hp_d_2->setText(QString::number(current_hp) + "/" + QString::number(total_hp));
}

void lobby::update_server_pet_hp(int current_hp, int total_hp)
{
    ui->server_pet_hp->setMinimum(0);
    ui->server_pet_hp->setMaximum(total_hp);
    if (current_hp <= 0) {
        ui->server_pet_hp->setValue(0);
    }
    ui->server_pet_hp->setValue(current_hp);
    ui->server_pet_hp_d->setText(QString::number(current_hp) + "/" + QString::number(total_hp));
}

void lobby::update_server_pet_hp_vs(int current_hp, int total_hp)
{
    ui->server_pet_hp_2->setMinimum(0);
    ui->server_pet_hp_2->setMaximum(total_hp);
    if (current_hp <= 0) {
        ui->server_pet_hp_2->setValue(0);
    }
    ui->server_pet_hp_2->setValue(current_hp);
    ui->server_pet_hp_d_2->setText(QString::number(current_hp) + "/" + QString::number(total_hp));
}

void lobby::user_attack()
{
    if (p1->get_fight_hp() <= 0 || p2->get_fight_hp() <= 0) {
        attack_timer_user->stop();
        attack_timer_server->stop();
        QString winner = p1->get_fight_hp() > 0 ? "用户" : "服务器";
        emit logGenerated("战斗结束，" + winner + "获胜！");
        if (winner == "用户") {
            mo.win = 1;
        } else {
            mo.win = -1;
        }
        int experience = 1;
        for (int i = 0; i < p2->get_level(); i++) {
            experience = experience * 2;
        }
        if (mo.win == 1) {
            QJsonObject json = {{
                                    "define",
                                    UPGRADE_WIN,
                                },
                                {"socket_id", mo.socket_id},
                                {"exp", experience},
                                {"pet_id", mo.client_pet},
                                {"user_id", mo.user_id}};
            if (tcp_socket->isOpen()) {
                tcp_socket->write(QString(QJsonDocument(json).toJson()).toUtf8().data());
                tcp_socket->flush();
                qDebug() << "已发送升级战胜利请求:" << json;
            } else {
                qDebug() << "套接字未连接，无法发送升级战胜利请求";
            }
        } else if (mo.win == -1) {
            QJsonObject json = {
                {
                    "define",
                    UPGRADE_LOSE,
                },
                {"socket_id", mo.socket_id},
                {"user_id", mo.user_id},
            };
            if (tcp_socket->isOpen()) {
                tcp_socket->write(QString(QJsonDocument(json).toJson()).toUtf8().data());
                tcp_socket->flush();
                qDebug() << "已发送决斗战落败请求:" << json;
            } else {
                qDebug() << "套接字未连接，无法发送决斗战落败请求";
            }
        }
        showResultMessage(mo.win, experience);
        return;
    }
    p1->attack(p2);
    update_user_pet_hp(p1->get_fight_hp(), p1->get_hp());
    update_server_pet_hp(p2->get_fight_hp(), p2->get_hp());
    shakePetImage(ui->user_pet_attack);
}
void lobby::user_attack_vs()
{
    if (p1->get_fight_hp() <= 0 || p2->get_fight_hp() <= 0) {
        attack_timer_user->stop();
        attack_timer_server->stop();
        QString winner = p1->get_fight_hp() > 0 ? "用户" : "服务器";
        emit logGenerated("战斗结束，" + winner + "获胜！");
        if (winner == "用户") {
            mo.win = 1;
        } else {
            mo.win = -1;
        }
        int experience = 1;
        for (int i = 0; i < p2->get_level(); i++) {
            experience = experience * 2;
        }
        if (mo.win == 1) {
            QJsonObject json = {{
                                    "define",
                                    VS_WIN,
                                },
                                {"socket_id", mo.socket_id},
                                {"exp", experience},
                                {"user_pet_id", mo.client_pet},
                                {"server_pet_id", mo.server_pet},
                                {"user_id", mo.user_id}};
            if (tcp_socket->isOpen()) {
                tcp_socket->write(QString(QJsonDocument(json).toJson()).toUtf8().data());
                tcp_socket->flush();
                qDebug() << "已发送决斗战胜利请求:" << json;
            } else {
                qDebug() << "套接字未连接，无法发送决斗战胜利请求";
            }
        } else if (mo.win == -1) {
            QJsonObject json = {
                {
                    "define",
                    VS_LOSE,
                },
                {"socket_id", mo.socket_id},
                {"user_id", mo.user_id},
            };
            if (tcp_socket->isOpen()) {
                tcp_socket->write(QString(QJsonDocument(json).toJson()).toUtf8().data());
                tcp_socket->flush();
                qDebug() << "已发送决斗战落败请求:" << json;
            } else {
                qDebug() << "套接字未连接，无法发送决斗战落败请求";
            }
        }

        showResultMessage_vs(mo.win, experience);
        return;
    }
    p1->attack(p2);
    update_user_pet_hp_vs(p1->get_fight_hp(), p1->get_hp());
    update_server_pet_hp_vs(p2->get_fight_hp(), p2->get_hp());
    shakePetImage(ui->user_pet_attack_2);
}

void lobby::server_attack()
{
    if (p1->get_fight_hp() <= 0 || p2->get_fight_hp() <= 0) {
        attack_timer_server->stop();
        attack_timer_user->stop();
        QString winner = p1->get_fight_hp() > 0 ? "用户" : "服务器";
        emit logGenerated("战斗结束，" + winner + "获胜！");
        if (winner == "用户") {
            mo.win = 1;
        } else {
            mo.win = -1;
        }
        int experience = 1;
        for (int i = 0; i < p2->get_level(); i++) {
            experience = experience * 2;
        }
        if (mo.win == 1) {
            QJsonObject json = {{
                                    "define",
                                    UPGRADE_WIN,
                                },
                                {"socket_id", mo.socket_id},
                                {"exp", experience},
                                {"pet_id", mo.client_pet},
                                {"user_id", mo.user_id}};
            if (tcp_socket->isOpen()) {
                tcp_socket->write(QString(QJsonDocument(json).toJson()).toUtf8().data());
                tcp_socket->flush();
                qDebug() << "已发送升级战胜利请求:" << json;
            } else {
                qDebug() << "套接字未连接，无法发送升级战胜利请求";
            }
        } else if (mo.win == -1) {
            QJsonObject json = {
                {
                    "define",
                    UPGRADE_LOSE,
                },
                {"socket_id", mo.socket_id},
                {"user_id", mo.user_id},
            };
            if (tcp_socket->isOpen()) {
                tcp_socket->write(QString(QJsonDocument(json).toJson()).toUtf8().data());
                tcp_socket->flush();
                qDebug() << "已发送决斗战落败请求:" << json;
            } else {
                qDebug() << "套接字未连接，无法发送决斗战落败请求";
            }
        }
        showResultMessage(mo.win, experience);
        return;
    }

    p2->attack(p1);
    update_user_pet_hp(p1->get_fight_hp(), p1->get_hp());
    update_server_pet_hp(p2->get_fight_hp(), p2->get_hp());
    shakePetImage(ui->server_pet_attack);
}

void lobby::server_attack_vs()
{
    if (p1->get_fight_hp() <= 0 || p2->get_fight_hp() <= 0) {
        attack_timer_server->stop();
        attack_timer_user->stop();
        QString winner = p1->get_fight_hp() > 0 ? "用户" : "服务器";
        emit logGenerated("战斗结束，" + winner + "获胜！");
        if (winner == "用户") {
            mo.win = 1;
        } else {
            mo.win = -1;
        }
        int experience = 1;
        for (int i = 0; i < p2->get_level(); i++) {
            experience = experience * 2;
        }
        if (mo.win == 1) {
            QJsonObject json = {{
                                    "define",
                                    VS_WIN,
                                },
                                {"socket_id", mo.socket_id},
                                {"exp", experience},
                                {"user_pet_id", mo.client_pet},
                                {"server_pet_id", mo.server_pet},
                                {"user_id", mo.user_id}};
            if (tcp_socket->isOpen()) {
                tcp_socket->write(QString(QJsonDocument(json).toJson()).toUtf8().data());
                tcp_socket->flush();
                qDebug() << "已发送决斗战胜利请求:" << json;
            } else {
                qDebug() << "套接字未连接，无法发送决斗战胜利请求";
            }
        } else if (mo.win == -1) {
            QJsonObject json = {
                {
                    "define",
                    VS_LOSE,
                },
                {"socket_id", mo.socket_id},
                {"user_id", mo.user_id},
            };
            if (tcp_socket->isOpen()) {
                tcp_socket->write(QString(QJsonDocument(json).toJson()).toUtf8().data());
                tcp_socket->flush();
                qDebug() << "已发送决斗战落败请求:" << json;
            } else {
                qDebug() << "套接字未连接，无法发送决斗战落败请求";
            }
        }

        showResultMessage_vs(mo.win, experience);
        return;
    }
    p2->attack(p1);
    update_user_pet_hp_vs(p1->get_fight_hp(), p1->get_hp());
    update_server_pet_hp_vs(p2->get_fight_hp(), p2->get_hp());
    shakePetImage(ui->server_pet_attack_2);
}

void lobby::displayPetImage(QLabel *label, const QString &imagePath)
{
    // 创建一个 QPixmap 对象，用于加载图片
    QPixmap pixmap(imagePath);

    if (pixmap.isNull()) {
        qDebug() << "加载图片失败:" << imagePath;
        return;
    }

    // 设置 QLabel 的 pixmap
    label->setPixmap(pixmap);

    // 调整 QLabel 的大小以适应图片
    label->setScaledContents(true);
    label->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    label->setPixmap(pixmap.scaled(label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void lobby::on_start_vs_battle_clicked()
{
    qDebug() << "开始决斗战信号" << endl;

    QJsonObject json = {{"define", START_VS_BATTLE},
                        {"socket_id", mo.socket_id},
                        {"user_pet_id", mo.client_pet},
                        {"server_pet_id", mo.server_pet}};
    if (tcp_socket->isOpen()) {
        tcp_socket->write(QString(QJsonDocument(json).toJson()).toUtf8().data());
        tcp_socket->flush();
        qDebug() << "已发送开始决斗战请求:" << json;
    } else {
        qDebug() << "套接字未连接，无法发送开始决斗战请求";
    }
    qDebug() << "切换到决斗战界面" << endl;
    lobby_page->setCurrentWidget(ui->vs_battle);
}

void lobby::Vs_battle(QJsonObject user_pet, QJsonObject server_pet)
{
    // 提取用户宠物信息
    int user_pet_id = user_pet["pet_id"].toInt();
    QString user_pet_name = user_pet["pet_name"].toString();
    int user_pet_level = user_pet["level"].toInt();
    int user_pet_is_evolved = user_pet["is_evolved"].toInt();
    double user_pet_hp = user_pet["hp"].toDouble();
    // 创建用户宠物对象
    if (user_pet_name == "音速犬" || user_pet_name == "sonic_dog") {
        p1 = new sonic_dog(user_pet_level);
    } else if (user_pet_name == "烈火战神" || user_pet_name == "blaze_god_of_war") {
        p1 = new blaze_god_of_war(user_pet_level);
    } else if (user_pet_name == "黑熊狂战" || user_pet_name == "black_bear_berserk") {
        p1 = new black_bear_berserk(user_pet_level);
    } else if (user_pet_name == "阿瑞斯" || user_pet_name == "ares") {
        p1 = new ares(user_pet_level);
    } else if (user_pet_name == "岩铠领主" || user_pet_name == "rock_armor_lord") {
        p1 = new rock_armor_lord(user_pet_level);
    } else if (user_pet_name == "琥珀骑士" || user_pet_name == "amber_knight") {
        p1 = new amber_knight(user_pet_level);
    } else if (user_pet_name == "疾驰奎因" || user_pet_name == "swift_quinn") {
        p1 = new swift_quinn(user_pet_level);
    } else if (user_pet_name == "迪莫" || user_pet_name == "dimo") {
        p1 = new dimo(user_pet_level);
    }
    qDebug() << "p1:" << p1->get_hp() << p1->get_fight_hp() << p1->get_level()
             << p1->get_damage_point() << p1->get_fight_damage_point()
             << p1->get_fight_damage_gain() << p1->get_defense_point()
             << p1->get_fight_defense_point() << p1->get_fight_defense_gain() << endl;

    QString user_pet_image_path;
    QString pet_image_path;
    if (user_pet_name == "音速犬" || user_pet_name == "sonic_dog") {
        pet_image_path = "sonic_dog";
    } else if (user_pet_name == "烈火战神" || user_pet_name == "blaze_god_of_war") {
        pet_image_path = "blaze_god_of_war";
    } else if (user_pet_name == "黑熊狂战" || user_pet_name == "black_bear_berserk") {
        pet_image_path = "black_bear_berserk";
    } else if (user_pet_name == "阿瑞斯" || user_pet_name == "ares") {
        pet_image_path = "ares";
    } else if (user_pet_name == "岩铠领主" || user_pet_name == "rock_armor_lord") {
        pet_image_path = "rock_armor_lord";
    } else if (user_pet_name == "琥珀骑士" || user_pet_name == "amber_knight") {
        pet_image_path = "amber_knight";
    } else if (user_pet_name == "疾驰奎因" || user_pet_name == "swift_quinn") {
        pet_image_path = "swift_quinn";
    } else if (user_pet_name == "迪莫" || user_pet_name == "dimo") {
        pet_image_path = "dimo";
    }
    // 假设有 pet_image 字段存放图片路径
    if (user_pet_is_evolved == 0) {
        user_pet_image_path = ":/resource/pet/" + pet_image_path + "_0" + ".svg";
    } else {
        user_pet_image_path = ":/resource/pet/" + pet_image_path + "_1" + ".svg";
    }

    // 提取服务器宠物信息
    int server_pet_id = server_pet["pet_id"].toInt();
    QString server_pet_name = server_pet["pet_name"].toString();
    int server_pet_level = server_pet["level"].toInt();
    int server_pet_is_evolved = server_pet["is_evolved"].toInt();

    // 创建用户宠物对象
    if (server_pet_name == "音速犬" || user_pet_name == "sonic_dog") {
        p2 = new sonic_dog(server_pet_level);
    } else if (server_pet_name == "烈火战神" || server_pet_name == "blaze_god_of_war") {
        p2 = new blaze_god_of_war(server_pet_level);
    } else if (server_pet_name == "黑熊狂战" || server_pet_name == "black_bear_berserk") {
        p2 = new black_bear_berserk(server_pet_level);
    } else if (server_pet_name == "阿瑞斯" || server_pet_name == "ares") {
        p2 = new ares(server_pet_level);
    } else if (server_pet_name == "岩铠领主" || server_pet_name == "rock_armor_lord") {
        p2 = new rock_armor_lord(server_pet_level);
    } else if (server_pet_name == "琥珀骑士" || server_pet_name == "amber_knight") {
        p2 = new amber_knight(server_pet_level);
    } else if (server_pet_name == "疾驰奎因" || server_pet_name == "swift_quinn") {
        p2 = new swift_quinn(server_pet_level);
    } else if (server_pet_name == "迪莫" || server_pet_name == "dimo") {
        p2 = new dimo(server_pet_level);
    }

    qDebug() << "p2:" << p1->get_hp() << p2->get_fight_hp() << p2->get_level()
             << p2->get_damage_point() << p2->get_fight_damage_point()
             << p2->get_fight_damage_gain() << p2->get_defense_point()
             << p2->get_fight_defense_point() << p2->get_fight_defense_gain() << endl;

    QString server_pet_image_path;
    QString pet_image_path1;
    if (server_pet_name == "音速犬" || server_pet_name == "sonic_dog") {
        pet_image_path1 = "sonic_dog";
    } else if (server_pet_name == "烈火战神" || server_pet_name == "blaze_god_of_war") {
        pet_image_path1 = "blaze_god_of_war";
    } else if (server_pet_name == "黑熊狂战" || server_pet_name == "black_bear_berserk") {
        pet_image_path1 = "black_bear_berserk";
    } else if (server_pet_name == "阿瑞斯" || server_pet_name == "ares") {
        pet_image_path1 = "ares";
    } else if (server_pet_name == "岩铠领主" || server_pet_name == "rock_armor_lord") {
        pet_image_path1 = "rock_armor_lord";
    } else if (server_pet_name == "琥珀骑士" || server_pet_name == "amber_knight") {
        pet_image_path1 = "amber_knight";
    } else if (server_pet_name == "疾驰奎因" || server_pet_name == "swift_quinn") {
        pet_image_path1 = "swift_quinn";
    } else if (server_pet_name == "迪莫" || server_pet_name == "dimo") {
        pet_image_path1 = "dimo";
    }
    // 假设有 pet_image 字段存放图片路径
    if (server_pet_is_evolved == 0) {
        server_pet_image_path = ":/resource/pet/" + pet_image_path1 + "_0" + ".svg";
    } else {
        server_pet_image_path = ":/resource/pet/" + pet_image_path1 + "_1" + ".svg";
    }

    // 将宠物的图片设置到 QLabel 中
    displayPetImage(ui->user_pet_attack_2, user_pet_image_path);
    displayPetImage(ui->server_pet_attack_2, server_pet_image_path);

    // 初始化用户宠物的血量进度条
    ui->user_pet_hp_2->setMinimum(0);
    ui->user_pet_hp_2->setMaximum(p1->get_hp());
    ui->user_pet_hp_2->setValue(p1->get_fight_hp());
    ui->user_pet_name_b_2->setText(user_pet_name);
    ui->user_pet_hp_d_2->setText(QString::number(p1->get_fight_hp()) + "/"
                                 + QString::number(p1->get_hp()));

    // 初始化服务器宠物的血量进度条
    ui->server_pet_hp_2->setMinimum(0);
    ui->server_pet_hp_2->setMaximum(p2->get_hp());
    ui->server_pet_hp_2->setValue(p2->get_fight_hp());
    ui->server_pet_name_b_2->setText(server_pet_name);
    ui->server_pet_hp_d_2->setText(QString::number(p2->get_fight_hp()) + "/"
                                   + QString::number(p2->get_hp()));
    on_vs_battle();
}

void lobby::on_vs_battle()
{
    ui->vs_battle_log->clear();
    update_user_pet_hp_vs(p1->get_fight_hp(), p1->get_hp());
    update_server_pet_hp_vs(p2->get_fight_hp(), p2->get_hp());

    attack_timer_user = new QTimer(this);
    attack_timer_server = new QTimer(this);

    connect(attack_timer_user, &QTimer::timeout, this, &lobby::user_attack_vs);
    connect(attack_timer_server, &QTimer::timeout, this, &lobby::server_attack_vs);

    // 连接战斗日志信号和槽
    connect(p1, &Pet::logGenerated, this, &lobby::onBattleLogReceived_vs);
    connect(p2, &Pet::logGenerated, this, &lobby::onBattleLogReceived_vs);

    attack_timer_user->start(static_cast<int>(p1->get_fight_attack_interval() * 1000));
    attack_timer_server->start(static_cast<int>(p2->get_fight_attack_interval() * 1000));
}

void lobby::onBattleLogReceived(const QString &log)
{
    // 将新的日志信息添加到QListWidget
    QListWidgetItem *item = new QListWidgetItem(log);
    ui->upgrade_battle_log->addItem(item);

    // 自动滚动到最新的日志信息
    ui->upgrade_battle_log->scrollToBottom();
}
void lobby::onBattleLogReceived_vs(const QString &log)
{
    // 将新的日志信息添加到QListWidget
    QListWidgetItem *item = new QListWidgetItem(log);
    ui->vs_battle_log->addItem(item);

    // 自动滚动到最新的日志信息
    ui->vs_battle_log->scrollToBottom();
}

void lobby::shakePetImage(QLabel *label)
{
    // 创建一个动画对象，用来改变 QLabel 的位置
    QPropertyAnimation *animation = new QPropertyAnimation(label, "pos");
    animation->setDuration(200);                                  // 设置震动的持续时间
    animation->setKeyValueAt(0, label->pos());                    // 起始位置
    animation->setKeyValueAt(0.25, label->pos() + QPoint(-5, 0)); // 向左移动
    animation->setKeyValueAt(0.5, label->pos());                  // 回到起始位置
    animation->setKeyValueAt(0.75, label->pos() + QPoint(5, 0));  // 向右移动
    animation->setKeyValueAt(1, label->pos());                    // 回到起始位置

    // 开始动画
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

void lobby::showResultMessage(int win, int experience)
{
    QMessageBox msgBox;
    msgBox.setWindowTitle("战斗结果");
    msgBox.setIcon(QMessageBox::Information);

    // 设置字体和字体大小
    QFont font = msgBox.font();
    font.setPointSize(14); // 设置字体大小为14，可以根据需要调整
    msgBox.setFont(font);
    if (win == 1) {
        QString message = "恭喜，你赢了！";
        message += "\n你的宠物获得了 " + QString::number(experience) + " 经验。";
        msgBox.setText(message);
    } else if (win == -1) {
        msgBox.setText("很遗憾，你输了。");
    }

    // 添加“OK”按钮
    QPushButton *okButton = msgBox.addButton(QMessageBox::Ok);

    // 设置默认按钮
    msgBox.setDefaultButton(okButton);

    msgBox.exec();

    // 处理“返回大厅(ok)”按钮点击
    if (msgBox.clickedButton() == okButton) {
        qDebug() << "返回大厅(ok)按钮点击";

        // 停止所有的计时器
        if (attack_timer_user) {
            attack_timer_user->stop();
        }
        if (attack_timer_server) {
            attack_timer_server->stop();
        }

        // 断开与宠物的日志连接（根据需要）
        if (p1) {
            disconnect(p1, &Pet::logGenerated, this, &lobby::onBattleLogReceived);
        }
        if (p2) {
            disconnect(p2, &Pet::logGenerated, this, &lobby::onBattleLogReceived);
        }

        // 切换到大厅界面
        lobby_page->setCurrentWidget(ui->lobby_2);

        // 清理战斗界面的状态（根据需要）
        ui->upgrade_battle_log->clear();

        qDebug() << "已切换回大厅";
    }
}

void lobby::showResultMessage_vs(int win, int experience)
{
    QMessageBox msgBox;
    msgBox.setWindowTitle("战斗结果");
    msgBox.setIcon(QMessageBox::Information);

    // 设置字体和字体大小
    QFont font = msgBox.font();
    font.setPointSize(14); // 设置字体大小为14，可以根据需要调整
    msgBox.setFont(font);
    if (win == 1) {
        QString message = "恭喜，你赢了！";
        message += "\n你的宠物获得了 " + QString::number(experience) + " 经验。";
        message += "\n你可以获得刚刚和你对战的宠物";
        msgBox.setText(message);
    } else if (win == -1) {
        msgBox.setText("很遗憾，你输了，你需要丢弃一个宠物");
    }

    // 添加“OK”按钮
    QPushButton *okButton = msgBox.addButton(QMessageBox::Ok);

    // 设置默认按钮
    msgBox.setDefaultButton(okButton);

    msgBox.exec();

    // 处理“返回大厅(ok)”按钮点击
    if (msgBox.clickedButton() == okButton) {
        qDebug() << "返回大厅(ok)按钮点击";

        // 停止所有的计时器
        if (attack_timer_user) {
            attack_timer_user->stop();
        }
        if (attack_timer_server) {
            attack_timer_server->stop();
        }

        // 断开与宠物的日志连接（根据需要）
        if (p1) {
            disconnect(p1, &Pet::logGenerated, this, &lobby::onBattleLogReceived_vs);
        }
        if (p2) {
            disconnect(p2, &Pet::logGenerated, this, &lobby::onBattleLogReceived_vs);
        }

        // 切换到大厅界面
        lobby_page->setCurrentWidget(ui->lobby_2);

        // 清理战斗界面的状态（根据需要）
        ui->vs_battle_log->clear();

        qDebug() << "已切换回大厅";
    }
}

void lobby::Abandon_pet(const QJsonArray &pet_array)
{
    // 创建一个对话框窗口
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("选择要丢弃的宠物");
    qDebug() << "pet_array size:" << pet_array.size();
    // 创建一个列表组件来展示宠物
    QListWidget *listWidget = new QListWidget(dialog);
    for (const QJsonValue &value : pet_array) {
        QJsonObject pet = value.toObject();
        qDebug() << "pet_id:" << pet["pet_id"].toInt()
                 << ", pet_name:" << pet["pet_name"].toString();
        QString petInfo
            = QString("ID: %1, Name: %2").arg(pet["pet_id"].toInt()).arg(pet["pet_name"].toString());
        listWidget->addItem(petInfo);
    }

    // 创建“丢弃”按钮
    QPushButton *abandonButton = new QPushButton("丢弃", dialog);

    // 布局管理
    QVBoxLayout *layout = new QVBoxLayout(dialog);
    layout->addWidget(listWidget);
    layout->addWidget(abandonButton);
    dialog->setLayout(layout);

    // 处理“丢弃”按钮点击事件
    connect(abandonButton, &QPushButton::clicked, [=]() {
        QListWidgetItem *selectedItem = listWidget->currentItem();
        if (selectedItem) {
            // 获取选中的宠物信息
            QString petInfo = selectedItem->text();
            QStringList parts = petInfo.split(",");
            int pet_id = parts[0].split(":")[1].trimmed().toInt();

            // 准备发送到服务器的JSON数据
            QJsonObject json;
            json["define"] = ABANDON;
            json["pet_id"] = pet_id;
            json["sockte_id"] = mo.socket_id;

            if (tcp_socket->isOpen()) {
                tcp_socket->write(QString(QJsonDocument(json).toJson()).toUtf8().data());
                tcp_socket->flush();
                qDebug() << "已发送失去宠物请求:" << json;
            } else {
                qDebug() << "套接字未连接，无法发送失去宠物请求";
            }

            // 显示确认信息
            QMessageBox::information(dialog, "宠物丢弃", "宠物已丢弃！");

            // 关闭对话框
            dialog->accept();
        } else {
            QMessageBox::warning(dialog, "未选择宠物", "请先选择一个宠物");
        }
    });

    // 显示对话框
    dialog->exec();
}

void lobby::on_battle_choose_tab_currentChanged() {}

void lobby::on_sign_out_to_login_clicked()
{
    qDebug() << "窗口即将关闭，执行登出操作";
    Signout(); // 执行登出操作
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
        login_1 = new login(tcp_socket);

        // 确保 lobby_1 是有效的
        if (login_1) {
            login_1->reconnect(); // 假设 reconnect 是一个有效的方法
            login_1->show();
            //lobby_1->update();
        } else {
            qDebug() << "lobby_1 对象无效";
        }
    });
}
