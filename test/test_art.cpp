#include <cstddef>
#include <format>
#include <fstream>
#include <gtest/gtest.h>
#include <string>

#include "art.hpp"

// NOLINTBEGIN

using namespace stl;

using Keys = const std::vector<std::string>&;

template<class T>
void assert_search(const ART<T>& art, const std::string& key,
                   typename ART<T>::const_reference value = T{})
{
    Leaf<T>* leaf = art.search(key);
    ASSERT_TRUE(leaf != nullptr && key == leaf->key_to_string() && leaf->value() == value);
}

template<class T>
void assert_failed_search(ART<T>& art, const std::string& key)
{
    ASSERT_TRUE(art.search(key) == nullptr);
}

template<class T>
void test_insert(ART<T>& art, Keys insert_keys, Keys valid_keys, Keys invalid_keys)
{
    for (auto it = insert_keys.begin(); it != insert_keys.end(); ++it) {
        art.insert(*it);
        assert_search(art, *it);

        for (auto it_s = insert_keys.begin(); it_s != it; ++it_s)
            assert_search(art, *it_s);

        for (auto it_s = std::next(it); it_s != insert_keys.end(); ++it_s)
            assert_failed_search(art, *it_s);

        for (auto& it_val : valid_keys)
            assert_search(art, it_val);

        for (auto& it_inv : invalid_keys)
            assert_failed_search(art, it_inv);
    }
}

template<class T>
void test_erase(ART<T>& art, Keys erase_keys, Keys valid_keys, Keys invalid_keys)
{
    for (auto it = erase_keys.begin(); it != erase_keys.end(); ++it) {
        art.erase(*it);
        assert_failed_search(art, *it);

        for (auto it_s = erase_keys.begin(); it_s != it; ++it_s)
            assert_failed_search(art, *it_s);

        for (auto it_s = std::next(it); it_s != erase_keys.end(); ++it_s)
            assert_search(art, *it_s);

        for (auto& it_val : valid_keys)
            assert_search(art, it_val);

        for (auto& it_inv : invalid_keys)
            assert_failed_search(art, it_inv);
    }
}

template<class T>
void test_crud(ART<T>& art, Keys keys, Keys valid_keys, Keys invalid_keys)
{
    test_insert(art, keys, valid_keys, invalid_keys);
    test_erase(art, keys, valid_keys, invalid_keys);
}

TEST(art_tests, sanity_test_0)
{
    static u32 c = 0;

    class SomeType {
    public:
        SomeType() { ++c; }

        ~SomeType() { --c; }
    };

    {
        ART<SomeType> art;
        art.insert("key_1");
        ASSERT_TRUE(c == 1);

        art.insert("key_2");
        ASSERT_TRUE(c == 2);

        art.erase("key_1");
        ASSERT_TRUE(c == 1);
    }

    ASSERT_TRUE(c == 0);
}

TEST(art_tests, sanity_test_1)
{
    ART art;

    test_insert(art, {"a"}, {}, {"", "aa", "b"});
    test_insert(art, {""}, {"a"}, {"aa", "b"});
    test_erase(art, {"a"}, {""}, {"a", "aa", "b"});
    test_erase(art, {""}, {}, {"", "a", "aa", "b"});
}

TEST(art_tests, sanity_test_2)
{
    ART art;

    std::vector<std::string> v1{"str1"};
    std::vector<std::string> v2{"str2"};
    std::vector<std::string> v3{"str3"};
    std::vector<std::string> v4{"str4"};
    std::vector<std::string> v5{"str5"};

    art.insert("my_vector1", &v1);
    art.insert("my_vector2", &v2);
    art.insert("my_vector3", &v3);
    art.insert("my_vector4", &v4);
    art.insert("my_vector5", &v5);

    ASSERT_TRUE(art.search("my_vector1")->value() == &v1);
    ASSERT_TRUE(art.search("my_vector2")->value() == &v2);
    ASSERT_TRUE(art.search("my_vector3")->value() == &v3);
    ASSERT_TRUE(art.search("my_vector4")->value() == &v4);
    ASSERT_TRUE(art.search("my_vector5")->value() == &v5);
}

