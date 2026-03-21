/**
 * Copyright 2026, Aleksandar Colic
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

#ifndef STL_VAR_ARRAY_HPP
#define STL_VAR_ARRAY_HPP

#include <algorithm>
#include <bit>
#include <cassert>
#include <cstddef>
#include <memory>
#include <new>
#include <span>
#include <stdexcept>

#include "alloc.hpp"
#include "types.hpp"

namespace stl {

/**
 * Variable size heap array which holds size and elements in the same place.
 * TODO: This is the first try implemenentation - needs testing.
 */
template<typename T>
class VarArray {
    static constexpr usize align = std::max(alignof(VarArray), alignof(T));

    /**
     * Prevents direct construction. Use static functions below for initialization.
     */
    struct Private {
        explicit Private() = default;
    };

public:
    explicit VarArray(Private /* private construction */, usize size) : m_size{size} {}

    VarArray(const VarArray&) = delete;
    VarArray& operator=(const VarArray&) = delete;
    VarArray(VarArray&&) = delete;
    VarArray& operator=(VarArray&&) = delete;

    /**
     * Creates array from provided args. Note that we must not take forwarding (universal) args
     * reference, because data might be moved on first element construction.
     */
    template<typename... Args>
    static std::unique_ptr<VarArray> create(usize size, const Args&... args)
    {
        void* mem = allocate_bytes(sizeof(VarArray) + (sizeof(T) * size), align);
        VarArray* va = std::construct_at(static_cast<VarArray*>(mem), Private{}, size);

        usize ctred = 0;
        try {
            for (; ctred < size; ++ctred)
                std::construct_at(va->data() + ctred, args...);
        }
        catch (...) {
            for (usize i = ctred; i > 0; --i)
                std::destroy_at(va->data() + i - 1);

            deallocate_bytes(va);
            throw;
        }

        return std::unique_ptr<VarArray>{va};
    }

    ~VarArray()
    {
        for (usize i = m_size; i > 0; --i)
            std::destroy_at(data() + i - 1);
    }

    T& operator[](usize idx)
    {
        if (idx >= m_size)
            throw std::runtime_error{"Index out of bounds."};

        return data()[idx];
    }

    const T& operator[](usize idx) const
    {
        if (idx >= m_size)
            throw std::runtime_error{"Index out of bounds."};

        return data()[idx];
    }

    std::span<T> span() noexcept { return {data(), m_size}; }

    std::span<const T> span() const noexcept { return {data(), m_size}; }

    T& front() noexcept { return data()[0]; }

    const T& front() const noexcept { return data()[0]; }

    T& back() noexcept { return data()[m_size - 1]; }

    const T& back() const noexcept { return data()[m_size - 1]; }

    auto begin() noexcept { return data(); }

    auto end() noexcept { return data() + m_size; }

    auto begin() const noexcept { return data(); }

    auto end() const noexcept { return data() + m_size; }

    [[nodiscard]] usize size() const noexcept { return m_size; }

private:
    T* data() noexcept { return std::launder(std::bit_cast<T*>(this + 1)); }

    const T* data() const noexcept { return std::launder(std::bit_cast<const T*>(this + 1)); }

    usize m_size;
    /* T m_data[]; data array that lives after this object. */
};

} // namespace stl

#endif // STL_VAR_ARRAY_HPP
