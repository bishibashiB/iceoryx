// Copyright (c) 2020 by Robert Bosch GmbH. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

//#include "test.hpp"
#include "../../../build/dependencies/gbenchmark/src/include/benchmark/benchmark.h"
#include "iceoryx_utils/cxx/list.hpp"
#include "iceoryx_utils/cxx/vector.hpp"
#include <list>
#include <vector>

#define ListSizeInTest 256U


template <typename ContainerType>
void fillContainer(ContainerType& c, uint64_t fillCnt)
{
    for (uint64_t k = 0; k < fillCnt; ++k)
    {
        c.emplace_back(k);
    }
}

template <typename ContainerType>
void iterateContainer(ContainerType& c)
{
    auto iter = c.begin();
    for (uint l = 1; l < c.size(); ++l)
    {
        benchmark::DoNotOptimize(++iter);
        benchmark::ClobberMemory();
    }
}

template <typename ContainerTypeFrom, typename ContainerTypeTo>
void readAndCopyPointerOfContainerElem(ContainerTypeFrom& frC, ContainerTypeTo& toC)
{
    for (auto& el : frC)
    {
        toC.emplace_back(&el);
        benchmark::DoNotOptimize(toC.back());
        benchmark::ClobberMemory();
    }
}

template <typename ContainerTypeFrom, typename ContainerTypeTo>
static void BM_typedListRead(benchmark::State& st)
{
    for (auto _ : st)
    {
        st.PauseTiming();
        ContainerTypeTo v;
        ContainerTypeFrom testContainer;
        fillContainer<ContainerTypeFrom>(testContainer, st.range(0));
        st.ResumeTiming();

        readAndCopyPointerOfContainerElem(testContainer, v);
    }
}
BENCHMARK_TEMPLATE(BM_typedListRead, iox::cxx::vector<int, ListSizeInTest>, iox::cxx::vector<int*, ListSizeInTest>)
    ->Arg(ListSizeInTest);
BENCHMARK_TEMPLATE(BM_typedListRead, std::list<int>, iox::cxx::vector<int*, ListSizeInTest>)->Arg(ListSizeInTest);
BENCHMARK_TEMPLATE(BM_typedListRead, iox::cxx::list<int, ListSizeInTest>, iox::cxx::vector<int*, ListSizeInTest>)
    ->Arg(ListSizeInTest);


template <typename ContainerType>
static void BM_typedListIncr(benchmark::State& st)
{
    for (auto _ : st)
    {
        st.PauseTiming();
        ContainerType testContainer;
        fillContainer<ContainerType>(testContainer, st.range(0));
        st.ResumeTiming();

        iterateContainer(testContainer);
    }
}
BENCHMARK_TEMPLATE(BM_typedListIncr, iox::cxx::vector<int, ListSizeInTest>)->Arg(ListSizeInTest);
BENCHMARK_TEMPLATE(BM_typedListIncr, std::list<int>)->Arg(ListSizeInTest);
BENCHMARK_TEMPLATE(BM_typedListIncr, iox::cxx::list<int, ListSizeInTest>)->Arg(ListSizeInTest);


template <typename ContainerType>
static void BM_typedListRemove(benchmark::State& st)
{
    for (auto _ : st)
    {
        st.PauseTiming();
        ContainerType testContainer;
        fillContainer<ContainerType>(testContainer, st.range(0));

        auto iter = testContainer.begin();
        for (int64_t k; k < st.range(1); ++k)
        {
            ++iter;
        }
        st.ResumeTiming();

        benchmark::DoNotOptimize(testContainer.erase(iter));
        benchmark::ClobberMemory();
    }
}
BENCHMARK_TEMPLATE(BM_typedListRemove, iox::cxx::vector<int, ListSizeInTest>)
    ->Args({ListSizeInTest, 1U})
    ->Args({ListSizeInTest, ListSizeInTest / 2U})
    ->Args({ListSizeInTest, ListSizeInTest - 1U});
BENCHMARK_TEMPLATE(BM_typedListRemove, std::list<int>)
    ->Args({ListSizeInTest, 1U})
    ->Args({ListSizeInTest, ListSizeInTest / 2U})
    ->Args({ListSizeInTest, ListSizeInTest - 1U});
BENCHMARK_TEMPLATE(BM_typedListRemove, iox::cxx::list<int, ListSizeInTest>)
    ->Args({ListSizeInTest, 1U})
    ->Args({ListSizeInTest, ListSizeInTest / 2U})
    ->Args({ListSizeInTest, ListSizeInTest - 1U});

template <typename ContainerType>
static void BM_typedListIterateAndRemove(benchmark::State& st)
{
    for (auto _ : st)
    {
        st.PauseTiming();
        ContainerType testContainer;
        fillContainer<ContainerType>(testContainer, st.range(0));
        st.ResumeTiming();

        auto iter = testContainer.begin();
        for (int64_t k; k < st.range(1); ++k)
        {
            benchmark::DoNotOptimize(++iter);
            benchmark::ClobberMemory();
        }

        benchmark::DoNotOptimize(testContainer.erase(iter));
        benchmark::ClobberMemory();
    }
}

BENCHMARK_TEMPLATE(BM_typedListIterateAndRemove, iox::cxx::vector<int, ListSizeInTest>)
    ->Args({ListSizeInTest, 1U})
    ->Args({ListSizeInTest, ListSizeInTest / 2U})
    ->Args({ListSizeInTest, ListSizeInTest - 1U});
BENCHMARK_TEMPLATE(BM_typedListIterateAndRemove, std::list<int>)
    ->Args({ListSizeInTest, 1U})
    ->Args({ListSizeInTest, ListSizeInTest / 2U})
    ->Args({ListSizeInTest, ListSizeInTest - 1U});
BENCHMARK_TEMPLATE(BM_typedListIterateAndRemove, iox::cxx::list<int, ListSizeInTest>)
    ->Args({ListSizeInTest, 1U})
    ->Args({ListSizeInTest, ListSizeInTest / 2U})
    ->Args({ListSizeInTest, ListSizeInTest - 1U});

BENCHMARK_MAIN();
