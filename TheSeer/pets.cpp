
#include "pets.h"
#include <QRandomGenerator>
#include <QtDebug>

Pet::Pet(QObject *parent)
    : QObject(parent)
{
    level = 1;
    exp = 0;
    damage_point = 50;
    defense_point = 50;
    hp = 200;
    attack_interval = 2;
    is_evolved = 0;
    id = QRandomGenerator::global()->bounded(1, 101);
    is_miss = 0;                             //是否免疫伤害
    is_relife = 0;                           //是否复活
    is_dead = 0;                             //是否死亡
    fight_damage_point = damage_point;       //战斗时的战斗力
    fight_defense_point = defense_point;     //战斗时防御力
    fight_attack_interval = attack_interval; //战斗时攻击间隔
    fight_hp = hp;                           //战时生命值
    fight_damage_gain = 0;                   //战时战斗力增益次数
    fight_defense_gain = 0;                  //战时防御力增益次数
    fight_attack_interval_gain = 0;          //战时攻击间隔增益次数
    fight_damage_decrease = 0;               //战时攻击力削弱次数
    fight_defense_decrease = 0;              //战时护甲削弱次数
    fight_attack_interval_decrease = 0;      //战时攻击速度削弱次数
}

QString Pet::get_name()
{
    return name;
}

double Pet::get_hp()
{
    return hp;
}

double Pet::get_damage_point()
{
    return damage_point;
}

double Pet::get_defense_point()
{
    return defense_point;
}

double Pet::get_attack_interval()
{
    return attack_interval;
}

double Pet::get_fight_damage_point()
{
    return fight_damage_point;
}

double Pet::get_fight_defense_point()
{
    return fight_defense_point;
}

double Pet::get_fight_hp()
{
    return fight_hp;
}

double Pet::get_fight_attack_interval()
{
    return fight_attack_interval;
}

int Pet::get_killed_exp(int level)
{
    int i = 1;
    for (i = 1; i <= level; i++) {
        i = i * 2;
    }
    return i;
}

int Pet::get_level()
{
    return level;
}

int Pet::get_fight_defense_decrease()
{
    return fight_defense_decrease;
}

int Pet::get_fight_defense_gain()
{
    return fight_defense_gain;
}

int Pet::get_fight_damage_gain()
{
    return fight_damage_gain;
}

int Pet::get_fight_damage_decrease()
{
    return fight_damage_decrease;
}

int Pet::get_fight_attack_interval_gain()
{
    return fight_attack_interval_gain;
}

int Pet::get_fight_attack_interval_decrease()
{
    return fight_attack_interval_decrease;
}

int Pet::get_is_miss()
{
    return is_miss;
}

void Pet::set_level(int value)
{
    level = value;
}

void Pet::set_damage_point(double value)
{
    damage_point = value;
}

void Pet::set_defense_point(double value)
{
    defense_point = value;
}

void Pet::set_hp(double value)
{
    hp = value;
}

void Pet::set_attack_interval(double value)
{
    attack_interval = value;
}

void Pet::set_exp(int self_level, int enemy_level)
{
    exp = exp + get_killed_exp(enemy_level);
    if (exp >= 2 * get_killed_exp(self_level)) {
        is_upgrade = 1;
    } else
        is_upgrade = 0;
}

void Pet::set_fight_damage_point(double value)
{
    fight_damage_point = value;
}

void Pet::set_fight_defense_point(double value)
{
    fight_defense_point = value;
}

void Pet::set_fight_hp(double value)
{
    fight_hp = value;
}

void Pet::set_fight_attack_interval(double value)
{
    fight_attack_interval = value;
}

void Pet::set_fight_defense_decrease(int value)
{
    fight_defense_decrease = value;
}

void Pet::set_fight_defense_gain(int value)
{
    fight_defense_gain = value;
}

void Pet::set_fight_damage_gain(int value)
{
    fight_damage_gain = value;
}

void Pet::set_fight_damage_decrease(int value)
{
    fight_damage_decrease = value;
}

void Pet::set_fight_attack_interval_gain(int value)
{
    fight_attack_interval_gain = value;
}

void Pet::set_fight_attack_interval_decrease(int value)
{
    fight_attack_interval_decrease = value;
}

void Pet::set_is_miss(int value)
{
    is_miss = value;
}

int Pet::check_is_evolved()
{
    if (level >= 6) {
        return 1;
    } else
        return 0;
}

int Pet::check_is_dead()
{
    if (is_relife == 0 && hp <= 0) {
        return 1;
    } else if (is_relife == 1 && hp <= 0) {
        is_relife = 0;
        return 0;
    } else
        return 0;
}

void attack_pet::upgrade()
{
    level++;
    hp = hp + 30;
    damage_point = damage_point + 25;
    defense_point = defense_point + 7;
    attack_interval = attack_interval - 0.06;
    fight_damage_point = damage_point;
    fight_defense_point = defense_point;
    fight_attack_interval = attack_interval;
    exp = 0;
    if (level >= 6) {
        is_evolved = 1;
    } else
        is_evolved = 0;
}

void sonic_dog::fury(Pet *enemy)
{
    double damage_value;
    QString log;
    if (fight_damage_gain > 5) {
        damage_value = fight_damage_point
                       * (1
                          - (enemy->get_fight_defense_point() / 100
                             + enemy->get_fight_defense_point()));
    } else {
        fight_damage_gain++;
        fight_damage_point = fight_damage_point + 0.1 * damage_point;
        damage_value = fight_damage_point
                       * (1
                          - (enemy->get_fight_defense_point()
                             / (100 + enemy->get_fight_defense_point())));
    }
    enemy->set_fight_hp(enemy->get_fight_hp() - damage_value);
    qDebug() << "音速犬使用了技能1 狂暴" << endl;
    qDebug() << "对方hp-" << damage_value << endl;
    log = "音速犬使用了技能1 狂暴,对方hp-" + QString::number(damage_value);
    emit logGenerated(log);
    qDebug() << log.toUtf8().constData();
}

void sonic_dog::bloodthirsty_strike(Pet *enemy)
{
    double damage_value;
    QString log;

    damage_value = fight_damage_point * 1.5;
    fight_hp = fight_hp + damage_value * 0.3;
    if (fight_hp >= hp) {
        fight_hp = hp;
    }
    enemy->set_fight_hp(enemy->get_fight_hp() - damage_value);
    qDebug() << "音速犬使用了技能2 嗜血一击治疗了自身HP+" << damage_value * 0.3 << endl;
    qDebug() << "对方hp-" << damage_value << endl;
    log = "音速犬使用了技能2 嗜血一击治疗了自身HP+" + QString::number(damage_value * 0.3)
          + " 对方hp-" + QString::number(damage_value);
    emit logGenerated(log);
}

