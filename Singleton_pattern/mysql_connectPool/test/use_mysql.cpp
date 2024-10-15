//
// Created by renduiyang on 24-10-15.
//
// mysql数据库c++接口使用流程
// mysql相关句柄 MYSQL* ; MYSQL_RES* ; MYSQL_ROW
// qmysql相关句柄 QSqlDatabase; QSqlQuery; QSqlRecord
#include "mysql.h"
#include <iostream>

int main() {
    // 初始化一个连接  mysql自己的句柄为 MYSQL ,Qt为QSqlDatabase
    MYSQL *conn = mysql_init(nullptr);

    // 这里是一个判断语句，如果初始化失败，则退出程序,fprintf(stderr,xxx,xxx)标准格式化错误输出
    if (conn == nullptr) {
        fprintf(stderr, "%s\n", mysql_error(conn));
        exit(1);
    }
    // 连接数据库借助mysql_real_connect函数
    if (!mysql_real_connect(conn, "localhost", "root", "root", "rdy_test", 0, NULL, 0)) {
        fprintf(stderr, "%s\n", mysql_error(conn));
        mysql_close(conn);
        exit(1);
    }
    const std::string query = "SELECT * FROM users";
    // 执行sql语句 mysql_query(sql连接,sql语句)  这里的sql语句必须为char* ,如果时std::string类型时要通过c_str()转换
    if (mysql_query(conn, query.c_str())) {
        fprintf(stderr, "%s\n", mysql_error(conn));
        mysql_close(conn);
        exit(1);
    }
    // mysql结果集的句柄为 MYSQL_RES , Qt为QSqlQuery
    // mysql_store_result函数把结果集存放到内存中
    MYSQL_RES *result = mysql_store_result(conn);
    if (result == nullptr) {
        fprintf(stderr, "%s\n", mysql_error(conn)); // 格式化输出错误
        mysql_close(conn);
        exit(1);
    }
    // mysql一行数据句柄为 MYSQL_ROW ,Qt为QSqlRecord(QSqlQuery调用record函数即可)
    MYSQL_ROW row;
    // 遍历每一行,mysql遍历每一行结果时借助 mysql_fetch_row 函数
    while ((row = mysql_fetch_row(result))) {
        const int column_count = mysql_num_fields(result); // 获取一行数据中的列数
        for (int i = 0; i < column_count; i++) {
            printf_s("%s ", row[i]);
        }
        printf_s("\n");
        // printf("%s %s\n", row[0], row[1]);
    }
    // 释放结果集,mysql每次查询相当于把结果存放到结果集里,下一次查询前需要释放这个结果集
    mysql_free_result(result);
    mysql_close(conn);
    return 0;
}
