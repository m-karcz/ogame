#include <gtest/gtest.h>
#include "BigNum.hpp"

using namespace ::testing;

/*TEST(BigNumTests, addingSameSign)
{
    auto sut = BigNum::fromStr("123.456");
    ASSERT_EQ(sut.toString(), "123.456000");

    auto lhs = BigNum::fromStr("123.45");
    auto rhs = BigNum::fromStr("2222.222");
    ASSERT_EQ((lhs + rhs).toString(), "2345.672");
}*/

struct AddParam
{
    std::string lhs;
    std::string rhs;
    std::string res;
};

struct BigNumAddTests : TestWithParam<AddParam>
{};

TEST_P(BigNumAddTests, testAdding)
{
    auto lhs = BigNum::fromStr(GetParam().lhs);
    auto rhs = BigNum::fromStr(GetParam().rhs);
    auto result = (lhs + rhs).toString();
    ASSERT_EQ(result, GetParam().res);
}

INSTANTIATE_TEST_CASE_P(Adding,
                        BigNumAddTests,
                        Values(AddParam{"123.45", "2222.222", "2345.672000"},
                               AddParam{"-123.45","1234.56", "1111.110000"},
                               AddParam{"1234.56", "-123.45", "1111.110000"},
                               AddParam{"-1234.56", "123.45", "-1111.110000"},
                               AddParam{"123.45", "-1234.56", "-1111.110000"},
                               AddParam{"-123.45", "-2222.222", "-2345.672000"}));

struct BigNumShiftTests : Test
{
};

TEST_F(BigNumShiftTests, leftShift)
{
    auto res = BigNum::fromStr("0.1") << 1;
    ASSERT_EQ(BigNum::fromStr("0.1").toString(), "0.100000");
    ASSERT_EQ(res.toString(), "1.000000");
}

TEST_F(BigNumShiftTests, rightShift)
{
    auto res = BigNum::fromStr("1.2") >> 1;
    ASSERT_EQ(res.toString(), "0.120000");

    res = res >> 5;

    ASSERT_EQ(res.toString(), "0.000001");
}

struct BigNumMultiplyTests : Test
{};

TEST_F(BigNumMultiplyTests, mul)
{
    auto lhs = BigNum::fromStr("3.5");
    auto rhs = BigNum::fromStr("4");

    ASSERT_EQ((lhs * rhs).toString(), "14.000000");
}

struct BigNumDivisionTests : Test
{};

TEST_F(BigNumDivisionTests, div)
{
    auto lhs = BigNum::fromStr("12");
    auto rhs = BigNum::fromStr("4");

    ASSERT_EQ((lhs / rhs).toString(), "3.000000");
}
