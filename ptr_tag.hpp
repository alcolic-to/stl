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

#ifndef STL_PTR_TAG_H
#define STL_PTR_TAG_H

#include <bit>
#include <cassert>
#include <cstddef>

#include "types.hpp"

namespace stl {

/**
 * Pointer tagging.
 * Since malloc's returned address is guaranteed to be aligned at least as std::max_align_t, we will
 * use unused last bits in pointer to store additonal info.
 */
static constexpr uptr tag_bits = alignof(std::max_align_t) - 1;

constexpr uptr raw(const void* ptr) noexcept
{
    return std::bit_cast<uptr>(ptr);
}

constexpr uptr tag(const void* ptr) noexcept
{
    return raw(ptr) & tag_bits;
}

constexpr void* clear_tag(const void* ptr) noexcept
{
    return std::bit_cast<void*>(raw(ptr) & ~tag_bits);
}

constexpr void* set_tag(const void* ptr, uptr tag) noexcept
{
    assert((tag & ~tag_bits) == 0);
    return std::bit_cast<void*>(raw(clear_tag(ptr)) | (tag & tag_bits));
}

} // namespace stl

#endif // STL_PTR_TAG_H
