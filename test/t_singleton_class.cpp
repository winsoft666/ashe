#include "catch.hpp"
#include "ashe/singleton_class.h"

using namespace ashe;

class SingletonTest : public SingletonClass<SingletonTest> {
   public:
    int value = 0;

    unsigned long GetThisPointer() {
        return (unsigned long)this;
    }

    void setDestoryFlag(bool* p) {
        destoryed_ = p;
    }

    ~SingletonTest() {
        if (destoryed_)
            *destoryed_ = true;
    }

   private:
    SingletonTest() {}

   private:
    bool* destoryed_ = nullptr;

    friend class SingletonClass<SingletonTest>;
};

class SingletonTestAET : public SingletonClass<SingletonTestAET, true> {
   public:
    int value = 0;

    unsigned long GetThisPointer() {
        return (unsigned long)this;
    }

    void setDestoryFlag(bool* p) {
        destoryed_ = p;
    }

    ~SingletonTestAET() {
        if (destoryed_)
            *destoryed_ = true;
    }

   private:
    SingletonTestAET() {}

   private:
    bool* destoryed_ = nullptr;

    friend class SingletonClass<SingletonTestAET, true>;
};

unsigned long Bar() {
    return SingletonTest::Instance()->GetThisPointer();
}

unsigned long BarAET() {
    return SingletonTestAET::Instance()->GetThisPointer();
}

TEST_CASE("SingletonClass") {
    unsigned long p1 = Bar();
    unsigned long p2 = SingletonTest::Instance()->GetThisPointer();

    REQUIRE(p1 == p2);

    SingletonTest::Instance()->value = 1;
    REQUIRE(SingletonTest::Instance()->value == 1);

    SingletonTest::Instance()->value = 2;
    REQUIRE(SingletonTest::Instance()->value == 2);
}

TEST_CASE("SingletonClass-AtExitManager") {
    bool destoryed = false;
    do {
        AtExitManager aet;
        SingletonTestAET::Instance()->setDestoryFlag(&destoryed);

        unsigned long p1 = BarAET();
        unsigned long p2 = SingletonTestAET::Instance()->GetThisPointer();

        REQUIRE(p1 == p2);

        SingletonTestAET::Instance()->value = 1;
        REQUIRE(SingletonTestAET::Instance()->value == 1);

        SingletonTestAET::Instance()->value = 2;
        REQUIRE(SingletonTestAET::Instance()->value == 2);
    } while (false);

    REQUIRE(destoryed);
}