#include "catch.hpp"
#include "ashe/singleton_class.h"

using namespace ashe;

class SingletonTest : public SingletonClass<SingletonTest> {
   public:
    int value = 0;

    unsigned long GetThisPointer() {
        return (unsigned long)this;
    }

   private:
    SingletonTest() {}

    friend class SingletonClass<SingletonTest>;
};

unsigned long Bar() {
    return SingletonTest::Instance()->GetThisPointer();
}

TEST_CASE("SingletonClass") {
    unsigned long p1 = Bar();
    unsigned long p2 = SingletonTest::Instance()->GetThisPointer();

    REQUIRE(p1 == p2);

    SingletonTest::Instance()->value = 1;
    REQUIRE(SingletonTest::Instance()->value == 1);

    SingletonTest::Instance()->value = 2;
    REQUIRE(SingletonTest::Instance()->value == 2);

    SingletonTest::Release();
}