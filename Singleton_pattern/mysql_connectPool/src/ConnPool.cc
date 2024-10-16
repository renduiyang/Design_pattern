//
// Created by renduiyang on 24-10-16.
//
#include "ConnPool.h"

ConnPool *ConnPool::getConnPool() {
    static ConnPool pool;
    return &pool;
}

/**
 * @brief 从数据库连接池中取出一个连接,这里使用shared_ptr指针
 * @return
 */
std::shared_ptr<MysqlConn> ConnPool::getConn() {
    std::unique_lock<std::mutex> m_lock(m_mutexQ);
    while (m_connQ.empty()) {
        // 如果连接池为空，则等待
        // m_cond.wait(m_lock);
        //增加超时等待,如果超时则继续等待
        if (std::cv_status::timeout == m_cond.wait_for(m_lock, std::chrono::milliseconds(m_timeout))) {
            if (m_connQ.empty()) {
                continue;
            }
        }
    }
    // 智能指针第二个参数属于删除器,可以与匿名函数结合使用
    // 当智能指针被销毁时，会调用第二个参数的函数,把这个连接重新放回到mysql连接池中
    std::shared_ptr<MysqlConn> get_conn(m_connQ.front(), [this](MysqlConn *mysql_conn) {
        std::lock_guard<std::mutex> delete_lock(m_mutexQ);
        mysql_conn->refreshAliveTime();
        m_connQ.push(mysql_conn);
    });
    m_connQ.pop();
    m_cond.notify_one(); // 唤醒一个等待的线程
    return get_conn;
}

/**
 * @brief 析构函数，释放连接池中的连接
 */
ConnPool::~ConnPool() {
    while (!m_connQ.empty()) {
        MysqlConn *conn = m_connQ.front();
        delete conn;
        m_connQ.pop();
    }
}

/**
 * @brief 构造函数,初始化连接池,同时建立两个子线程,分别为制造连接和释放连接
 */
ConnPool::ConnPool() {
    m_ip = "127.0.0.1";
    m_port = 3306;
    m_user = "root";
    m_passwd = "root";
    m_dbName = "rdy_test";
    m_maxSize = 20; // 最大连接数为20
    m_minSize = 10; // 最小连接数为10
    m_timeout = 10000; // 超时秒数为10秒
    m_maxIdleTime = 300000; // 最大空闲时间为5分钟
    for (int i = 0; i < m_minSize; i++) {
        addConn();
    }
    std::thread product_thread(&ConnPool::produceConn, this);
    std::thread release_thread(&ConnPool::releaseConn, this);
    product_thread.detach();
    release_thread.detach();
}

/**
 * @brief 生成线程，每30秒检查一次连接池
 * @note 如果连接数小于最小连接数(同时保证进程建立的连接数小于最大连接数,因为最后这些连接是会被回收的)，则添加连接
 */
void ConnPool::produceConn() {
    while (true) {
        std::unique_lock<std::mutex> m_lock(m_mutexQ);
        // wait 函数用于使线程等待直到满足某个条件为止。也就是说wait第二个参数为true时，线程被唤醒，否则线程被挂起。
        // 这个使用一个生成记录器,每调用一次addConn，生成记录器加一,直到生成记录器等于最大连接数
        // 当生成记录器等于最大连接数时，线程将会被阻塞  直到释放线程删除掉一个连接才可以
        m_cond.wait(m_lock, [this]() { return (m_connQ.size() < m_minSize) && (creat_connect < m_maxSize); });
        addConn();
        m_cond.notify_all();
    }
}

/**
 * @brief 释放线程，每30秒检查一次连接池，如果连接数大于最小连接数，则回收连接
 */
void ConnPool::releaseConn() {
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(30));
        // 每隔1分钟检查一次
        std::lock_guard<std::mutex> m_lock(m_mutexQ);
        while (m_connQ.size() > m_minSize) {
            // 如果连接池中的连接数大于最小连接数，则回收连接
            MysqlConn *conn = m_connQ.front(); // 取出连接池中的连接
            if (conn->getAliveTime() >= m_maxIdleTime) {
                m_connQ.pop(); // 回收连接
                delete conn; // 释放连接资源,释放连接之后对应所记录的生成数也要减一
                --creat_connect;
            } else {
                break; // 如果连接的空闲时间小于最大空闲时间，则跳出循环
            }
        }
    }
}

/**
 * @brief 数据库连接池添加连接
 */
void ConnPool::addConn() {
    MysqlConn *conn = new MysqlConn();
    if (conn->connect(m_user, m_passwd, m_dbName, m_ip, m_port)) {
        conn->refreshAliveTime();
        m_connQ.push(conn);
        ++creat_connect;
    }
}
