//
// Created by renduiyang on 24-10-16.
//
#include "MysqlConn.h"

// 可以使用内联 inline内联减少函数调用开销,提高执行效率,但是会增加代码大小
void MysqlConn::freeResult() {
    if (m_result != nullptr) {
        mysql_free_result(m_result);
        m_result = nullptr;
    }
}

/**
 * @brief
 * @param sql 数据库查询语句 将查询到的结果存储在m_result中
 * @return bool true:执行成功
 */
bool MysqlConn::query(std::string sql) {
    freeResult();
    if (mysql_query(m_connect, sql.c_str())) {
        return false;
    }
    m_result = mysql_store_result(m_connect);
    return true;
}

/**
 * @brief 构造函数 初始化mysql句柄
 */
MysqlConn::MysqlConn() {
    m_connect = mysql_init(nullptr);
    m_aliveTime = std::chrono::steady_clock::now();
}

/**
 * @brief 析构函数
 */
MysqlConn::~MysqlConn() {
    if (m_connect != nullptr) {
        mysql_close(m_connect);
    }
    freeResult();
}


/**
 * @brief 数据库连接函数
 * @param user 用户名
 * @param passwd 密码
 * @param dbName 数据库
 * @param ip 数据库ip地址
 * @param port 端口号
 * @return bool true:连接成功
 */
bool MysqlConn::connect(std::string user, std::string passwd, std::string dbName, std::string ip,
                        unsigned short port) const {
    if (mysql_real_connect(m_connect, ip.c_str(), user.c_str(), passwd.c_str(), dbName.c_str(), port, nullptr, 0) ==
        nullptr) {
        return false;
    }
    return true;
}

/**
 * @brief 数据库中updata insert delete操作函数
 * @param sql 数据库句柄
 * @return bool true:执行成功
 */
bool MysqlConn::update(std::string sql) const {
    // 这里涉及到mysql_query返回值问题: !!! 如果mysql_query执行成功默认时返回0的
    if (mysql_query(m_connect, sql.c_str())) {
        return false;
    }
    // updata, insert, delete 这三个如果执行成功数据库肯定会受到影响
    if (mysql_affected_rows(m_connect) != 0) {
        return true;
    }
    return false;
}

/**
 * @brief 返回查询到的结果集中的行数以及列数
 * @return
 */
std::pair<int, int> MysqlConn::get_result_rowAndCol() const {
    return std::make_pair(mysql_num_rows(m_result), mysql_num_fields(m_result));
}

/**
 * @brief 返回数据库查询结果集中的列名
 * @return
 */
std::vector<std::string> MysqlConn::get_result_fieldName() const {
    std::vector<std::string> result;
    MYSQL_FIELD *field = nullptr;
    for (int i = 0; i < mysql_num_fields(m_result); ++i) {
        field = mysql_fetch_field_direct(m_result, i);
        result.push_back(field->name);
    }
    return result;
}

/**
 * @brief 返回数据库查询结果集,二维数组
 * @return
 * @note 这个存在问题,如果这个结果集很大,那么会消耗大量内存
 */
std::vector<std::vector<std::string> > MysqlConn::get_result() {
    if (m_result == nullptr) {
        return {};
    }
    freeResult();
    std::vector<std::vector<std::string> > result;
    int col_num = mysql_num_fields(m_result);
    for (int i = 0; i < mysql_num_rows(m_result); i++) {
        m_row = mysql_fetch_row(m_result);
        std::vector<std::string> temp_row_result;
        for (int j = 0; j < col_num; j++) {
            temp_row_result.push_back(m_row[j]);
        }
        result.push_back(temp_row_result);
        temp_row_result.clear();
    }
    return result;
}

/**
 * @brief 事务操作函数 禁止自动commit
 * @return
 */
bool MysqlConn::transaction() const {
    return mysql_autocommit(m_connect, false);
}

/**
 * @brief commit函数, 用于提交手动事务
 * @return
 */
bool MysqlConn::commit() {
    return mysql_commit(m_connect);
}

/**
 * @brief 事务回滚
 * @return
 */
bool MysqlConn::rollback() {
    return mysql_rollback(m_connect);
}

/**
 * @brief 刷新时间,也可以用于构造函数初始化时间
 */
void MysqlConn::refreshAliveTime() {
    // 这个时间戳就是某个数据库连接，它起始存活的时间点
    // 这个时间点通过时间类就可以得到了
    m_aliveTime = std::chrono::steady_clock::now();
}

/**
 * @brief 查询mysql连接存活时间
 * @return
 */
long long MysqlConn::getAliveTime() {
    std::chrono::nanoseconds duration = std::chrono::steady_clock::now() - m_aliveTime;
    // duration_cast<TargetDuration>(source)：将 source 转换为 TargetDuration 类型。
    // nanoseconds：纳秒  milliseconds：毫秒
    std::chrono::milliseconds millsec = duration_cast<std::chrono::milliseconds>(duration);
    return millsec.count();
}
