#ifndef LOBBY_H
#define LOBBY_H

#include "config.h"
//#include "login.h"
#include "manager.h"
#include "pets.h"
#include <QByteArray>
#include <QCloseEvent>
#include <QDebug>
#include <QGraphicsColorizeEffect>
#include <QGraphicsEffect>
#include <QIcon>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>
#include <QPainter>
#include <QPalette>
#include <QPixmap>
#include <QPropertyAnimation>
#include <QStackedWidget>
#include <QTcpServer>
#include <QTcpSocket> //通信套接字
#include <QTextBrowser>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

#include <QSqlError>
extern manager mo;

namespace Ui {
class lobby;
}

class login;

class lobby : public QWidget
{
    Q_OBJECT

public:
    explicit lobby(QTcpSocket *_tcp_socket, QWidget *parent = nullptr);
    ~lobby();

public slots:
    void receive_from_server(); //处理来自服务器的信息.
    void reconnect();         //重新连接
    void closeEvent(QCloseEvent *event);
    void Signout();
    void handle_userlist_info(const QJsonArray &userlist_array);
    void handle_pet_info(const QJsonArray &user_pets);
    void show_pet_details(QJsonObject);
    void handle_battle_choose(const QJsonArray &user_pets, const QJsonArray &server_user_pets);
    void choose_user_pet(QListWidgetItem *item);
    void choose_server_pet(QListWidgetItem *item);
    void handle_flush_server_pets(const QJsonArray &server_user_pets);
    void updateSelectionStatus();
    void Upgrade_battle(QJsonObject user_pet, QJsonObject server_pet);
    void displayPetImage(QLabel *label, const QString &imagePath);
    void onBattleLogReceived(const QString &log);
    void on_upgrade_battle();
    void update_user_pet_hp(int current_hp, int total_hp);
    void update_server_pet_hp(int current_hp, int total_hp);
    void update_user_pet_hp_vs(int current_hp, int total_hp);
    void update_server_pet_hp_vs(int current_hp, int total_hp);
    void shakePetImage(QLabel *label);
    void showResultMessage(int win, int experience);
    void Vs_battle(QJsonObject user_pet, QJsonObject server_pet);
    void on_vs_battle();
    void onBattleLogReceived_vs(const QString &log);
    void showResultMessage_vs(int win, int experience);
    void Abandon_pet(const QJsonArray &pet_array);

private slots:

    void on_switch_to_userlist_clicked();

    void on_switch_to_petbag_clicked();

    void on_switch_to_battle_clicked();

    void on_userlist_backto_lobby_clicked();

    void on_petbag_backto_lobby_clicked();

    void on_battle_choose_backto_lobby_clicked();

    void on_addpets_clicked();

    void on_recharge_button_clicked();

    void on_flush_serve_pets_clicked();

    void on_add_server_pets_clicked();

    void on_start_upgrade_battle_clicked();

    void on_start_vs_battle_clicked();

    void on_battle_choose_tab_currentChanged();

    void on_sign_out_to_login_clicked();

private:
    int money;
    int num_pets;
    int num_high_level_pets;
    Ui::lobby *ui;
    QTcpSocket *tcp_socket;
    QStackedWidget *lobby_page;
    bool user_pet_selected;
    bool server_pet_selected;
    Pet *p1;
    Pet *p2;
    QTimer *attack_timer_user;
    QTimer *attack_timer_server;
    // 槽函数用于处理定时器超时事件
    void user_attack();
    void server_attack();
    void user_attack_vs();
    void server_attack_vs();
    login *login_1 = nullptr;

signals:
    void logGenerated(const QString &log);
};

#endif // LOBBY_H
