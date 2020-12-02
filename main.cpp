#include "utils/BigNum.hpp"
#include "PlanetLocation.hpp"
#include "toJson.hpp"
#include <iostream>
#include "StorageDb.hpp"
#include "UserCredentials.hpp"
#include <thread>

void test(const std::string& str)
{
    auto num = BigNum::fromStr(str);
    std::cout << num.toString() << std::endl;
}

void testSum(const std::string& lhs, const std::string& rhs)
{
    std::cout << lhs << " + " << rhs << " = " << (BigNum::fromStr(lhs) + BigNum::fromStr(rhs)).toString() << std::endl;
}

int main()
{
    test("123.456");
    test("0");
    test("0.0");
    test("0.123");

    testSum("12.345", "111.111");

    BigNum aa{5};
    BigNum bb{5.6};

    PlanetLocation loc{1, 234, 5};
    nlohmann::json j;
    to_json(j, loc);
    std::cout << j << std::endl;

    sqlite::StorageDb db{""};
    std::this_thread::sleep_for(std::chrono::seconds(2));
    db.queryPlayer(UserCredentials{.login = "asdf", .passcode = "ddd"});
    return 0;
}