TEST(art_tests, overaligned_object)
{
    class alignas(128) Overalign {
    public:
        operator void*() { return ptr; }

        void* ptr;
    };

    ART<Overalign> art;

    std::vector<std::string> v1{"str1"};
    std::vector<std::string> v2{"str2"};
    std::vector<std::string> v3{"str3"};
    std::vector<std::string> v4{"str4"};
    std::vector<std::string> v5{"str5"};

    art.insert("my_vector1", &v1);
    art.insert("my_vector2", &v2);
    art.insert("my_vector3", &v3);
    art.insert("my_vector4", &v4);
    art.insert("my_vector5", &v5);

    ASSERT_TRUE(art.search("my_vector1")->value() == &v1);
    ASSERT_TRUE(art.search("my_vector2")->value() == &v2);
    ASSERT_TRUE(art.search("my_vector3")->value() == &v3);
    ASSERT_TRUE(art.search("my_vector4")->value() == &v4);
    ASSERT_TRUE(art.search("my_vector5")->value() == &v5);
}

TEST(art_tests, common_header_test)
{
    ART art;

    art.insert("aaaaaaaaabbbbbbbbb");
    art.insert("aaaaaaaaabbbbbbbbc");

    assert_search(art, "aaaaaaaaabbbbbbbbb");
    assert_search(art, "aaaaaaaaabbbbbbbbc");

    assert_failed_search(art, "aaaaaaaaaaaa");
}

TEST(art_tests, i32_ranges)
{
    ART art;

    constexpr i32 capacity = 100000;
    constexpr i32 invalid = -1;
    std::vector<i32> v;
    v.reserve(capacity);

    for (i32 i = 0; i < capacity; ++i) {
        v.push_back(i);
        art.insert(std::to_string(i), &v[i]);

        assert_search(art, std::to_string(i), &v[i]);
    }

    for (i32 i = 0; i < capacity; ++i)
        assert_search(art, std::to_string(i), &v[i]);

    auto erase_range = [&](i32 start, i32 end) {
        for (i32 i = start; i < end; ++i) {
            art.erase(std::to_string(i));
            v[i] = invalid;
        }
    };

    auto search_all = [&]() {
        for (i32 i = 0; i < capacity; ++i)
            if (v[i] != invalid)
                assert_search(art, std::to_string(i), &v[i]);
            else
                assert_failed_search(art, std::to_string(i));
    };

    erase_range(0, 500);
    search_all();

    erase_range(capacity - 500, capacity);
    search_all();

    erase_range(2500, 3000);
    search_all();
}

TEST(art_tests, multiple_items)
{
    ART art;

    test_crud(art, {"abcdef", "abcde", "a", "abcdefgh"}, {},
              {"", "ab", "acdef", "abcdefg", "abcdefghy"});
}

TEST(art_tests, similar_keys_insertion)
{
    ART art;

    test_crud(art, {"aaaa", "aaaaa", "a", "aaaaaaaaaa", "aaba", "aa"}, {}, {"aaa"});
}

TEST(art_tests, similar_keys_insertion_2)
{
    ART art;

    test_crud(art, {"a", "aa", "aaa", "aaaa", "aaaaa", "aaaaaa", "aaaaaaa"}, {},
              {"", "aaaaaaaa", "b", "ab", "aab", "aaab", "aaaab", "aaaaab", "aaaaaab", "aaaaaaab"});
}

TEST(art_tests, similar_prefix_insertions)
{
    ART art;

    test_insert(art, {"aaaaaaaaa"}, {}, {"aaaaaaaaaa", "aaaaaaaab", "aaaaaaaaab"});
    test_insert(art, {"aaaaaaaaaa"}, {"aaaaaaaaa"}, {"aaaaaaaab", "aaaaaaaaab"});
    test_insert(art, {"aaaaaaaab"}, {"aaaaaaaaa", "aaaaaaaaaa"}, {"aaaaaaaaab"});
    test_insert(art, {"aaaaaaaaab"}, {"aaaaaaaaa", "aaaaaaaaaa", "aaaaaaaab"}, {});

    test_erase(art, {"aaaaaaaaab"}, {"aaaaaaaaa", "aaaaaaaaaa", "aaaaaaaab"}, {});
    test_erase(art, {"aaaaaaaab"}, {"aaaaaaaaa", "aaaaaaaaaa"}, {"aaaaaaaaab"});
    test_erase(art, {"aaaaaaaaaa"}, {"aaaaaaaaa"}, {"aaaaaaaab", "aaaaaaaaab"});
    test_erase(art, {"aaaaaaaaa"}, {}, {"aaaaaaaaaa", "aaaaaaaab", "aaaaaaaaab"});
}