void sonic_dog::one_shot(Pet *enemy)
{
    QString log;
    int instant_kill = QRandomGenerator::global()->bounded(1, 1001); //如果秒杀标志为34，秒杀对手
    if (instant_kill == 34) {
        enemy->set_fight_hp(0);
        qDebug() << "音速犬使用了技能3 致命一击 秒杀了对手" << endl;
        log = "音速犬使用了技能3 致命一击 秒杀了对手";
        emit logGenerated(log);
    } else
        qDebug() << "音速犬使用了技能3 致命一击但未奏效" << endl;
    log = "音速犬使用了技能3 致命一击但未奏效";
    emit logGenerated(log);
}

void sonic_dog::attack(Pet *enemy)
{
    int skill_selected;
    double damage_value;
    QString log;
    if (is_evolved == 1) {
        skill_selected = QRandomGenerator::global()->bounded(1, 5);
        if (skill_selected == 1) {
            if (enemy->get_is_miss() == 1) {
                enemy->set_is_miss(0);
                qDebug() << "音速犬使用了普通攻击" << endl;
                qDebug() << "对方miss" << endl;
                log = "音速犬使用了普通攻击,但对方miss了";
                emit logGenerated(log);
                return;
            } else {
                damage_value = fight_damage_point
                               * (1
                                  - (enemy->get_fight_defense_point()
                                     / (100 + enemy->get_fight_defense_point())));
                enemy->set_fight_hp(enemy->get_fight_hp() - damage_value);
                qDebug() << "音速犬使用了普通攻击" << endl;
                qDebug() << "对方hp-" << damage_value << endl;
                log = "音速犬使用了普通攻击,对方hp-" + QString::number(damage_value);
                emit logGenerated(log);
            }
        } else if (skill_selected == 2) {
            if (enemy->get_is_miss() == 1) {
                enemy->set_is_miss(0);
                qDebug() << "音速犬使用了技能1 狂暴" << endl;
                qDebug() << "对方miss" << endl;
                log = "音速犬使用了技能1 狂暴,但对方miss了";
                emit logGenerated(log);
                return;
            } else {
                fury(enemy);
            }

        } else if (skill_selected == 3) {
            if (enemy->get_is_miss() == 1) {
                enemy->set_is_miss(0);
                qDebug() << "音速犬使用了技能2 嗜血一击" << endl;
                qDebug() << "对方miss" << endl;
                log = "音速犬使用了技能2 嗜血一击,但对方miss了";
                emit logGenerated(log);
                return;
            } else
                bloodthirsty_strike(enemy);
        } else if (skill_selected == 4) {
            if (enemy->get_is_miss() == 1) {
                enemy->set_is_miss(0);
                qDebug() << "音速犬使用了技能3 致命一击" << endl;
                qDebug() << "对方miss" << endl;
                log = "音速犬使用了技能3 致命一击但对方miss";
                emit logGenerated(log);
                return;
            } else
                one_shot(enemy);
        }
    } else if (is_evolved == 0) {
        skill_selected = QRandomGenerator::global()->bounded(1, 4);
        if (skill_selected == 1) {
            if (enemy->get_is_miss() == 1) {
                enemy->set_is_miss(0);
                qDebug() << "音速犬使用了普通攻击" << endl;
                qDebug() << "对方miss" << endl;
                log = "音速犬使用了普通攻击,但对方miss了";
                emit logGenerated(log);
                return;
            } else {
                damage_value = fight_damage_point
                               * (1
                                  - (enemy->get_fight_defense_point()
                                     / (100 + enemy->get_fight_defense_point())));
                enemy->set_fight_hp(enemy->get_fight_hp() - damage_value);
                qDebug() << "音速犬使用了普通攻击" << endl;
                qDebug() << "对方hp-" << damage_value << endl;
                log = "音速犬使用了普通攻击,对方hp-" + QString::number(damage_value);
                emit logGenerated(log);
            }
        } else if (skill_selected == 2) {
            if (enemy->get_is_miss() == 1) {
                enemy->set_is_miss(0);
                qDebug() << "音速犬使用了技能1 狂暴" << endl;
                qDebug() << "对方miss" << endl;
                log = "音速犬使用了技能1 狂暴,但对方miss了";
                emit logGenerated(log);
                return;
            } else
                fury(enemy);
        } else if (skill_selected == 3) {
            if (enemy->get_is_miss() == 1) {
                enemy->set_is_miss(0);
                qDebug() << "音速犬使用了技能2 嗜血一击" << endl;
                qDebug() << "对方miss" << endl;
                log = "音速犬使用了技能2 嗜血一击,但对方miss了";
                emit logGenerated(log);
                return;
            } else
                bloodthirsty_strike(enemy);
        }
    }
}

void blaze_god_of_war::overthrow(Pet *enemy)
{
    double damage_value;
    QString log;
    int enemy_level = enemy->get_level();
    int level_difference = 0;
    if (enemy_level > level) {
        level_difference = enemy_level - level;
    }
    damage_value = fight_damage_point * (1.5 + 0.1 * enemy_level)
                   * (1
                      - (enemy->get_fight_defense_point()
                         / (100 + enemy->get_fight_defense_point())));
    enemy->set_fight_hp(enemy->get_fight_hp() - damage_value);
    qDebug() << "烈焰战神使用了技能1 以下克上" << endl;
    qDebug() << "对方hp-" << damage_value << endl;
    log = "烈焰战神使用了技能1 以下克上,对方hp-" + QString::number(damage_value);
    emit logGenerated(log);
}

void blaze_god_of_war::blazing_charge(Pet *enemy)
{
    double damage_value;
    QString log;
    damage_value = fight_damage_point * 3
                   * (1
                      - (enemy->get_fight_defense_point()
                         / (100 + enemy->get_fight_defense_point())));
    fight_hp = fight_hp - damage_value * 0.3;
    enemy->set_fight_hp(enemy->get_fight_hp() - damage_value);
    qDebug() << "烈焰战神使用了技能2 烈焰冲击" << endl;
    qDebug() << "对方hp-" << damage_value << endl;
    log = "烈焰战神使用了技能2 烈焰冲击,对方hp-" + QString::number(damage_value);
    emit logGenerated(log);
}

void blaze_god_of_war::the_monarch_arrive()
{
    QString log;
    if (fight_damage_gain > 4) {
        qDebug() << "烈焰战神使用了技能3 君王降临 但是使用次数已达上限" << endl;
        log = "烈焰战神使用了技能3 君王降临 但是使用次数已达上限";
        emit logGenerated(log);
        return;
    } else {
        fight_damage_gain++;
        fight_damage_point = fight_damage_point + damage_point * 0.1;
        fight_defense_point = fight_defense_point + defense_point * 0.1;
        qDebug() << "烈焰战神使用了技能3 君王降临 增加自身攻击力和护甲各10%" << endl;
        qDebug() << "自身增加了攻击力" << damage_point * 0.1 << endl;
        qDebug() << "自身增加了护甲" << defense_point * 0.1 << endl;
        log = "烈焰战神使用了技能3 君王降临 增加自身攻击力和护甲各10%, 自身增加了攻击力"
              + QString::number(damage_point * 0.1) + ",自身护甲增加"
              + QString::number(defense_point * 0.1);
    }
}

