//
// Created by renduiyang on 24-10-13.
//

#ifndef MYSQLCONN_H
#define MYSQLCONN_H
#include "mysql.h"
#include <string>
#include <chrono>


class MysqlConn {
private:
    MYSQL *m_connect = nullptr;
    MYSQL_RES *m_result = nullptr;
    MYSQL_ROW m_row;
    std::chrono::steady_clock::time_point m_aliveTime;

    void freeResult();

public:
    // 初始化数据库连接
    MysqlConn();

    // 释放数据库连接
    ~MysqlConn();

    // 连接数据库
    bool connect(std::string user, std::string passwd, std::string dbName, std::string ip,
                 unsigned short port = 3306) const;

    // 更新数据库: select，update，delete
    bool update(std::string sql) const;

    //查询语句
    bool query(std::string sql);

    // 给用户查询的到结果集的数据的行数
    std::pair<int, int> get_result_rowAndCol() const;

    // 返回查询结果集字段名
    std::vector<std::string> get_result_fieldName() const;

    // 返回查询结果
    std::vector<std::vector<std::string> > get_result();

    // 事务操作
    bool transaction() const;

    // 提交事务
    bool commit();

    // 事务回滚
    bool rollback();

    // 刷新起始的空闲时间点
    void refreshAliveTime();

    // 计算连接存活的总时长
    long long getAliveTime();
};

#endif //MYSQLCONN_H
