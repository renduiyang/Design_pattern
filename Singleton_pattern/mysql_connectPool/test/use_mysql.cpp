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
    // 获取结果集行数(mysql_num_row返回值是unsigned long long ---> 可以用auto更简洁一些,但是auto导致代码阅读效果很差)
    const unsigned long long find_number = mysql_num_rows(result);
    printf("select number is %llu \n", find_number);
    // mysql一行数据句柄为 MYSQL_ROW ,Qt为QSqlRecord(QSqlQuery调用record函数即可)
    // 遍历每一行,mysql遍历每一行结果时借助 mysql_fetch_row 函数
    MYSQL_ROW row;
    //  mysql_fetch_row返回当前结果集的一行数据 可以理解为是一个指针  每调用一次 行数据指针向下移动一行
    // 可以用for循环配合 mysql_num_rows函数使用
    // 也可以直接使用while((row = mysql_fetch_row(result))来使用
    // 补充!!! mysql_num_fields会输出一行数据中的列数
    for (auto i = 0; i < find_number; i++) {
        row = mysql_fetch_row(result);
        const int column_count = mysql_num_fields(result); // 获取一行数据中的列数
        for (int j = 0; j < column_count; j++) {
            printf_s("%s ", row[j]);
        }
        printf_s("\n");
    }
    // while ((row = mysql_fetch_row(result))) {
    //     const int column_count = mysql_num_fields(result); // 获取一行数据中的列数
    //     for (int i = 0; i < column_count; i++) {
    //         printf_s("%s ", row[i]);
    //     }
    //     printf_s("\n");
    // }
    // 释放结果集,mysql每次查询相当于把结果存放到结果集里,下一次查询前需要释放这个结果集
    mysql_free_result(result);
    mysql_close(conn);
    return 0;
}
