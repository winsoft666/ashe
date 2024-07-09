#include "catch.hpp"
#include "ashe/singleton_class.hpp"

class SingletonTest : public ashe::SingletonClass<SingletonTest> {
   public:
    int value = 0;

   private:
    SingletonTest() {}

    friend class ashe::SingletonClass<SingletonTest>;
};

TEST_CASE("SingletonClass", "1") {
    SingletonTest::Instance()->value = 1;
    SingletonTest::Instance()->value = 2;

    SingletonTest::Release();
}