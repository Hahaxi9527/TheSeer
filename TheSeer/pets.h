
#ifndef PETS_H
#define PETS_H

#include <QObject>
#include <QString>
#include <QWidget>

class Pet : public QObject
{
    Q_OBJECT
protected:
    QString name;        //名字
    int level;           //等级
    int exp;             //Experience point经验值
    int killed_exp;      //击杀后所得到的经验
    double damage_point; //physical damage point物理攻击力
                         // int magical_dp;      //magical damage point法术攻击力
    double defense_point; //physical armor point物理防御力
    //int magical_ap;      //magical armor point法术防御力
    double hp;              //生命值
    double attack_interval; //攻击间隔
    int id;              //每一只宠物编号唯一
    int is_upgrade = 0;  //是否升级
    int is_evolved;      //是否到达6级进化
    int is_miss = 0;     //是否免疫伤害
    int is_relife = 0;                      //是否复活
    int is_dead = 0;                        //是否死亡
    double fight_damage_point;              //战斗时的战斗力
    double fight_defense_point;             //战斗时防御力
    double fight_attack_interval;           //战斗时攻击间隔
    double fight_hp;                        //战时生命值
    int fight_damage_gain = 0;              //战时战斗力增益次数
    int fight_defense_gain = 0;             //战时防御力增益次数
    int fight_attack_interval_gain = 0;     //战时攻击间隔增益次数
    int fight_damage_decrease = 0;          //战时攻击力削弱次数
    int fight_defense_decrease = 0;         //战时护甲削弱次数
    int fight_attack_interval_decrease = 0; //战时攻击速度削弱次数
public:
    explicit Pet(QObject *parent = nullptr);
    Pet();
    Pet(QString pet_name, int _level);
    virtual void upgrade() = 0; //升级函数
    virtual void attack(Pet *) = 0;
    //virtual void magical_normal_attack(Pet *);

    //int get_magical_dp();
    //int get_magical_ap();
    QString get_name(); //获取名字

    double get_damage_point();          //获取攻击力
    double get_defense_point();         //获取护甲
    double get_hp();                    //获取生命值
    double get_attack_interval();       //获取攻击间隔
    double get_fight_damage_point();    //获取战时攻击力
    double get_fight_defense_point();   //获取战时护甲
    double get_fight_hp();              //获取战时hp
    double get_fight_attack_interval(); //获取战时攻击间隔
    int get_killed_exp(int level);      //获取击杀后得到的经验
    int get_level();                    //获取等级
    int get_fight_defense_decrease();   //获取战时护甲削弱次数
    int get_fight_defense_gain();       //获取战时护甲增益次数
    int get_fight_damage_decrease();    //获取战时攻击力削弱次数
    int get_fight_damage_gain();        //获取战时攻击力增益次数
    int get_fight_attack_interval_decrease(); //获取战时攻击速度削弱次数
    int get_fight_attack_interval_gain();     //获取战时攻击力增益次数
    int get_is_miss();                        //获取是否免疫伤害和技能

    void set_level(int value);                    //设置等级
    void set_damage_point(double value);          //设置攻击力
    void set_defense_point(double value);         //设置护甲
    void set_hp(double value);                    //设置hp
    void set_attack_interval(double value);       //设置攻击间隔
    void set_exp(int self_level, int enemy_level); //设置经验值
    void set_fight_damage_point(double value);    //设置战时1攻击力
    void set_fight_defense_point(double value);   //设置战时护甲
    void set_fight_attack_interval(double value); //设置战时攻击间隔
    void set_fight_hp(double value);              //设置战时hp
    void set_fight_defense_decrease(int value);   //设置战时护甲削弱次数
    void set_fight_defense_gain(int value);       //设置战时护甲增益次数
    void set_fight_damage_decrease(int value);    //设置战时攻击力削弱次数
    void set_fight_damage_gain(int value);        //设置战时攻击力增益次数
    void set_fight_attack_interval_decrease(int value); //设置战时攻击速度削弱次数
    void set_fight_attack_interval_gain(int value);     //设置战时攻击速度增益次数
    void set_is_miss(int value);                        //设置是否免疫伤害和技能

    int check_is_evolved(); //检查是否6级
    int check_is_dead();    //检查是否死亡

signals:
    void logGenerated(const QString &log);
};

