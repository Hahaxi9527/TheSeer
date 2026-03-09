#ifndef DATABASE_H
#define DATABASE_H

#pragma execution_character_set("utf-8")
#include <QDebug>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QTextCodec>

class database
{
public:
    database();

    //数据库是否打开
    int is_open;
    QSqlDatabase db;
    void open_database();
    void close_database();
    //    void execute_query(const QString &queryStr);
};

#endif // DATABASE_H
