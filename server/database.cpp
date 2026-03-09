#include "database.h"

database::database()
{
    is_open = 0;
    open_database();
}

void database::open_database()
{
    if (is_open == 1) {
        return;
    }

    // 检查是否已经存在一个名为 "MyDbConnection" 的连接
    if (QSqlDatabase::contains("QMYSQL")) {
        db = QSqlDatabase::database("QMYSQL");
    } else {
        db = QSqlDatabase::addDatabase("QMYSQL");
    }

    db.setHostName("localhost");
    db.setPort(3306);
    db.setDatabaseName("the_seer");
    db.setUserName("root");
    db.setPassword("Hahaxi9527_MT");

    QTextCodec *codec = QTextCodec::codecForName("utf-8");
    QTextCodec::setCodecForLocale(codec);

    if (!db.open()) {
        qDebug() << "打开失败，失败原因：" << db.lastError().text();
        is_open = 0;
    } else {
        qDebug() << "成功连接数据库";
        is_open = 1;
    }
}

// 示例函数：执行 SQL 查询
//void database::execute_query(const QString &queryStr)
//{
//    // 检查数据库是否打开
//    if (!db.isOpen()) {
//        qDebug() << "数据库未打开";
//        return;
//    }

//    QSqlQuery query(db);
//    if (!query.exec(queryStr)) {
//        qDebug() << "查询执行失败：" << query.lastError().text();
//    } else {
//        qDebug() << "查询执行成功";
//    }
//}

void database::close_database()
{
    db.close();
    is_open = 0;
}
