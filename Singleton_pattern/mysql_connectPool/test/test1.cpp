//
// Created by renduiyang on 24-10-13.
//
#include <iostream>
#include "MysqlConn.h"
#include "ConnPool.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>

namespace {
    TEST(ExampleTests, SimpleAddition) {
        int a = 5;
        int b = 10;
        EXPECT_EQ(a + b, 15);
    }

    TEST(ExampleTests2, SimpleAddition2) {
        int a = 5;
        int b = 10;
        EXPECT_EQ(a + b, 15);
    }
}

int main(int argc, char *argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
