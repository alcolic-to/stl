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

#ifndef STL_ALLOC_HPP
#define STL_ALLOC_HPP

#include <cstddef>
#include <cstring>
#include <new>

#include "types.hpp"

namespace stl {

/**
 * Allocation/deallocation implementations.
 * This implementation is based on libcpp implementation.
 *
 * All memory allocated with allocate() and callocate(), should be deallocated with deallocate()
 * with the same alignment (if align is provided).
 *
 * All memory allocated with allocate_bytes() and callocate_bytes(), should be deallocated with
 * deallocate_bytes() with the same alignment (if align is provided).
 */

/**
 * Check whether align is overalligned for regular new operator.
 * Fundamental alignments are always supported. If alignment is a power of two and not greater than
 * alignof(std::max_align_t), aligned_alloc may simply call std::malloc.
 * https://en.cppreference.com/w/cpp/memory/c/aligned_alloc
 */
inline bool overaligned(size_t align) noexcept
{
    return align > alignof(std::max_align_t);
}

/**
 * Allocates elem_count T elements (with optional alignment).
 */
template<class T>
inline T* allocate(usize elem_count = 1, usize align = alignof(T)) // NOLINT
{
    usize size = elem_count * sizeof(T);
    if (overaligned(align))
        return static_cast<T*>(::operator new(size, static_cast<std::align_val_t>(align)));

    return static_cast<T*>(::operator new(size));
}

/**
 * Allocates elem_count T elements (with optional alignment) and zeros them.
 */
template<class T>
inline T* callocate(usize elem_count = 1, usize align = alignof(T)) // NOLINT
{
    T* ptr = allocate<T>(elem_count, align);
    std::memset(ptr, 0, elem_count * sizeof(T));

    return ptr;
}

/**
 * Deallocates elem_count T elements (with optional alignment).
 */
template<class T>
inline void deallocate(T* ptr, usize elem_count = 1, usize align = alignof(T)) noexcept // NOLINT
{
    usize size = elem_count * sizeof(T);
    if (overaligned(align))
        return ::operator delete(ptr, size, static_cast<std::align_val_t>(align));

    return ::operator delete(ptr, size);
}

/**
 * Allocates bytes (with optional alignment).
 */
inline void* allocate_bytes(usize bytes, usize align = alignof(std::max_align_t)) // NOLINT
{
    if (overaligned(align))
        return ::operator new(bytes, static_cast<std::align_val_t>(align));

    return ::operator new(bytes);
}

/**
 * Allocates bytes (with optional alignment) and zeros them.
 */
inline void* callocate_bytes(usize bytes, usize align = alignof(std::max_align_t)) // NOLINT
{
    void* ptr = allocate_bytes(bytes, align);
    std::memset(ptr, 0, bytes);

    return ptr;
}

/**
 * Deallocates bytes (with optional alignment).
 */
inline void deallocate_bytes(void* ptr, usize align = alignof(std::max_align_t)) noexcept // NOLINT
{
    if (overaligned(align))
        return ::operator delete(ptr, static_cast<std::align_val_t>(align));

    return ::operator delete(ptr); // NOLINT
}

} // namespace stl

#endif // STL_ALLOC_HPP
