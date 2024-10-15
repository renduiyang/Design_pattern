//
// Created by renduiyang on 24-10-13.
//

#ifndef MYSQLCONN_HPP
#define MYSQLCONN_HPP
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
    bool connect(std::string user, std::string passwd, std::string dbName, std::string ip, unsigned short port = 3306) const;

    // 更新数据库: select，update，delete
    bool update(std::string sql) const;

    // 查询数据库
    bool query(std::string sql);

    // 遍历查询得到的结果集
    bool next();

    // 得到结果集中的字段值
    std::string value(int index);

    // 事务操作
    bool transaction();

    // 提交事务
    bool commit();

    // 事务回滚
    bool rollback();

    // 刷新起始的空闲时间点
    void refreshAliveTime();

    // 计算连接存活的总时长
    long long getAliveTime();
};

// inline内联减少函数调用开销,提高执行效率,但是会增加代码大小
inline void MysqlConn::freeResult() {
    if (m_result != nullptr) {
        mysql_free_result(m_result);
        m_result = nullptr;
    }
}

inline MysqlConn::MysqlConn() {
    m_connect = mysql_init(nullptr);
    m_aliveTime = std::chrono::steady_clock::now();
}

inline MysqlConn::~MysqlConn() {
    freeResult();
}

inline bool MysqlConn::connect(std::string user, std::string passwd, std::string dbName, std::string ip,
                               unsigned short port) const {
    if (mysql_real_connect(m_connect, ip.c_str(), user.c_str(), passwd.c_str(), dbName.c_str(), port, nullptr, 0) ==
        nullptr) {
        return false;
    }
    return true;
}

inline bool MysqlConn::update(std::string sql) const {
    // 这里涉及到mysql_query返回值问题: !!! 如果mysql_query执行成功默认时返回0的
    if (mysql_query(m_connect, sql.c_str())) {
        return false;
    }
    return true;
}

inline bool MysqlConn::next() {
}

#endif //MYSQLCONN_HPP
