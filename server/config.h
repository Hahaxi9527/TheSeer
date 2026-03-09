#ifndef CONFIG_H
#define CONFIG_H

#define SOCKET "00"
#define SOCKET_1 "001"
#define SIGNIN "0"
#define SIGNIN_SUCCEED "01"
#define SIGNIN_FAILED "02"
#define SIGNOUT "03"
#define SIGNOUT_SUCCEED "031"
#define NOT_REGISTER "04"
#define USER_ONLINE "05"
#define REGISTER "1"
#define REGISTER_SUCCEED "11"
#define REGISTER_FAILED "12"
#define LOBBY "2"
#define LOBBY_SUCCEED "21"
#define LOBBY_FAILED "22"
#define SWITCH_TO_USERLIST "3"
#define USERLIST_INFO "31"
#define SWITCH_TO_PETBAG "4"
#define PET_INFO "41"
#define SWITCH_TO_BATTLE_CHOOSE "5"
#define BATTLE_CHOOSE "51"

#define RECHARGE "6"
#define RECHARGE_SUCCESSED "61"
#define RECHARGE_FAILED "62"
#define ADDPETS "7"
#define ADDPETS_SUCCESSED "71"
#define ADDPETS_FAILED "72"

#define FLUSH_SERVER "8"
#define FLUSH_SERVER_SUCCESSED "81"
#define ADD_SERVER_PETS "9"
#define ADD_SERVER_PETS_SUCCESSED "91"

#define START_UPGRADE_BATTLE "10"
#define START_UPGRADE_BATTLE_SUCCESSED "101"
#define START_VS_BATTLE_SUCCESSED "121"

#define UPGRADE_WIN "110"
#define UPGRADE_LOSE "111"
#define START_VS_BATTLE "120"
#define VS_WIN "122"
#define VS_LOSE "123"

#define SHOW_ABANDON "130"
#define ABANDON "140"

#define UPGRADE "150"
#define UPGRADE_SUCCESSED "151"

//宠物名字编号
#define SONIC_DOG 0
#define BLAZE_GOD_OF_WAR 1
#define BLACK_BEAR_BERSERK 2
#define ARES 3
#define ROCK_ARMOR_LORD 4
#define AMBER_KNIGHT 5
#define SWIFT_QUINN 6
#define DIMO 7

//宠物属性编号
#define ATTACK_PET 0
#define HIGH_HP__PET 1
#define DEFENSE_PET 2
#define AGILE_PET 3
#endif // CONFIG_H
