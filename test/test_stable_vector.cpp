#include <condition_variable>
#include <cstddef>
#include <gtest/gtest.h>
#include <stdexcept>
#include <string>
#include <vector>

#include "stable_vector.hpp"

// NOLINTBEGIN

using namespace stl;

class Normal {
public:
    u32 v{};
    std::string s;
};

class Non_copyable_nor_moveable {
public:
    Non_copyable_nor_moveable() = default;

    Non_copyable_nor_moveable(u32 ov, std::string os) : v{ov}, s{std::move(os)} {}

    u32 v{};
    std::string s;
    std::mutex m;
    std::condition_variable cv;
};

TEST(stable_vector, sanity_test)
{
    StableVector<Normal> vec{5};
    ASSERT_EQ(vec.size(), 0);
    ASSERT_EQ(vec.capacity(), 5);

    vec.emplace_back(1U, "String 1");
    vec.emplace_back(2U, "String 2");
    vec.emplace_back(3U, "String 3");
    vec.emplace_back(4U, "String 4");
    vec.emplace_back(5U, "String 5");

    ASSERT_EQ(vec.size(), 5U);

    u32 i = 1U;
    for (auto& el : vec)
        ASSERT_EQ(i++, el.v);

    ASSERT_THROW(vec.emplace_back(6U, "String 6"), std::runtime_error);

    ASSERT_EQ(vec.size(), 5);
    ASSERT_EQ(vec.capacity(), 5);

    Normal n{std::move(vec.back())};
    ASSERT_EQ(n.v, 5U);
    ASSERT_EQ(n.s, "String 5");
    vec.pop_back();

    ASSERT_EQ(vec.size(), 4);
    ASSERT_EQ(vec.capacity(), 5);

    i = 1U;
    for (auto& el : vec)
        ASSERT_EQ(i++, el.v);

    while (!vec.empty())
        vec.pop_back();

    ASSERT_EQ(vec.size(), 0);
    ASSERT_EQ(vec.capacity(), 5);
}

TEST(stable_vector, class_test)
{
    StableVector<Non_copyable_nor_moveable> vec{5};
    ASSERT_EQ(vec.size(), 0);
    ASSERT_EQ(vec.capacity(), 5);

    vec.emplace_back(1U, "String 1");
    vec.emplace_back(2U, "String 2");
    vec.emplace_back(3U, "String 3");
    vec.emplace_back(4U, "String 4");
    vec.emplace_back(5U, "String 5");

    ASSERT_EQ(vec.size(), 5U);

    u32 i = 1U;
    for (auto& el : vec)
        ASSERT_EQ(i++, el.v);

    ASSERT_THROW(vec.emplace_back(6U, "String 6"), std::runtime_error);

    ASSERT_EQ(vec.size(), 5);
    ASSERT_EQ(vec.capacity(), 5);

    vec.pop_back();

    ASSERT_EQ(vec.size(), 4);
    ASSERT_EQ(vec.capacity(), 5);

    i = 1U;
    for (auto& el : vec)
        ASSERT_EQ(i++, el.v);

    while (!vec.empty())
        vec.pop_back();

    ASSERT_EQ(vec.size(), 0);
    ASSERT_EQ(vec.capacity(), 5);
}

// NOLINTEND