void blaze_god_of_war::attack(Pet *enemy)
{
    int skill_selected;
    double damage_value;
    QString log;
    if (is_evolved == 1) {
        skill_selected = QRandomGenerator::global()->bounded(1, 5);
        if (skill_selected == 1) {
            if (enemy->get_is_miss() == 1) {
                enemy->set_is_miss(0);
                qDebug() << "烈焰战神使用了普通攻击" << endl;
                qDebug() << "对方miss" << endl;
                log = "烈焰战神使用了普通攻击，但对方miss了";
                emit logGenerated(log);

                return;
            } else {
                damage_value = fight_damage_point
                               * (1
                                  - (enemy->get_fight_defense_point()
                                     / (100 + enemy->get_fight_defense_point())));
                enemy->set_fight_hp(enemy->get_fight_hp() - damage_value);
                qDebug() << "烈焰战神使用了普通攻击" << endl;
                qDebug() << "对方hp-" << damage_value << endl;
                log = "烈焰战神使用了普通攻击，对方hp-" + QString::number(damage_value);
                emit logGenerated(log);
            }
        } else if (skill_selected == 2) {
            if (enemy->get_is_miss() == 1) {
                enemy->set_is_miss(0);
                qDebug() << "烈焰战神使用了技能1 以下克上" << endl;
                qDebug() << "对方miss" << endl;
                log = "烈焰战神使用了技能1 以下克上，但对方miss了";
                emit logGenerated(log);
                return;
            } else
                overthrow(enemy);
        } else if (skill_selected == 3) {
            if (enemy->get_is_miss() == 1) {
                enemy->set_is_miss(0);
                qDebug() << "烈焰战神使用了技能2 烈焰冲击" << endl;
                qDebug() << "对方miss" << endl;
                log = "烈焰战神使用了技能2 烈焰冲击，但对方miss了";
                emit logGenerated(log);
                return;
            } else
                blazing_charge(enemy);
        } else if (skill_selected == 4) {
            the_monarch_arrive();
        }
    } else if (is_evolved == 0) {
        skill_selected = QRandomGenerator::global()->bounded(1, 4);
        if (skill_selected == 1) {
            if (enemy->get_is_miss() == 1) {
                enemy->set_is_miss(0);
                qDebug() << "烈焰战神使用了普通攻击" << endl;
                qDebug() << "对方miss" << endl;
                log = "烈焰战神使用了普通攻击，但对方miss了";
                emit logGenerated(log);
                return;
            } else {
                damage_value = fight_damage_point
                               * (1
                                  - (enemy->get_fight_defense_point()
                                     / (100 + enemy->get_fight_defense_point())));
                enemy->set_fight_hp(enemy->get_fight_hp() - damage_value);
                qDebug() << "烈焰战神使用了普通攻击" << endl;
                qDebug() << "对方hp-" << damage_value << endl;
                log = "烈焰战神使用了普通攻击，对方hp-" + QString::number(damage_value);
                emit logGenerated(log);
            }
        } else if (skill_selected == 2) {
            if (enemy->get_is_miss() == 1) {
                enemy->set_is_miss(0);
                qDebug() << "烈焰战神使用了技能1 以下克上" << endl;
                qDebug() << "对方miss" << endl;
                log = "烈焰战神使用了技能1 以下克上,但对方miss了";
                emit logGenerated(log);
                return;
            } else
                overthrow(enemy);
        } else if (skill_selected == 3) {
            if (enemy->get_is_miss() == 1) {
                enemy->set_is_miss(0);
                qDebug() << "烈焰战神使用了技能2 烈焰冲击" << endl;
                qDebug() << "对方miss" << endl;
                log = "烈焰战神使用了技能2 烈焰冲击,但对方miss了";
                emit logGenerated(log);
                return;
            } else
                blazing_charge(enemy);
        }
    }
}

void high_hp_pet::upgrade()
{
    level++;
    hp = hp + 75;
    damage_point = damage_point + 8;
    defense_point = defense_point + 10;
    attack_interval = attack_interval - 0.03;
    fight_damage_point = damage_point;
    fight_defense_point = defense_point;
    fight_attack_interval = attack_interval;
    exp = 0;
    if (level >= 6) {
        is_evolved = 1;
    } else
        is_evolved = 0;
}

void black_bear_berserk::recovery()
{
    QString log;
    fight_hp = fight_hp + hp * 0.15;
    qDebug() << "黑熊狂战使用了技能1 复苏 治疗自身" << endl;
    qDebug() << "自身回复了hp" << hp * 0.15 << endl;
    log = "黑熊狂战使用了技能1 复苏 治疗自身，自身回复了hp" + QString::number(hp * 0.15);
    emit logGenerated(log);
}

void black_bear_berserk::eye_for_eye(Pet *enemy)
{
    double damage_value;
    damage_value = fight_hp * 0.2;
    fight_hp = fight_hp - fight_hp * 0.08;
    enemy->set_fight_hp(enemy->get_fight_hp() - damage_value);
    qDebug() << "黑熊狂战使用了技能2 以眼还眼 伤害敌方并对自身造成一定伤害" << endl;
    qDebug() << "对方hp-" << damage_value << endl;
    qDebug() << "自身hp-" << fight_hp * 0.08 << endl;
    QString log;
    log = "黑熊狂战使用了技能2 以眼还眼 伤害敌方并对自身造成一定伤害,对方hp-"
          + QString::number(damage_value) + " 自身hp-" + QString::number(fight_hp * 0.08);
    emit logGenerated(log);
}

void black_bear_berserk::guardian_wings()
{
    QString log;
    int is_guardian_wings = QRandomGenerator::global()->bounded(1, 1001);
    if (is_guardian_wings == 3 || is_guardian_wings == 8 || is_guardian_wings == 30
        || is_guardian_wings == 31 || is_guardian_wings == 34) {
        fight_hp = hp;
        qDebug() << "黑熊狂战使用了技能3 守护者之翼 千分之五的概率回满血" << endl;
        qDebug() << "守护者之翼 启动" << endl;

        log = "黑熊狂战使用了技能3 守护者之翼 千分之五的概率回满血 守护者之翼 启动";
        emit logGenerated(log);
    } else {
        qDebug() << "黑熊狂战使用了技能3 守护者之翼 千分之五的概率回满血" << endl;
        qDebug() << "守护者之翼 miss" << endl;
        log = "黑熊狂战使用了技能3 守护者之翼 千分之五的概率回满血 守护者之翼 miss";
        emit logGenerated(log);
    }
}

