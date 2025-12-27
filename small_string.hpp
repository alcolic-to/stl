/**
 * Copyright 2025, Aleksandar Colic
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#pragma once

#ifndef STL_SMALL_STRING_HPP
#define STL_SMALL_STRING_HPP

#include <cassert>
#include <cstring>
#include <string>

#include "alloc.hpp"
#include "ptr_tag.hpp"
#include "types.hpp"

// NOLINTBEGIN

namespace stl {

/**
 * Small string.
 * If string is smaller than 7 bytes, data will be stored in pointer directly, otherwise m_data will
 * point to allocated string. We will set pointer tag (small or big) which indicates whether string
 * is small or big.
 * TODO: Provide whether we should allocate bytes in chunks or with general purpose allocator. For
 * now, we will always allocate in chunks, which might not be good for general purpose usage.
 */
class SmallString {
    static constexpr usize small_limit = 6;
    static constexpr uptr small_tag = 0;
    static constexpr uptr big_tag = 1;

public:
    static constexpr usize npos = -1;

    constexpr SmallString() : m_data{nullptr} {}

    constexpr SmallString(const std::string& s) : SmallString(s.c_str(), s.size()) {}

    constexpr SmallString(const char* s, usize size = 0) : m_data{nullptr}
    {
        if (s == nullptr)
            return;

        if (size == 0)
            size = std::strlen(s);

        if (size <= small_limit) {
            std::memcpy(m_sso + 1, s, size); // Small tag is set to 0 implicitly on a first byte.
            return;
        }

        ctr_from_big(s, size);
    }

    constexpr SmallString(const SmallString& other)
    {
        if (other.small())
            m_data = other.m_data;
        else
            ctr_from_big(other.c_str(), other.size());
    }

    SmallString(SmallString&& other) noexcept
    {
        m_data = other.m_data;
        other.m_data = nullptr;
    }

    ~SmallString()
    {
        if (empty() || small())
            return;

        free_buffer(big_data());
    }

    /**
     * Allocates buffer long enough to hold size bytes.
     * It allocates memory from pools of 1MB. When chunk is exceeded, new chunk is requested with
     * malloc. Allocations are aligned on a std::max_align_t (usually 16) bytes.
     * Note that this function is not thread safe.
     * Also, it is only used in finder for file names allocation. For general purpose case, it might
     * not be good.
     */
    static void* allocate_buffer(usize size)
    {
        static constexpr usize align = alignof(std::max_align_t);
        static constexpr usize align_mask = align - 1;
        static constexpr usize chunk_size = 1024UL * 1024;

        static u8* m_memory = nullptr;
        static usize m_allocated = 0;

        usize aligned_size = (size + align_mask) & ~align_mask;

        if (m_memory == nullptr || m_allocated + aligned_size > chunk_size) {
            m_memory = allocate<u8>(chunk_size);
            m_allocated = 0;
        }

        void* allocation = m_memory + m_allocated;
        m_allocated += aligned_size;

        return allocation;
    }

    /**
     * Nothing to do in delete.
     */
    static void free_buffer([[maybe_unused]] void* memory) {}

    SmallString& operator=(const SmallString& other)
    {
        if (other.small())
            m_data = other.m_data;
        else
            ctr_from_big(other.c_str(), other.size());

        return *this;
    }

    SmallString& operator=(SmallString&& other) noexcept
    {
        m_data = other.m_data;
        other.m_data = nullptr;
        return *this;
    }

    [[nodiscard]] bool operator==(const char* other) const noexcept
    {
        return !std::strcmp(c_str(), other);
    }

    [[nodiscard]] bool operator==(const std::string& other) const noexcept
    {
        return !std::strcmp(c_str(), other.c_str());
    }

    [[nodiscard]] bool operator==(const SmallString& other) const noexcept
    {
        return !std::strcmp(c_str(), other.c_str());
    }

    [[nodiscard]] constexpr bool empty() const noexcept { return m_data == nullptr; }

    [[nodiscard]] constexpr void* big_data() const noexcept { return clear_tag(m_data); }

    [[nodiscard]] constexpr const char* c_str() const noexcept
    {
        return small() ? &m_sso[1] : static_cast<char*>(clear_tag(m_data));
    }

    [[nodiscard]] constexpr std::string str() const noexcept { return std::string{c_str()}; }

    [[nodiscard]] constexpr operator const char*() const noexcept { return c_str(); }

    [[nodiscard]] constexpr operator bool() const noexcept { return !empty(); }

    [[nodiscard]] usize size() const noexcept { return std::strlen(*this); }

    [[nodiscard]] bool starts_with(const SmallString& other) const noexcept
    {
        return std::strncmp(c_str(), other.c_str(), other.size()) == 0;
    }

    [[nodiscard]] bool contains(const char* needle) const noexcept
    {
        return std::strstr(c_str(), needle);
    }

    [[nodiscard]] bool contains(const std::string& needle) const noexcept
    {
        return contains(needle.c_str());
    }

    [[nodiscard]] usize find(const char* needle, usize offset = 0) const noexcept
    {
        assert(offset <= size()); // we are always 0 terminated.

        const char* r = std::strstr(c_str() + offset, needle);
        return r != nullptr ? r - c_str() : npos;
    }

    [[nodiscard]] usize find(const std::string& needle, usize offset = 0) const noexcept
    {
        return find(needle.c_str(), offset);
    }

    [[nodiscard]] usize find(const std::string_view& needle, usize offset = 0) const noexcept
    {
        return find(needle.data(), offset);
    }

private:
    [[nodiscard]] constexpr bool small() const noexcept { return tag(m_data) == small_tag; }

    [[nodiscard]] constexpr bool big() const noexcept { return tag(m_data) == big_tag; }

    void ctr_from_big(const char* big, usize size)
    {
        assert(size > small_limit);

        char* str = static_cast<char*>(allocate_buffer(size + 1));
        std::memcpy(str, big, size);
        str[size] = 0;
        m_data = set_tag(str, big_tag);
    }

    union {
        void* m_data;
        char m_sso[8];
    };
};

static_assert(sizeof(SmallString) == 8);

} // namespace stl

// NOLINTEND

#endif // STL_SMALL_STRING_HPP
