

#include <gtest/gtest.h>
#include <iostream>
#include <package.hpp>
TEST(PackageTest, get_test){
    Package pack(3);
    ASSERT_EQ(pack.get_id(),3);
}
TEST(package_test,get){
    Package pack(3);
    ASSERT_EQ(pack.get_id(),3);
}