void black_bear_berserk::attack(Pet *enemy)
{
    QString log;
    int skill_selected;
    double damage_value;
    if (is_evolved == 1) {
        skill_selected = QRandomGenerator::global()->bounded(1, 5);
        if (skill_selected == 1) {
            if (enemy->get_is_miss() == 1) {
                enemy->set_is_miss(0);
                qDebug() << "黑熊狂战使用了普通攻击" << endl;
                qDebug() << "对方miss" << endl;
                log = "黑熊狂战使用了普通攻击,但对方miss了";
                emit logGenerated(log);
                return;
            } else {
                damage_value = fight_damage_point
                               * (1
                                  - (enemy->get_fight_defense_point()
                                     / (100 + enemy->get_fight_defense_point())));
                enemy->set_fight_hp(enemy->get_fight_hp() - damage_value);
                qDebug() << "黑熊狂战使用了普通攻击" << endl;
                qDebug() << "对方hp-" << damage_value << endl;
                log = "黑熊狂战使用了普通攻击,对方hp-" + QString::number(damage_value);
                emit logGenerated(log);
            }
        } else if (skill_selected == 2) {
            recovery();
        } else if (skill_selected == 3) {
            if (enemy->get_is_miss() == 1) {
                enemy->set_is_miss(0);
                qDebug() << "黑熊狂战使用了技能2 以眼还眼" << endl;
                qDebug() << "对方miss" << endl;
                log = "黑熊狂战使用了技能2 以眼还眼，但对方miss";
                emit logGenerated(log);
                return;
            } else
                eye_for_eye(enemy);
        } else if (skill_selected == 4) {
            guardian_wings();
        }
    } else if (is_evolved == 0) {
        skill_selected = QRandomGenerator::global()->bounded(1, 4);
        if (skill_selected == 1) {
            if (enemy->get_is_miss() == 1) {
                enemy->set_is_miss(0);
                qDebug() << "黑熊狂战使用了普通攻击" << endl;
                qDebug() << "对方miss" << endl;
                log = "黑熊狂战使用了普通攻击,但对方miss了";
                emit logGenerated(log);
                return;
            } else {
                damage_value = fight_damage_point
                               * (1
                                  - (enemy->get_fight_defense_point()
                                     / (100 + enemy->get_fight_defense_point())));
                enemy->set_fight_hp(enemy->get_fight_hp() - damage_value);
                qDebug() << "黑熊狂战使用了普通攻击" << endl;
                qDebug() << "对方hp-" << damage_value << endl;
                log = "黑熊狂战使用了普通攻击,对方hp-" + QString::number(damage_value);
                emit logGenerated(log);
            }
        } else if (skill_selected == 2) {
            recovery();
        } else if (skill_selected == 3) {
            if (enemy->get_is_miss() == 1) {
                enemy->set_is_miss(0);
                qDebug() << "黑熊狂战使用了技能2 以眼还眼" << endl;
                qDebug() << "对方miss" << endl;
                log = "黑熊狂战使用了技能2 以眼还眼，但对方miss";
                emit logGenerated(log);
                return;
            } else
                eye_for_eye(enemy);
        }
    }
}

void ares::heal()
{
    QString log;
    double lost_hp = hp - fight_hp;
    fight_hp = fight_hp + lost_hp * 0.3;
    qDebug() << "阿瑞斯使用了技能1 治疗术 回复自身hp" << endl;
    qDebug() << "自身hp+" << lost_hp * 0.3 << endl;
    log = "阿瑞斯使用了技能1 治疗术 回复自身hp" + QString::number(lost_hp * 0.3);
    emit logGenerated(log);
}

void ares::earthquake(Pet *enemy)
{
    QString log;
    double damage_value;
    damage_value = fight_hp * 0.2;
    enemy->set_fight_hp(enemy->get_fight_hp() - damage_value);
    qDebug() << "阿瑞斯使用了技能2 山崩地裂 对敌方造成伤害" << endl;
    qDebug() << "对方hp-" << damage_value << endl;
    log = "阿瑞斯使用了技能2 山崩地裂 对敌方造成伤害 对方hp-" + QString::number(damage_value);
    emit logGenerated(log);
}

void ares::final_radiance(Pet *enemy)
{
    QString log;
    double damage_value;
    double lost_hp = hp - fight_hp;
    damage_value = lost_hp * 0.5;
    enemy->set_fight_hp(enemy->get_fight_hp() - damage_value);
    qDebug() << "阿瑞斯使用了技能3 回光返照 对敌方造成伤害" << endl;
    qDebug() << "对方hp-" << damage_value << endl;
    log = "阿瑞斯使用了技能3 回光返照 对敌方造成伤害 对方hp-" + QString::number(damage_value);
    emit logGenerated(log);
}

void ares::attack(Pet *enemy)
{
    QString log;
    int skill_selected;
    double damage_value;
    if (is_evolved == 1) {
        skill_selected = QRandomGenerator::global()->bounded(1, 5);
        if (skill_selected == 1) {
            if (enemy->get_is_miss() == 1) {
                enemy->set_is_miss(0);
                qDebug() << "阿瑞斯使用了普通攻击" << endl;
                qDebug() << "对方miss" << endl;
                log = "阿瑞斯使用了普通攻击,但对方miss了";
                emit logGenerated(log);
                return;
            } else {
                damage_value = fight_damage_point
                               * (1
                                  - (enemy->get_fight_defense_point()
                                     / (100 + enemy->get_fight_defense_point())));
                enemy->set_fight_hp(enemy->get_fight_hp() - damage_value);
                qDebug() << "阿瑞斯使用了普通攻击" << endl;
                qDebug() << "对方hp-" << damage_value << endl;
                log = "阿瑞斯使用了普通攻击,对方hp-" + QString::number(damage_value);
                emit logGenerated(log);
            }
        } else if (skill_selected == 2) {
            heal();
        } else if (skill_selected == 3) {
            if (enemy->get_is_miss() == 1) {
                enemy->set_is_miss(0);
                qDebug() << "阿瑞斯使用了技能2 山崩地裂" << endl;
                qDebug() << "对方miss" << endl;
                log = "阿瑞斯使用了技能2 山崩地裂 对方miss";
                emit logGenerated(log);
                return;
            } else
                earthquake(enemy);
        } else if (skill_selected == 4) {
            if (enemy->get_is_miss() == 1) {
                enemy->set_is_miss(0);
                qDebug() << "阿瑞斯使用了技能3 回光返照" << endl;
                qDebug() << "对方miss" << endl;
                log = "阿瑞斯使用了技能3 回光返照 对方miss";
                emit logGenerated(log);
                return;
            } else
                final_radiance(enemy);
        }
    } else if (is_evolved == 0) {
        skill_selected = QRandomGenerator::global()->bounded(1, 4);
        if (skill_selected == 1) {
            if (enemy->get_is_miss() == 1) {
                enemy->set_is_miss(0);
                qDebug() << "阿瑞斯使用了普通攻击" << endl;
                qDebug() << "对方miss" << endl;
                log = "阿瑞斯使用了普通攻击,但对方miss了";
                emit logGenerated(log);
                return;
            } else {
                damage_value = fight_damage_point
                               * (1
                                  - (enemy->get_fight_defense_point()
                                     / (100 + enemy->get_fight_defense_point())));
                enemy->set_fight_hp(enemy->get_fight_hp() - damage_value);
                qDebug() << "阿瑞斯使用了普通攻击" << endl;
                qDebug() << "对方hp-" << damage_value << endl;
                log = "阿瑞斯使用了普通攻击,对方hp-" + QString::number(damage_value);
                emit logGenerated(log);
            }
        } else if (skill_selected == 2) {
            heal();
        } else if (skill_selected == 3) {
            if (enemy->get_is_miss() == 1) {
                enemy->set_is_miss(0);
                qDebug() << "阿瑞斯使用了技能2 山崩地裂" << endl;
                qDebug() << "对方miss" << endl;
                log = "阿瑞斯使用了技能2 山崩地裂 对方miss";
                emit logGenerated(log);
                return;
            } else
                earthquake(enemy);
        }
    }
}

