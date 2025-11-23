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

/**
 * Playground for STL.
 */
#include <algorithm>
#include <iostream>
#include <ranges>
#include <string>
#include <vector>

#include "intrusive_list.hpp"

using namespace stl;

struct A {
    INode m_node;
    std::string m_s;
};

int main(int argc, char** argv)
{
    std::cout << "Hello from STL!\n";

    A a1{.m_s = "String 1"};
    A a2{.m_s = "String 2"};
    A a3{.m_s = "String 3"};
    A a4{.m_s = "String 4"};
    A a5{.m_s = "String 5"};

    IList<A, offsetof(A, m_node)> list;

    list.push_back(&a1);
    list.push_back(&a2);
    list.push_back(&a3);
    list.push_back(&a4);
    list.push_back(&a5);

    usize c = 0;
    for (const auto& node : list)
        std::cout << (c++ ? ", " : "") << node.m_s;
    std::cout << "\n";
}