TEST(art_tests, medium_size_keys_insertion)
{
    ART art;

    test_insert(art, {"abcdefghijklmnopqrstuvwxyz"}, {},
                {"abcdefghijklmnopqrstuvwxy", "abcdefghijklmnopqrstuvwxyzz"});

    test_insert(art, {"abcdefghijklmnopqrstuvwxy"}, {"abcdefghijklmnopqrstuvwxyz"},
                {"abcdefghijklmnopqrstuvwxyzz"});

    test_insert(art, {"abcdefghijklmnopqrstuvwxyzz"},
                {"abcdefghijklmnopqrstuvwxyz", "abcdefghijklmnopqrstuvwxy"}, {});

    test_erase(art, {"abcdefghijklmnopqrstuvwxyzz"},
               {"abcdefghijklmnopqrstuvwxyz", "abcdefghijklmnopqrstuvwxy"}, {});

    test_erase(art, {"abcdefghijklmnopqrstuvwxy"}, {"abcdefghijklmnopqrstuvwxyz"},
               {"abcdefghijklmnopqrstuvwxyzz"});

    test_erase(art, {"abcdefghijklmnopqrstuvwxyz"}, {},
               {"abcdefghijklmnopqrstuvwxy", "abcdefghijklmnopqrstuvwxyzz"});
}

TEST(art_tests, long_keys_insertion)
{
    ART art;

    constexpr usize str_len = 1024ULL * 1024;
    const std::string long_str(str_len, '!');

    art.insert(long_str);
    assert_search(art, long_str);
    assert_failed_search(art, "a" + long_str);

    art.insert("a" + long_str);
    art.insert("b" + long_str);
    art.insert("c" + long_str);
    art.insert("d" + long_str);
    art.insert("e" + long_str);

    assert_search(art, long_str);
    assert_search(art, "a" + long_str);
    assert_search(art, "b" + long_str);
    assert_search(art, "c" + long_str);
    assert_search(art, "d" + long_str);
    assert_search(art, "e" + long_str);
    assert_failed_search(art, "f" + long_str);
}

TEST(art_tests, growing_nodes)
{
    ART art;

    constexpr usize str_len = 1024ULL;
    const std::string long_str(str_len, '!');

    std::vector<std::string> keys;

    for (i32 i = 1; i < 256; ++i)
        keys.push_back(char(i) + long_str);

    test_crud(art, keys, {}, {});
}

TEST(art_tests, growing_nodes_2)
{
    ART art;

    constexpr usize str_len = 1024ULL;
    const std::string long_str(str_len, '!');

    std::vector<std::string> keys;

    const usize buf_size = 16;
    char buf[buf_size];
    std::memset(buf, 1, buf_size);

    keys.push_back(std::string(buf, buf + buf_size));
    keys.push_back(std::string(buf, buf + buf_size) + long_str);
    keys.push_back(long_str + std::string(buf, buf + buf_size));

    for (usize i = 0; i < buf_size; ++i) {
        for (usize j = 2; j < 64; ++j) {
            buf[i] = j;
            keys.push_back(std::string(buf, buf + buf_size));
            keys.push_back(std::string(buf, buf + buf_size) + long_str);
            keys.push_back(long_str + std::string(buf, buf + buf_size));
        }
    }

    test_crud(art, keys, {}, {});
}

TEST(art_tests, different_key_sizes)
{
    ART art;

    constexpr usize key_max_size = 8;
    u8 buff[key_max_size];
    std::memset(buff, 1, key_max_size);

    std::vector<std::string> keys;

    for (usize i = 0; i < key_max_size; ++i) {
        for (usize j = 1; j < 32; ++j) {
            buff[i] = j;
            keys.push_back(std::string(buff, buff + i + 1));
        }
    }

    test_crud(art, keys, {}, {});
}

#ifndef DEBUG
TEST(art_tests, different_key_sizes_big)
{
    ART art;

    constexpr usize key_max_size = 32;
    u8 buff[key_max_size];
    std::memset(buff, 1, key_max_size);

    std::vector<std::string> keys;

    for (usize i = 0; i < key_max_size; ++i) {
        for (usize j = 1; j < 256; ++j) {
            buff[i] = j;
            keys.push_back(std::string(buff, buff + i + 1));
        }
    }

    test_crud(art, keys, {}, {});
}
#endif

