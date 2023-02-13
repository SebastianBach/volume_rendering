#include "log.h"
#include <gtest/gtest.h>

TEST(ErrorHandling, ErrorClass)
{
    error_sys_intern::SetUnitTestMode();

    const int* const n_ptr = nullptr;
    EXPECT_TRUE(IsNullptr(n_ptr, MSG_INFO("")));

    const int iValue = 123;
    EXPECT_FALSE(IsNullptr(&iValue, MSG_INFO("")));

    EXPECT_TRUE(IsFalse(false, MSG_INFO("")));

    EXPECT_FALSE(IsFalse(true, MSG_INFO("")));

    EXPECT_TRUE(IsNull(0, MSG_INFO("")));

    EXPECT_FALSE(IsNull(1, MSG_INFO("")));

    EXPECT_FALSE(IsValue(1, 0, MSG_INFO("")));

    EXPECT_TRUE(IsValue(1, 1, MSG_INFO("")));

    EXPECT_TRUE(IsNotValue(1, 0, MSG_INFO("")));

    EXPECT_FALSE(IsNotValue(1, 1, MSG_INFO("")));
}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