void defense_pet::upgrade()
{
    level++;
    hp = hp + 60;
    damage_point = damage_point + 7;
    defense_point = defense_point + 15;
    attack_interval = attack_interval - 0.03;
    fight_damage_point = damage_point;
    fight_defense_point = defense_point;
    fight_attack_interval = attack_interval;
    exp = 0;
    if (level >= 6) {
        is_evolved = 1;
    } else
        is_evolved = 0;
}

void rock_armor_lord::solid_layer()
{
    QString log;
    if (fight_defense_gain > 9) {
        return;
    } else {
        defense_point += 10;
        fight_defense_gain++;
        qDebug() << "岩铠领主使用了技能1 坚实表皮 增加自身护甲" << endl;
        qDebug() << "自身护甲+10" << endl;
        log = "岩铠领主使用了技能1 坚实表皮 增加自身护甲" + QString::number(10);
        emit logGenerated(log);
    }
}

void rock_armor_lord::shield_bash(Pet *enemy)
{
    QString log;
    double damage_value;
    damage_value = fight_damage_point
                       * (1
                          - (enemy->get_fight_defense_point()
                             / (100 + enemy->get_fight_defense_point())))
                   + fight_defense_point * 0.1;
    enemy->set_fight_hp(enemy->get_fight_hp() - damage_value);
    qDebug() << "岩铠领主使用了技能2 盾击 对敌方造成伤害" << endl;
    qDebug() << "敌方hp-" << damage_value << endl;
    log = "岩铠领主使用了技能2 盾击 对敌方造成伤害 敌方hp-" + QString::number(damage_value);
    emit logGenerated(log);
}

void rock_armor_lord::taunt(Pet *enemy)
{
    QString log;
    if (enemy->get_fight_defense_decrease() > 5) {
        qDebug() << "岩铠领主使用了技能3 嘲讽 但是已达上限次数" << endl;
        log = "岩铠领主使用了技能3 嘲讽 但是已达上限次数";
        emit logGenerated(log);
        return;
    } else {
        enemy->set_fight_defense_decrease(enemy->get_fight_defense_decrease() + 1);
        enemy->set_fight_defense_point(enemy->get_fight_defense_point() - 15);
        qDebug() << "岩铠领主使用了技能3 嘲讽 削弱对方护甲" << endl;
        qDebug() << "敌方护甲-15" << endl;
        log = "岩铠领主使用了技能3 嘲讽 削弱对方护甲 敌方护甲-15";
        emit logGenerated(log);
    }
}

void rock_armor_lord::attack(Pet *enemy)
{
    QString log;
    int skill_selected;
    double damage_value;
    if (is_evolved == 1) {
        skill_selected = QRandomGenerator::global()->bounded(1, 5);
        if (skill_selected == 1) {
            if (enemy->get_is_miss() == 1) {
                enemy->set_is_miss(0);
                qDebug() << "岩铠领主使用了普通攻击" << endl;
                qDebug() << "对方miss" << endl;
                log = "岩铠领主使用了普通攻击,对方miss";
                emit logGenerated(log);
                return;
            } else {
                damage_value = fight_damage_point
                               * (1
                                  - (enemy->get_fight_defense_point()
                                     / (100 + enemy->get_fight_defense_point())));
                enemy->set_fight_hp(enemy->get_fight_hp() - damage_value);
                qDebug() << "岩铠领主使用了普通攻击" << endl;
                qDebug() << "对方hp-" << damage_value << endl;
                log = "岩铠领主使用了普通攻击,对方hp-" + QString::number(damage_value);
                emit logGenerated(log);
            }
        } else if (skill_selected == 2) {
            solid_layer();
        } else if (skill_selected == 3) {
            if (enemy->get_is_miss() == 1) {
                enemy->set_is_miss(0);
                qDebug() << "岩铠领主使用了技能2 盾击" << endl;
                qDebug() << "敌人miss" << endl;
                log = "岩铠领主使用了技能2 盾击 敌人miss";
                emit logGenerated(log);
            } else
                shield_bash(enemy);
        } else if (skill_selected == 4) {
            if (enemy->get_is_miss() == 1) {
                enemy->set_is_miss(0);
                qDebug() << "岩铠领主使用了技能3 嘲讽" << endl;
                qDebug() << "敌人miss" << endl;
                log = "岩铠领主使用了技能3 嘲讽 敌人miss";
                emit logGenerated(log);
            } else
                taunt(enemy);
        }
    } else if (is_evolved == 0) {
        skill_selected = QRandomGenerator::global()->bounded(1, 4);
        if (skill_selected == 1) {
            if (enemy->get_is_miss() == 1) {
                enemy->set_is_miss(0);
                qDebug() << "岩铠领主使用了普通攻击" << endl;
                qDebug() << "对方miss" << endl;
                log = "岩铠领主使用了普通攻击,对方miss";
                emit logGenerated(log);
                return;
            } else {
                damage_value = fight_damage_point
                               * (1
                                  - (enemy->get_fight_defense_point()
                                     / (100 + enemy->get_fight_defense_point())));
                enemy->set_fight_hp(enemy->get_fight_hp() - damage_value);
                qDebug() << "岩铠领主使用了普通攻击" << endl;
                qDebug() << "对方hp-" << damage_value << endl;
                log = "岩铠领主使用了普通攻击,对方hp-" + QString::number(damage_value);
                emit logGenerated(log);
            }
        } else if (skill_selected == 2) {
            solid_layer();
        } else if (skill_selected == 3) {
            if (enemy->get_is_miss() == 1) {
                enemy->set_is_miss(0);
                qDebug() << "岩铠领主使用了技能2 盾击" << endl;
                qDebug() << "敌人miss" << endl;
                log = "岩铠领主使用了技能2 盾击 敌人miss";
                emit logGenerated(log);
            } else
                shield_bash(enemy);
        }
    }
}