//攻击性宠物
class attack_pet : public Pet
{
    Q_OBJECT
public:
    attack_pet(QObject *parent = nullptr)
        : Pet(parent)
    {}
    virtual void physical_normal_attack(Pet *){};
    void upgrade();
};

//音速犬
class sonic_dog : public attack_pet
{
    Q_OBJECT
public:
    sonic_dog(int _level, QObject *parent = nullptr)
        : attack_pet(parent)
    {
        name = "音速犬";
        level = _level;
        hp = 250 + (level - 1) * 30;
        damage_point = 50 + (level - 1) * 25;
        defense_point = 50 + (level - 1) * 7;
        attack_interval = 2 - (level - 1) * 0.06;
        fight_hp = hp;
        fight_damage_point = damage_point;
        fight_defense_point = defense_point;
        fight_attack_interval = attack_interval;
        exp = 0;
        if (level >= 6) {
            is_evolved = 1;
        } else
            is_evolved = 0;
    };
    void attack(Pet *enemy);
    void fury(Pet *enemy);                //技能1 狂暴
    void bloodthirsty_strike(Pet *enemy); //技能2 嗜血一击
    void one_shot(Pet *enemy);            // 技能3 致命一击
};

//烈火战神
class blaze_god_of_war : public attack_pet
{
    Q_OBJECT
public:
    blaze_god_of_war(int _level, QObject *parent = nullptr)
        : attack_pet(parent)
    {
        name = "烈火战神";
        level = _level;
        hp = 250 + (level - 1) * 30;
        damage_point = 50 + (level - 1) * 25;
        defense_point = 50 + (level - 1) * 7;
        attack_interval = 2 - (level - 1) * 0.06;
        fight_hp = hp;
        fight_damage_point = damage_point;
        fight_defense_point = defense_point;
        fight_attack_interval = attack_interval;
        exp = 0;
        if (level >= 6) {
            is_evolved = 1;
        } else
            is_evolved = 0;
    }
    void attack(Pet *enemy);
    void overthrow(Pet *enemy);      //技能1 以下克上
    void blazing_charge(Pet *enemy); //技能2 烈焰冲击
    void the_monarch_arrive();       //技能3 君王降临
};

//肉盾型宠物
class high_hp_pet : public Pet
{
    Q_OBJECT
public:
    high_hp_pet(QObject *parent = nullptr)
        : Pet(parent)
    {}
    void upgrade();
};

//黑熊狂战
class black_bear_berserk : public high_hp_pet
{
public:
    black_bear_berserk(int _level, QObject *parent = nullptr)
        : high_hp_pet(parent)
    {
        name = "黑熊狂战";
        level = _level;
        hp = 350 + (level - 1) * 75;
        damage_point = 40 + (level - 1) * 8;
        defense_point = 60 + (level - 1) * 10;
        attack_interval = 2 - (level - 1) * 0.03;
        fight_hp = hp;
        fight_damage_point = damage_point;
        fight_defense_point = defense_point;
        fight_attack_interval = attack_interval;
        exp = 0;
        if (level >= 6) {
            is_evolved = 1;
        } else
            is_evolved = 0;
    }
    void attack(Pet *enemy);
    void recovery();              //技能1 复苏
    void eye_for_eye(Pet *enemy); //技能2 以眼还眼
    void guardian_wings();        //技能3 守护者之翼
};

//阿瑞斯Ares
class ares : public high_hp_pet
{
public:
    ares(int _level, QObject *parent = nullptr)
        : high_hp_pet(parent)
    {
        name = "阿瑞斯";
        level = _level;
        hp = 350 + (level - 1) * 75;
        damage_point = 40 + (level - 1) * 8;
        defense_point = 60 + (level - 1) * 10;
        attack_interval = 2 - (level - 1) * 0.03;
        fight_hp = hp;
        fight_damage_point = damage_point;
        fight_defense_point = defense_point;
        fight_attack_interval = attack_interval;
        exp = 0;
        if (level >= 6) {
            is_evolved = 1;
        } else
            is_evolved = 0;
    }
    void attack(Pet *enemy);
    void heal();                     //技能1 治疗术
    void earthquake(Pet *enemy);     //技能2 山崩地裂
    void final_radiance(Pet *enemy); //技能3 回光返照
};

