//
// Created by renduiyang on 24-10-15.
//
#include "mysql.h"
#include <iostream>

int main() {
    MYSQL *conn = mysql_init(nullptr);

    if (conn == nullptr) {
        fprintf(stderr, "%s\n", mysql_error(conn));
        exit(1);
    }
    if (!mysql_real_connect(conn, "localhost", "root", "root", "rdy_test", 0, NULL, 0)) {
        fprintf(stderr, "%s\n", mysql_error(conn));
        mysql_close(conn);
        exit(1);
    }
    const char *query = "SELECT * FROM users";
    if (mysql_query(conn, query)) {
        fprintf(stderr, "%s\n", mysql_error(conn));
        mysql_close(conn);
        exit(1);
    }
    MYSQL_RES *result = mysql_store_result(conn);
    if (result == nullptr) {
        fprintf(stderr, "%s\n", mysql_error(conn)); // 格式化输出错误
        mysql_close(conn);
        exit(1);
    }
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
    mysql_free_result(result);
    mysql_close(conn);
    return 0;
}