void amber_knight::defense_stance()
{
    QString log;
    if (fight_defense_gain > 4) {
        qDebug() << "琥珀骑士使用了技能1 防御姿态 但是已达上限次数" << endl;
        log = "琥珀骑士使用了技能1 防御姿态 但是已达上限次数";
        emit logGenerated(log);
        return;
    } else {
        fight_defense_gain++;
        fight_defense_point = fight_defense_point + defense_point * 0.1;
        qDebug() << "琥珀骑士使用了技能1 防御姿态 增加自身护甲" << endl;
        qDebug() << "自身护甲+" << defense_point * 0.1 << endl;
        log = "琥珀骑士使用了技能1 防御姿态 增加自身护甲" + QString::number(defense_point * 0.1);
        emit logGenerated(log);
    }
}

void amber_knight::smashed_to_bits(Pet *enemy)
{
    QString log;
    if (enemy->get_fight_defense_decrease() > 4) {
        qDebug() << "琥珀骑士使用了技能2 粉身碎骨 但是已达上限次数" << endl;
        log = "琥珀骑士使用了技能2 粉身碎骨 但是已达上限次数";
        emit logGenerated(log);
        return;
    } else {
        enemy->set_fight_defense_decrease(enemy->get_fight_defense_decrease() + 1);
        enemy->set_fight_defense_point(enemy->get_fight_defense_point()
                                       - enemy->get_defense_point() * 0.06);
        qDebug() << "琥珀骑士使用了技能2 粉身碎骨 削弱敌人护甲" << endl;
        qDebug() << "敌人护甲-" << enemy->get_defense_point() * 0.06 << endl;
        log = "琥珀骑士使用了技能2 粉身碎骨 削弱敌人护甲 敌人护甲-"
              + QString::number(enemy->get_defense_point() * 0.06);
        emit logGenerated(log);
    }
}

void amber_knight::cycle_of_desperation(Pet *enemy)
{
    QString log;
    int is_skill3 = QRandomGenerator::global()->bounded(1, 101);
    if (is_skill3 >= 20 && is_skill3 <= 35) {
        enemy->set_fight_defense_gain(0);
        enemy->set_fight_damage_gain(0);
        enemy->set_fight_attack_interval_gain(0);
        if (enemy->get_fight_defense_point() > enemy->get_defense_point()) {
            enemy->set_fight_defense_point(enemy->get_defense_point());
        }
        if (enemy->get_fight_damage_point() > enemy->get_damage_point()) {
            enemy->set_fight_damage_point(enemy->get_damage_point());
        }
        if (enemy->get_fight_attack_interval() < enemy->get_attack_interval()) {
            enemy->set_fight_attack_interval(enemy->get_attack_interval());
        }
        qDebug() << "琥珀骑士使用了技能3 轮回绝境 百分之15的概率让敌人所有增益消失" << endl;
        qDebug() << "轮回绝境 启动" << endl;
        log = "琥珀骑士使用了技能3 轮回绝境 百分之15的概率让敌人所有增益消失，轮回绝境 启动";
        emit logGenerated(log);
    } else {
        qDebug() << "琥珀骑士使用了技能3 轮回绝境 百分之15的概率让敌人所有增益消失" << endl;
        qDebug() << "轮回绝境 miss" << endl;
        log = "琥珀骑士使用了技能3 轮回绝境 百分之15的概率让敌人所有增益消失，轮回绝境 miss";
        emit logGenerated(log);
    }
}

void amber_knight::attack(Pet *enemy)
{
    QString log;
    int skill_selected;
    double damage_value;
    if (is_evolved == 1) {
        skill_selected = QRandomGenerator::global()->bounded(1, 5);
        if (skill_selected == 1) {
            if (enemy->get_is_miss() == 1) {
                enemy->set_is_miss(0);
                qDebug() << "琥珀骑士使用了普通攻击" << endl;
                qDebug() << "对方miss" << endl;
                log = "琥珀骑士使用了普通攻击 对方miss";
                emit logGenerated(log);
                return;
            } else {
                damage_value = fight_damage_point
                               * (1
                                  - (enemy->get_fight_defense_point()
                                     / (100 + enemy->get_fight_defense_point())));
                enemy->set_fight_hp(enemy->get_fight_hp() - damage_value);
                qDebug() << "琥珀骑士使用了普通攻击" << endl;
                qDebug() << "对方hp-" << damage_value << endl;
                log = "琥珀骑士使用了普通攻击 对方hp-" + QString::number(damage_value);
                emit logGenerated(log);
            }
        } else if (skill_selected == 2) {
            defense_stance();
        } else if (skill_selected == 3) {
            if (enemy->get_is_miss() == 1) {
                enemy->set_is_miss(0);
                qDebug() << "琥珀骑士使用了技能2 粉身碎骨 削弱敌人护甲" << endl;
                qDebug() << "敌人miss" << endl;
                log = "琥琥珀骑士使用了技能2 粉身碎骨 削弱敌人护甲 敌人miss";
                emit logGenerated(log);
            } else
                smashed_to_bits(enemy);
        } else if (skill_selected == 4) {
            if (enemy->get_is_miss() == 1) {
                enemy->set_is_miss(0);
                qDebug() << "琥珀骑士使用了技能3 轮回绝境 解除敌人所有增益效果" << endl;
                qDebug() << "敌人miss" << endl;
                log = "琥珀骑士使用了技能3 轮回绝境 解除敌人所有增益效果 敌人miss";
                emit logGenerated(log);
            } else
                cycle_of_desperation(enemy);
        }
    } else if (is_evolved == 0) {
        skill_selected = QRandomGenerator::global()->bounded(1, 4);
        if (skill_selected == 1) {
            if (enemy->get_is_miss() == 1) {
                enemy->set_is_miss(0);
                qDebug() << "琥珀骑士使用了普通攻击" << endl;
                qDebug() << "对方miss" << endl;
                log = "琥珀骑士使用了普通攻击 对方miss";
                emit logGenerated(log);
                return;
            } else {
                damage_value = fight_damage_point
                               * (1
                                  - (enemy->get_fight_defense_point()
                                     / (100 + enemy->get_fight_defense_point())));
                enemy->set_fight_hp(enemy->get_fight_hp() - damage_value);
                qDebug() << "琥珀骑士使用了普通攻击" << endl;
                qDebug() << "对方hp-" << damage_value << endl;
                log = "琥珀骑士使用了普通攻击 对方hp-" + QString::number(damage_value);
                emit logGenerated(log);
            }
        } else if (skill_selected == 2) {
            defense_stance();
        } else if (skill_selected == 3) {
            if (enemy->get_is_miss() == 1) {
                enemy->set_is_miss(0);
                qDebug() << "琥珀骑士使用了技能2 粉身碎骨 削弱敌人护甲" << endl;
                qDebug() << "敌人miss" << endl;
                log = "琥琥珀骑士使用了技能2 粉身碎骨 削弱敌人护甲 敌人miss";
                emit logGenerated(log);
            } else
                smashed_to_bits(enemy);
        }
    }
}

