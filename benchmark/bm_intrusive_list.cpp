// NOLINTBEGIN

#include <atomic>
#include <benchmark/benchmark.h>
#include <chrono>
#include <deque>
#include <intrusive_list.hpp>
#include <iostream>
#include <list>
#include <memory>

using namespace stl;

struct Data {
    Data() = default;

    Data(int i) : m_value{i} {}

    INode<Data> m_node;
    int m_value;
};

static void BM_list_push_back(benchmark::State& state)
{
    std::vector<Data> data;
    data.reserve(state.range(0));
    for (int i = 0; i < state.range(0); ++i)
        data.emplace_back(i);

    std::list<Data*> list;

    for (auto _ : state) {
        for (int i = 0; i < state.range(0); ++i)
            list.push_back(&data[i]);
        benchmark::ClobberMemory();
    }

    [[maybe_unused]] usize c = 0;
    for (const auto& it : list)
        c += it->m_value;

    benchmark::ClobberMemory();
}

BENCHMARK(BM_list_push_back)->Range(1, 1024 * 1024);

static void BM_deque_push_back(benchmark::State& state)
{
    std::vector<Data> data;
    data.reserve(state.range(0));
    for (int i = 0; i < state.range(0); ++i)
        data.emplace_back(i);

    std::deque<Data*> deque;

    for (auto _ : state) {
        for (int i = 0; i < state.range(0); ++i)
            deque.push_back(&data[i]);
        benchmark::ClobberMemory();
    }

    [[maybe_unused]] usize c = 0;
    for (const auto& it : deque)
        c += it->m_value;

    benchmark::ClobberMemory();
}

BENCHMARK(BM_deque_push_back)->Range(1, 1024 * 1024);

static void BM_intrusive_list_push_back(benchmark::State& state)
{
    std::vector<Data> data;
    data.reserve(state.range(0));
    for (int i = 0; i < state.range(0); ++i)
        data.emplace_back(i);

    IList<Data, &Data::m_node> list;

    for (auto _ : state) {
        for (int i = 0; i < state.range(0); ++i)
            list.push_back(data[i]);
        benchmark::ClobberMemory();
    }

    [[maybe_unused]] usize c = 0;
    for (const auto& it : list)
        c += it.m_value;

    benchmark::ClobberMemory();
}

BENCHMARK(BM_intrusive_list_push_back)->Range(1, 1024 * 1024);

BENCHMARK_MAIN();

// NOLINTEND
