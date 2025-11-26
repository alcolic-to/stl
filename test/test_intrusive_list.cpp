#include <gtest/gtest.h>

#include "intrusive_list.hpp"

// NOLINTBEGIN

struct Data {
    Data() = default;

    Data(int i) : m_value{i} {}

    stl::INode m_node;
    int m_value;
};

TEST(test_intrusive_list, sanity_test)
{
    Data data[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    stl::IList<Data, offsetof(Data, m_node)> list;

    ASSERT_TRUE(list.empty());
    ASSERT_EQ(list.size(), 0u);

    list.push_back(data[0]);
    list.push_back(data[1]);
    list.push_back(data[2]);
    list.push_back(data[3]);
    list.push_back(data[4]);

    ASSERT_FALSE(list.empty());
    ASSERT_EQ(list.size(), 5u);
    ASSERT_EQ(list.front().m_value, 0);
    ASSERT_EQ(list.back().m_value, 4);

    int expected = 0;
    for (auto& d : list) {
        ASSERT_EQ(d.m_value, expected);
        ++expected;
    }

    expected = 0;
    for (const auto& d : list) {
        ASSERT_EQ(d.m_value, expected);
        ++expected;
    }

    list.push_front(data[5]);
    ASSERT_EQ(list.front().m_value, 5);
    ASSERT_EQ(list.size(), 6u);

    Data& pf = list.pop_front();
    Data& pb = list.pop_back();
    ASSERT_EQ(pf.m_value, 5);
    ASSERT_EQ(pb.m_value, 4);
    ASSERT_EQ(list.size(), 4u);

    auto it = list.begin();
    while (it != list.end() && it->m_value != 2)
        ++it;

    ASSERT_NE(it, list.end());
    list.erase(it);
    ASSERT_EQ(list.size(), 3u);

    for (auto& d : list)
        ASSERT_NE(d.m_value, 2);

    Data& to_remove = data[1];
    list.remove(to_remove);
    ASSERT_EQ(list.size(), 2u);

    ASSERT_EQ(to_remove.m_node.m_next, nullptr);
    ASSERT_EQ(to_remove.m_node.m_prev, nullptr);

    while (!list.empty())
        (void)list.pop_front();

    ASSERT_TRUE(list.empty());
    ASSERT_EQ(list.size(), 0u);
}

TEST(test_intrusive_list, erase_end_and_begin_erase)
{
    Data data[3] = {10, 11, 12};
    stl::IList<Data, offsetof(Data, m_node)> list;

    list.push_back(data[0]);
    list.push_back(data[1]);
    list.push_back(data[2]);

    auto size = list.size();
    auto res = list.erase(list.end());
    ASSERT_EQ(res, list.end());
    ASSERT_EQ(list.size(), size);

    auto last = list.remove(list.back());
    ASSERT_EQ(last.m_value, 12);
    ASSERT_EQ(list.size(), size - 1);

    auto it = list.begin();
    list.erase(it);
    ASSERT_EQ(list.size(), size - 2);
    ASSERT_EQ(list.front().m_value, 11);
}

TEST(test_intrusive_list, reuse_node_after_pop_and_reinsert)
{
    Data d{42};
    stl::IList<Data, offsetof(Data, m_node)> list;

    list.push_back(d);
    Data& p = list.pop_front();
    ASSERT_EQ(&p, &d);

    ASSERT_EQ(d.m_node.m_next, nullptr);
    ASSERT_EQ(d.m_node.m_prev, nullptr);

    list.push_back(d);
    ASSERT_EQ(list.size(), 1u);
    ASSERT_EQ(&list.front(), &d);

    (void)list.pop_back();
    ASSERT_TRUE(list.empty());
}

TEST(test_intrusive_list, const_iterator_equality)
{
    Data data[2] = {7, 8};
    stl::IList<Data, offsetof(Data, m_node)> list;

    list.push_back(data[0]);
    list.push_back(data[1]);

    auto it = list.begin();
    auto end = list.end();
    ASSERT_TRUE(it != end);

    ++it;
    ++it;
    ASSERT_TRUE(it == end);

    auto cit = list.cbegin();
    auto cend = list.cend();
    ASSERT_TRUE(cit != cend);

    ++cit;
    ++cit;
    ASSERT_TRUE(cit == cend);
}

// NOLINTEND