//防御型宠物
class defense_pet : public Pet
{
    Q_OBJECT
public:
    defense_pet(QObject *parent = nullptr)
        : Pet(parent)
    {}
    void upgrade();
};

//岩凯领主
class rock_armor_lord : public defense_pet
{
public:
    rock_armor_lord(int _level, QObject *parent = nullptr)
        : defense_pet(parent)
    {
        name = "岩铠领主";
        level = _level;
        hp = 300 + (level - 1) * 60;
        damage_point = 40 + (level - 1) * 7;
        defense_point = 70 + (level - 1) * 15;
        attack_interval = 2 - (level - 1) * 0.03;
        fight_hp = hp;
        fight_damage_point = damage_point;
        fight_defense_point = defense_point;
        fight_attack_interval = attack_interval;
        exp = 0;
        if (level >= 6) {
            is_evolved = 1;
        } else
            is_evolved = 0;
    }
    void attack(Pet *enemy);
    void solid_layer();           //技能1 坚实表皮
    void shield_bash(Pet *enemy); //技能2 盾击
    void taunt(Pet *enemy);       //技能3 嘲讽
};

//琥珀骑士
class amber_knight : public defense_pet
{
public:
    amber_knight(int _level, QObject *parent = nullptr)
        : defense_pet(parent)
    {
        name = "琥珀骑士";
        level = _level;
        hp = 300 + (level - 1) * 60;
        damage_point = 40 + (level - 1) * 7;
        defense_point = 70 + (level - 1) * 15;
        attack_interval = 2 - (level - 1) * 0.03;
        fight_hp = hp;
        fight_damage_point = damage_point;
        fight_defense_point = defense_point;
        fight_attack_interval = attack_interval;
        exp = 0;
        if (level >= 6) {
            is_evolved = 1;
        } else
            is_evolved = 0;
    }
    void attack(Pet *enemy);
    void defense_stance();                 //技能1 防御姿态
    void smashed_to_bits(Pet *enemy);      //技能2 粉身碎骨
    void cycle_of_desperation(Pet *enemy); //技能3 轮回绝境
};

//敏捷型宠物
class agile_pet : public Pet
{
    Q_OBJECT
public:
    agile_pet(QObject *parent = nullptr)
        : Pet(parent)
    {}
    void upgrade();
};

//疾驰奎因
class swift_quinn : public agile_pet
{
public:
    swift_quinn(int _level, QObject *parent = nullptr)
        : agile_pet(parent)
    {
        name = "疾驰奎因";
        level = _level;
        hp = 250 + (level - 1) * 30;
        damage_point = 45 + (level - 1) * 8;
        defense_point = 40 + (level - 1) * 5;
        attack_interval = 1.5 - (level - 1) * 0.05;
        fight_hp = hp;
        fight_damage_point = damage_point;
        fight_defense_point = defense_point;
        fight_attack_interval = attack_interval;
        exp = 0;
        if (level >= 6) {
            is_evolved = 1;
        } else
            is_evolved = 0;
    }
    void attack(Pet *enemy);
    void combo_attack(Pet *enemy);    //技能1 连砍
    void deadly_rhythm();             //技能2 致命节奏
    void a_narrow_escape();           //技能3 千钧一发
};

//迪莫
class dimo : public agile_pet
{
public:
    dimo(int _level, QObject *parent = nullptr)
        : agile_pet(parent)
    {
        name = "迪莫";
        level = _level;
        hp = 250 + (level - 1) * 30;
        damage_point = 45 + (level - 1) * 8;
        defense_point = 40 + (level - 1) * 5;
        attack_interval = 1.5 - (level - 1) * 0.05;
        fight_hp = hp;
        fight_damage_point = damage_point;
        fight_defense_point = defense_point;
        fight_attack_interval = attack_interval;
        exp = 0;
        if (level >= 6) {
            is_evolved = 1;
        } else
            is_evolved = 0;
    }
    void attack(Pet *enemy);
    void penetrate(Pet *enemy);       //技能1 识破
    void swift_stride(Pet *enemy);    //技能2 迅捷步伐
    void storm_gathering(Pet *enemy); //技能3 风暴聚集
};

#endif // PETS_H
