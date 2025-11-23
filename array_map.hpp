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

#ifndef STL_ARRAY_MAP_H
#define STL_ARRAY_MAP_H

#include <algorithm>
#include <format>
#include <stdexcept>
#include <vector>

#include "types.hpp"

namespace stl {

/**
 * Key-value container which holds data in a vector.
 */
template<class T>
class ArrayMap {
public:
    static constexpr usize npos = -1;

    void insert(usize key, T value) { emplace(key, std::move(value)); }

    template<class... Args>
    void emplace(usize key, Args&&... args)
    {
        if (key >= m_idxs.size()) {
            const usize resize = std::max({m_idxs.size() * 2, key + 1, usize(1)});
            m_idxs.resize(resize, npos);
        }

        if (m_idxs[key] != npos) {
            m_data[m_idxs[key]] = T(std::forward<Args>(args)...);
            return;
        }

        m_idxs[key] = m_data.size();
        m_data.emplace_back(std::forward<Args>(args)...);
        m_back_idxs.emplace_back(key);

        ++m_size;
    }

    /**
     * Removes item by taking value from last element and poping last.
     */
    void erase(usize key)
    {
        if (!contains(key))
            throw std::runtime_error{std::format("Invalid key {}", key)};

        usize rm_idx = m_idxs[key];
        T& rm_entry = m_data[rm_idx];

        if (rm_idx < m_data.size() - 1) {
            rm_entry = std::move(m_data[m_data.size() - 1]);
            m_back_idxs[rm_idx] = m_back_idxs[m_back_idxs.size() - 1];
            m_idxs[m_back_idxs[rm_idx]] = rm_idx;
        }

        m_data.pop_back();
        m_back_idxs.pop_back();
        m_idxs[key] = npos;

        --m_size;
    }

    T& operator[](const usize key)
    {
        if (!contains(key))
            throw std::runtime_error{std::format("Invalid key {}", key)};

        return m_data[m_idxs[key]];
    }

    const T& operator[](const usize key) const
    {
        if (!contains(key))
            throw std::runtime_error{std::format("Invalid key {}", key)};

        return m_data[m_idxs[key]];
    }

    [[nodiscard]] bool contains(const usize key) const noexcept
    {
        return key < m_idxs.size() && m_idxs[key] != npos;
    }

    [[nodiscard]] usize size() const noexcept { return m_size; }

    [[nodiscard]] bool empty() const noexcept { return m_size == 0; }

    /**
     * Iterators.
     */
    auto begin() noexcept { return m_data.begin(); }

    auto end() noexcept { return m_data.end(); }

    auto begin() const noexcept { return m_data.begin(); }

    auto end() const noexcept { return m_data.end(); }

private:
    std::vector<T> m_data;
    std::vector<usize> m_idxs;
    std::vector<usize> m_back_idxs;

    usize m_size = 0;
};

} // namespace stl

#endif // ARRAY_MAP
