
#include <gtest/gtest.h>
#include <numeric>
#include "../delegate/function.h"

#if defined _WIN32 || defined_WIN64

#include <crtdbg.h>

class MemoryLeakDetector {
public:
    MemoryLeakDetector() {
        _CrtMemCheckpoint(&memState_);
    }

    ~MemoryLeakDetector() {
        _CrtMemState stateNow, stateDiff;
        _CrtMemCheckpoint(&stateNow);
        int diffResult = _CrtMemDifference(&stateDiff, &memState_, &stateNow);
        if (diffResult)
            reportFailure(stateDiff.lSizes[1]);
    }
private:
    void reportFailure(unsigned int unfreedBytes) {
        FAIL() << "Memory leak of " << unfreedBytes << " byte(s) detected.";
    }
    _CrtMemState memState_;
};
#endif


TEST(function, test_function)
{
    #if defined _WIN32 || defined_WIN64
    MemoryLeakDetector memoryLeakDetector;
    #endif
    using namespace auto_delegate;


    function<int(int,int)> empty_f;

    ASSERT_FALSE(empty_f);

    function f0 = []{ return 114514; };
    ASSERT_EQ(f0(), 114514);

    function<int(int,int)> f = [](int a, int b) { return a + b; };
    ASSERT_EQ(f(1, 2), 3);

    f = [](int a, int b) { return a - b; };
    ASSERT_EQ(f(3, 2), 1);

    struct small_buffer
    {
        int a;
        int b;
    }capture(3, 4);

    f = [=] (int a, int b) { return a + b + capture.a + capture.b; };
    ASSERT_EQ(f(1, 2), 10);

    //copy
    auto f2 = f;
    ASSERT_EQ(f2(1, 2), 10);

    //move
    auto f3 = std::move(f);
    ASSERT_EQ(f3(1, 2), 10);

    ASSERT_FALSE(f);


    struct buffer_1_t
    {
        uint64_t arr[7] = {1, 2, 3, 4, 5, 6, 7};
    }b1;
    double exam_sum_1 = std::accumulate(std::begin(b1.arr), std::end(b1.arr), 0.0);
    function<uint64_t(uint64_t)> f4 = [=](uint64_t a)
    {
        return std::accumulate(std::begin(b1.arr), std::end(b1.arr), a);
    };
    ASSERT_EQ(f4(123), exam_sum_1 + 123);

    struct buffer_2_t
    {
        uint64_t arr[6] = {1, 2, 3, 4, 5, 6};
    }b2;
    double exam_sum_2 = std::accumulate(std::begin(b2.arr), std::end(b2.arr), 0.0);
    f4 = [=](uint64_t a)
    {
        return std::accumulate(std::begin(b2.arr), std::end(b2.arr), a);
    };
    ASSERT_EQ(f4(123), exam_sum_2 + 123);


    function<uint64_t(uint64_t)> f5 = [=] (uint64_t a) mutable
    {
        return std::accumulate(std::begin(b1.arr), std::end(b1.arr), a);
    };
    ASSERT_EQ(f5(123), exam_sum_1 + 123);

    struct buffer_12_t
    {
        uint64_t arr[6] = {1, 2, 3, 4, 5, 6};
    }b12;
    f4 = [=](uint64_t a) mutable
    {
        return std::accumulate(std::begin(b2.arr), std::end(b2.arr), a);
    };
    ASSERT_EQ(f4(123), exam_sum_2 + 123);
}

TEST(function, teststd_function)
{

    std::function<int(int,int)> f = [](int a, int b) { return a + b; };
    ASSERT_EQ(f(1, 2), 3);

}