void agile_pet::upgrade()
{
    level++;
    hp = hp + 30;
    damage_point = damage_point + 15;
    defense_point = defense_point + 5;
    attack_interval = attack_interval - 0.05;
    fight_damage_point = damage_point;
    fight_defense_point = defense_point;
    fight_attack_interval = attack_interval;
    exp = 0;
    if (level >= 6) {
        is_evolved = 1;
    } else
        is_evolved = 0;
}

void swift_quinn::combo_attack(Pet *enemy)
{
    QString log;
    double damage_value = 2 * fight_damage_point
                          * (1
                             - (enemy->get_fight_defense_point()
                                / (100 + enemy->get_fight_defense_point())));

    enemy->set_fight_hp(enemy->get_fight_hp() - damage_value);
    qDebug() << "疾驰奎因使用了技能1 连砍 快速攻击两次敌人" << endl;
    qDebug() << "敌人hp-" << damage_value << endl;
    log = "疾驰奎因使用了技能1 连砍 快速攻击两次敌人 敌人hp-" + QString::number(damage_value);
    emit logGenerated(log);
}

void swift_quinn::deadly_rhythm()
{
    QString log;
    if (fight_attack_interval_gain > 9) {
        qDebug() << "疾驰奎因使用了技能2 致命节奏 增加自身攻击速度" << endl;
        qDebug() << "使用次数已达上限" << endl;
        log = "疾驰奎因使用了技能2 致命节奏 增加自身攻击速度，但是使用次数已达上限";
        emit logGenerated(log);
        return;
    } else {
        fight_attack_interval_gain++;
        fight_attack_interval -= 0.05;
        qDebug() << "疾驰奎因使用了技能2 致命节奏 增加自身攻击速度" << endl;
        qDebug() << "自身攻击速度增快0.05s" << endl;
        log = "疾驰奎因使用了技能2 致命节奏 增加自身攻击速度 自身攻击速度增快0.05s";
        emit logGenerated(log);
    }
}

void swift_quinn::a_narrow_escape()
{
    QString log;
    int is_skill3 = QRandomGenerator::global()->bounded(1, 11);
    if (is_skill3 == 5 || is_skill3 == 2 || is_skill3 == 1) {
        is_miss = 1;
        log = "疾驰奎因使用了技能千钧一发 十分之三的概率闪避下一次攻击 下次有效";
        emit logGenerated(log);
    }
    log = "疾驰奎因使用了技能千钧一发 十分之三的概率闪避下一次攻击 无效";
    emit logGenerated(log);
}

void swift_quinn::attack(Pet *enemy)
{
    QString log;
    int skill_selected;
    double damage_value;
    if (is_evolved == 1) {
        skill_selected = QRandomGenerator::global()->bounded(1, 5);
        if (skill_selected == 1) {
            if (enemy->get_is_miss() == 1) {
                enemy->set_is_miss(0);
                qDebug() << "疾驰奎因使用了普通攻击" << endl;
                qDebug() << "对方miss" << endl;
                log = "疾驰奎因使用了普通攻击 对方miss";
                emit logGenerated(log);
                return;
            } else {
                damage_value = fight_damage_point
                               * (1
                                  - (enemy->get_fight_defense_point()
                                     / (100 + enemy->get_fight_defense_point())));
                enemy->set_fight_hp(enemy->get_fight_hp() - damage_value);
                qDebug() << "疾驰奎因使用了普通攻击" << endl;
                qDebug() << "对方hp-" << damage_value << endl;
                log = "疾驰奎因使用了普通攻击 对方hp-" + QString::number(damage_value);
                emit logGenerated(log);
            }

        } else if (skill_selected == 2) {
            if (enemy->get_is_miss() == 1) {
                enemy->set_is_miss(0);
                qDebug() << "疾驰奎因使用了技能1 连砍 快速攻击敌人两次" << endl;
                qDebug() << "对方miss" << endl;
                log = "疾驰奎因使用了技能1 连砍 快速攻击敌人两次 对方miss";
                emit logGenerated(log);
                return;
            } else
                combo_attack(enemy);
        } else if (skill_selected == 3) {
            deadly_rhythm();
        } else if (skill_selected == 4) {
            a_narrow_escape();
        }
    } else if (is_evolved == 0) {
        skill_selected = QRandomGenerator::global()->bounded(1, 4);
        if (skill_selected == 1) {
            if (enemy->get_is_miss() == 1) {
                enemy->set_is_miss(0);
                qDebug() << "疾驰奎因使用了普通攻击" << endl;
                qDebug() << "对方miss" << endl;
                log = "疾驰奎因使用了普通攻击 对方miss";
                emit logGenerated(log);
                return;
            } else {
                damage_value = fight_damage_point
                               * (1
                                  - (enemy->get_fight_defense_point()
                                     / (100 + enemy->get_fight_defense_point())));
                enemy->set_fight_hp(enemy->get_fight_hp() - damage_value);
                qDebug() << "疾驰奎因使用了普通攻击" << endl;
                qDebug() << "对方hp-" << damage_value << endl;
                log = "疾驰奎因使用了普通攻击 对方hp-" + QString::number(damage_value);
                emit logGenerated(log);
            }
        } else if (skill_selected == 2) {
            if (enemy->get_is_miss() == 1) {
                enemy->set_is_miss(0);
                qDebug() << "疾驰奎因使用了技能1 连砍 快速攻击敌人两次" << endl;
                qDebug() << "对方miss" << endl;
                log = "疾驰奎因使用了技能1 连砍 快速攻击敌人两次 对方miss";
                emit logGenerated(log);
                return;
            } else
                combo_attack(enemy);
        } else if (skill_selected == 3) {
            deadly_rhythm();
        }
    }
}

