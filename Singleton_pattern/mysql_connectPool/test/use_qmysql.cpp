//
// Created by renduiyang on 24-10-13.
//
// Qt对mysql的一层封装
#include <QSqlDatabase>
// 使用时使用QtSql
#include <QtSql>
#include <QDebug>

int main(int argc, char *argv[]) {
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("192.168.18.121"); // MySQL 服务器地址
    db.setDatabaseName("rdy_test"); // 数据库名称
    db.setUserName("root"); // 用户名
    db.setPassword("root"); // 密码

    // 尝试打开数据库连接
    if (!db.open()) {
        qDebug() << "Cannot open database:" << db.lastError().text();
        return 1;
    }

    qDebug() << "Database opened successfully";
    // 创建一个 QSqlQuery 对象
    QSqlQuery query(db);
    query.prepare("select * from users;");
    // 这里表示查询成功
    if (query.exec()) {
        qDebug() << query.record().count(); // 返回查询到的字段数
        qDebug() << query.size(); // 返回查询到的结果数(行数)
        for (int i = 0; i < query.record().count(); ++i) {
            //返回字段名
            qDebug() << query.record().fieldName(i);
        }
        while (query.next()) {
            qDebug() << "------------";
            // 知道查询到的每条结果的字段名后就可以用for循环输出
            for (int i = 0; i < query.record().count(); ++i) {
                //query相当于查询到的一条结果,value可以获取到每条结果的列值
                qDebug() << query.value(i).toString();
            }
        }
    }
    db.close();

    return 0;
}