TEST(art_tests, prefix_search)
{
    ART art;

    std::string s1{"str1"};
    std::string s2{"str2"};
    std::string s3{"str3"};
    std::string s4{"str4"};
    std::string s5{"str5"};

    art.insert(s1);
    art.insert(s2);
    art.insert(s3);
    art.insert(s4);
    art.insert(s5);

    ASSERT_TRUE(art.search_prefix("").size() == 5);
    ASSERT_TRUE(art.search_prefix("s").size() == 5);
    ASSERT_TRUE(art.search_prefix("st").size() == 5);
    ASSERT_TRUE(art.search_prefix("str").size() == 5);
    ASSERT_TRUE(art.search_prefix("str1").size() == 1);
    ASSERT_TRUE(art.search_prefix("str5").size() == 1);
    ASSERT_TRUE(art.search_prefix("str", 1).size() == 1);
    ASSERT_TRUE(art.search_prefix("str", 3).size() == 3);
    ASSERT_TRUE(art.search_prefix("str", 5).size() == 5);
    ASSERT_TRUE(art.search_prefix("str", 1024).size() == 5);
}

TEST(art_tests, key_value_test_1)
{
    ART<std::string> art_str;

    std::string key{"key_1"};
    std::string value{"value_1"};

    art_str.insert(key, value);
    assert_search(art_str, key, value);

    art_str.insert("key_2", "value_2");
    assert_search(art_str, "key_2", "value_2");

    std::string key_3{"key_3"};
    std::string value_3{"value_3"};

    art_str.insert(std::move(key_3), std::move(value_3));
    assert_search(art_str, "key_3", "value_3");
    ASSERT_TRUE(key_3 == "key_3" && value_3.empty()); // key should not be moved.
}

TEST(art_tests, key_value_test_2)
{
    ART<std::vector<i32>> art_v;

    std::vector<i32> v1{1, 2, 3, 4, 5, 6};
    art_v.insert("key_1", v1);
    assert_search(art_v, "key_1", v1);

    auto leaf = art_v.search("key_1");
    leaf->value().pop_back();

    auto leaf_1 = art_v.search("key_1");
    ASSERT_TRUE(leaf_1->value() != v1);

    v1.pop_back();
    assert_search(art_v, "key_1", v1);

    leaf->value() = {1, 2, 3};
    assert_search(art_v, "key_1", {1, 2, 3});
}

TEST(art_tests, key_value_test_3)
{
    ART<std::string> art_s;

    ASSERT_TRUE(art_s.insert("key_1", "value_1"));
    auto res = art_s.insert("key_1", "value_2");

    ASSERT_TRUE(res == false && res->value() == "value_1");
    res->value() = "value_2";

    ASSERT_TRUE(art_s.search("key_1")->value() == "value_2");
}

/**
 * Reads all filesystem paths from provided input file, inserts them into ART and search for them 1
 * by 1 while verifying searches.
 */
static void test_filesystem_paths(const std::string& file_name)
{
    ART art;

    std::ifstream in_file_stream{std::string(TEST_ROOT) + "/input_files/" + file_name};
    ASSERT_TRUE(in_file_stream.is_open());

    std::vector<std::string> paths;

    for (std::string file_path; std::getline(in_file_stream, file_path);) {
        paths.push_back(file_path);
        art.insert(paths.back());
    }

    // TODO: Do test_crud here.
    // It seems that there are duplicates in files.
    for (auto& it : paths)
        assert_search(art, it);

    constexpr usize MB = 1024ULL * 1024;

    std::cout << std::format("\nEntries count:           {}K\n", paths.size() / 1000ULL);
    std::cout << std::format("ART size with leaves:    {}MB\n", art.size_in_bytes(true) / MB);
    std::cout << std::format("ART size without leaves: {}MB\n\n", art.size_in_bytes(false) / MB);
}

TEST(art_tests, file_system_paths)
{
    std::vector<std::string> file_names{
        "windows_paths.txt",
        "linux_paths.txt",
        "windows_paths_vscode.txt",
    };

    for (const auto& file_name : file_names)
        test_filesystem_paths(file_name);
}

// NOLINTEND
