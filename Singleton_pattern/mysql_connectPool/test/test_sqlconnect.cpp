//
// Created by renduiyang on 24-10-20.
//
#include <gtest/gtest.h>
#include "MysqlConn.h"
#include "ConnPool.h"

class TestSqlConn : public ::testing::TestWithParam<std::tuple<int, int, bool> > {
protected:
    ConnPool *pool;

    virtual void SetUp() override {
        pool = ConnPool::getConnPool();
    }
};


bool test_operator1(int begin, int end) {
    bool flag = false;
    for (int i = begin; i < end; ++i) {
        MysqlConn conn;
        if (conn.connect("root", "root", "rdy_test", "127.0.0.1", 3306)) {
            char sql[1024] = {0};
            sprintf(sql, "insert into users(username, email) values('%s','%s')", "person", "person@qq.com");
            flag = conn.update(sql);
        }
    }
    return flag;
}


bool test_operator2(ConnPool *pool, int begin, int end) {
    bool flag = false;
    for (int i = begin; i < end; ++i) {
        std::shared_ptr<MysqlConn> conn = pool->getConn();
        char sql[1024] = {0};
        sprintf(sql, "insert into users(username, email) values('%s','%s')", "people", "people@qq.com");
        flag = conn->update(sql);
    }
    return flag;
}

// TEST_P(TestSqlConn, test_getConn_directUSE) {
//     int begin = std::get<0>(GetParam());
//     int end = std::get<1>(GetParam());
//     bool expect_flag = std::get<2>(GetParam());
//     EXPECT_EQ(test_operator1(begin,end), expect_flag);
// }

TEST_P(TestSqlConn, test_getConn_poolUSE) {
    int begin = std::get<0>(GetParam());
    int end = std::get<1>(GetParam());
    bool expect_flag = std::get<2>(GetParam());
    EXPECT_EQ(test_operator2(pool, begin, end), expect_flag);
}

INSTANTIATE_TEST_SUITE_P(
    PositiveNumbers,
    TestSqlConn,
    ::testing::Values(
        std::make_tuple(0, 50000, true)
    )
);

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
