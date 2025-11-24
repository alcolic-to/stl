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

#ifndef STL_HEAP_ARRAY_H
#define STL_HEAP_ARRAY_H

#include <cstdlib>
#include <memory>
#include <stdexcept>

#include "types.hpp"

namespace stl {

// NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic)

/**
 * Stable vector is a vector-like class that has fixed capacity which must be provided at
 * construction. Elements are never moved, hence only push/pop back functions are provided.
 * Data is always contiguous.
 * If class T is not copyable/moveable, you should use emplace_back.
 */
template<class T>
class StableVector {
public:
    explicit StableVector(usize capacity)
        : m_data{static_cast<T*>(std::malloc(sizeof(T) * capacity))} // NOLINT
        , m_capacity{capacity}
    {
    }

    ~StableVector()
    {
        std::destroy_n(m_data, m_size);
        std::free(m_data); // NOLINT
    }

    /* TODO: Implement these. */
    StableVector(const StableVector& other) = delete;
    StableVector& operator=(const StableVector& other) = delete;

    StableVector(StableVector&& other) noexcept = delete;
    StableVector& operator=(StableVector&& other) noexcept = delete;

    void push_back(T value) { emplace_back(std::move(value)); }

    template<class... Args>
    void emplace_back(Args&&... args)
    {
        if (m_size >= m_capacity)
            throw std::runtime_error{"Vector is full."};

        std::construct_at(&m_data[m_size++], std::forward<Args>(args)...);
    }

    void pop_back()
    {
        if (empty())
            throw std::runtime_error{"Vector is empty."};

        std::destroy_at(&m_data[--m_size]);
    }

    [[nodiscard]] constexpr T& operator[](usize idx)
    {
        if (idx >= m_size)
            throw std::runtime_error{"Index out of bounds."};

        return m_data[idx];
    }

    [[nodiscard]] constexpr const T& operator[](usize idx) const
    {
        if (idx >= m_size)
            throw std::runtime_error{"Index out of bounds."};

        return m_data[idx];
    }

    T* data() { return m_data; }

    const T* data() const { return m_data; }

    T& front() { return m_data[0]; }

    const T& front() const { return m_data[0]; }

    T& back() { return m_data[m_size - 1]; }

    const T& back() const { return m_data[m_size - 1]; }

    auto begin() { return m_data; }

    auto end() { return m_data + m_size; }

    auto begin() const { return m_data; }

    auto end() const { return m_data + m_size; }

    [[nodiscard]] usize size() { return m_size; }

    [[nodiscard]] usize capacity() { return m_capacity; }

    [[nodiscard]] bool empty() { return size() == 0; }

private:
    T* m_data; // NOLINT
    usize m_size = 0;
    usize m_capacity;
};

// NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic)

} // namespace stl

#endif // STL_HEAP_ARRAY_H
