//
// Created by renduiyang on 24-10-13.
//

#ifndef CONNPOOL_H
#define CONNPOOL_H

#include <queue>
#include <vector>
#include <mutex>
#include <condition_variable>//条件变量
#include "MysqlConn.h"

class ConnPool {
public:
    static ConnPool *getConnPool(); // 获得单例对象
    ConnPool(const ConnPool &obj) = delete; // 删除拷贝构造函数
    ConnPool &operator=(const ConnPool &obj) = delete; // 删除拷贝赋值运算符重载函数
    std::shared_ptr<MysqlConn> getConn(); // 从连接池中取出一个连接
    ~ConnPool(); // 析构函数
private:
    ConnPool(); // 构造函数私有化
    void produceConn(); // 生产数据库连接
    void releaseConn(); // 销毁数据库连接
    void addConn(); // 添加数据库连接

    // 连接服务器所需信息
    std::string m_ip; // 数据库服务器ip地址
    std::string m_user; // 数据库服务器用户名
    std::string m_dbName; // 数据库服务器的数据库名
    std::string m_passwd; // 数据库服务器密码
    unsigned short m_port; // 数据库服务器绑定的端口

    // 连接池信息
    std::queue<MysqlConn *> m_connQ;
    unsigned int m_maxSize; // 连接数上限值
    unsigned int m_minSize; // 连接数下限值
    // 时长单位均为毫秒
    int m_timeout; // 连接超时时长
    int m_maxIdleTime; // 最大的空闲时长
    std::mutex m_mutexQ; // 独占互斥锁
    std::condition_variable m_cond; // 条件变量
    static int creat_connect;
};

int ConnPool::creat_connect = 0;

#endif //CONNPOOL_H