void dimo::penetrate(Pet *enemy)
{
    QString log;
    if (enemy->get_fight_attack_interval_decrease() > 4) {
        qDebug() << "迪莫使用了技能1 识破 削弱敌人攻击速度" << endl;
        qDebug() << "技能使用次数已达上限" << endl;
        log = "迪莫使用了技能1 识破 削弱敌人攻击速度,但是技能使用次数已达上限";
        emit logGenerated(log);
    } else {
        enemy->set_fight_attack_interval_decrease(enemy->get_fight_attack_interval_decrease() + 1);
        enemy->set_fight_attack_interval(enemy->get_fight_attack_interval()
                                         - enemy->get_attack_interval() * 0.1);
        qDebug() << "迪莫使用了技能1 识破 削弱敌人攻击速度" << endl;
        qDebug() << "敌人攻速下降" << enemy->get_attack_interval() * 0.1 << endl;
        log = "迪莫使用了技能1 识破 削弱敌人攻击速度,敌人攻速下降"
              + QString::number(enemy->get_attack_interval() * 0.1);
        emit logGenerated(log);
    }
}

void dimo::swift_stride(Pet *enemy)
{
    QString log;
    double damage_value;
    damage_value = 1.5 * fight_damage_point
                   * (1
                      - (enemy->get_fight_defense_point()
                         / (100 + enemy->get_fight_defense_point())));
    enemy->set_fight_hp(enemy->get_fight_hp() - damage_value);
    fight_hp += damage_value * 0.5;
    qDebug() << "迪莫使用了技能2 迅捷步伐 对敌人造成伤害治疗自身" << endl;
    qDebug() << "对方hp-" << damage_value << endl;
    qDebug() << "自身hp+" << damage_value * 0.5 << endl;
    log = "迪莫使用了技能2 迅捷步伐 对敌人造成伤害治疗自身 对方hp-" + QString::number(damage_value)
          + "自身治疗" + QString::number(damage_value * 0.5);
    emit logGenerated(log);
}

void dimo::storm_gathering(Pet *enemy)
{
    QString log;
    double damage_value;
    damage_value = (1.5 + (level - 5) * 0.1) * fight_damage_point
                   * (1
                      - (enemy->get_fight_defense_point()
                         / (100 + enemy->get_fight_defense_point())));
    enemy->set_fight_hp(enemy->get_fight_hp() - damage_value);
    qDebug() << "迪莫使用了技能3 风暴聚集" << endl;
    qDebug() << "对方hp-" << damage_value << endl;
    log = "迪莫使用了技能3 风暴聚集 对方hp-" + QString::number(damage_value);
    emit logGenerated(log);
}

void dimo::attack(Pet *enemy)
{
    QString log;
    int skill_selected;
    double damage_value;
    if (is_evolved == 1) {
        skill_selected = QRandomGenerator::global()->bounded(1, 5);
        if (skill_selected == 1) {
            if (enemy->get_is_miss() == 1) {
                enemy->set_is_miss(0);
                qDebug() << "迪莫使用了普通攻击" << endl;
                qDebug() << "对方miss" << endl;
                log = "迪莫使用了普通攻击 对方miss";
                emit logGenerated(log);
                return;
            } else {
                damage_value = fight_damage_point
                               * (1
                                  - (enemy->get_fight_defense_point()
                                     / (100 + enemy->get_fight_defense_point())));
                enemy->set_fight_hp(enemy->get_fight_hp() - damage_value);
                qDebug() << "迪莫使用了普通攻击" << endl;
                qDebug() << "对方hp-" << damage_value << endl;
                log = "迪莫使用了普通攻击 对方hp-" + QString::number(damage_value);
                emit logGenerated(log);
            }

        } else if (skill_selected == 2) {
            if (enemy->get_is_miss() == 1) {
                enemy->set_is_miss(0);
                qDebug() << "迪莫使用了技能1 识破 削弱敌人攻击速度" << endl;
                qDebug() << "敌人miss" << endl;
                log = "迪莫使用了技能1 识破 削弱敌人攻击速度 敌人miss";
                emit logGenerated(log);
                return;
            } else
                penetrate(enemy);
        } else if (skill_selected == 3) {
            if (enemy->get_is_miss() == 1) {
                enemy->set_is_miss(0);
                qDebug() << "迪莫使用了技能2 迅捷步伐 对敌人造成伤害治疗自身" << endl;
                qDebug() << "敌人miss" << endl;
                log = "迪莫使用了技能2 迅捷步伐 对敌人造成伤害治疗自身 敌人miss";
                emit logGenerated(log);
                return;
            } else
                swift_stride(enemy);
        } else if (skill_selected == 4) {
            if (enemy->get_is_miss() == 1) {
                enemy->set_is_miss(0);
                qDebug() << "迪莫使用了技能3 风暴聚集" << endl;
                qDebug() << "敌人miss" << endl;
                log = "迪莫使用了技能3 风暴聚集 敌人miss";
                emit logGenerated(log);
                return;
            } else
                storm_gathering(enemy);
        }
    } else if (is_evolved == 0) {
        skill_selected = QRandomGenerator::global()->bounded(1, 4);
        if (skill_selected == 1) {
            if (enemy->get_is_miss() == 1) {
                enemy->set_is_miss(0);
                qDebug() << "迪莫使用了普通攻击" << endl;
                qDebug() << "对方miss" << endl;
                log = "迪莫使用了普通攻击 对方miss";
                emit logGenerated(log);
                return;
            } else {
                damage_value = fight_damage_point
                               * (1
                                  - (enemy->get_fight_defense_point()
                                     / (100 + enemy->get_fight_defense_point())));
                enemy->set_fight_hp(enemy->get_fight_hp() - damage_value);
                qDebug() << "迪莫使用了普通攻击" << endl;
                qDebug() << "对方hp-" << damage_value << endl;
                log = "迪莫使用了普通攻击 对方hp-" + QString::number(damage_value);
                emit logGenerated(log);
            }
        } else if (skill_selected == 2) {
            if (enemy->get_is_miss() == 1) {
                enemy->set_is_miss(0);
                qDebug() << "迪莫使用了技能1 识破 削弱敌人攻击速度" << endl;
                qDebug() << "敌人miss" << endl;
                log = "迪莫使用了技能1 识破 削弱敌人攻击速度 敌人miss";
                emit logGenerated(log);
                return;
            } else
                penetrate(enemy);
        } else if (skill_selected == 3) {
            if (enemy->get_is_miss() == 1) {
                enemy->set_is_miss(0);
                qDebug() << "迪莫使用了技能2 迅捷步伐 对敌人造成伤害治疗自身" << endl;
                qDebug() << "敌人miss" << endl;
                log = "迪莫使用了技能2 迅捷步伐 对敌人造成伤害治疗自身 敌人miss";
                emit logGenerated(log);
                return;
            } else
                swift_stride(enemy);
        }
    }
}
