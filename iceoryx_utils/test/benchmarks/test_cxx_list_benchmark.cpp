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

using namespace iox::cxx;

static void BM_ListRead(benchmark::State& st)
{
    for (auto _ : st)
    {
        st.PauseTiming();
        vector<int*, 256> v;
        list<int, 256> testContainer;
        for (uint64_t k = 0; k < testContainer.capacity(); ++k)
        {
            testContainer.emplace_back(k);
        }
        st.ResumeTiming();

        for (auto& el : testContainer)
        {
            benchmark::DoNotOptimize(v.emplace_back(&el));
        }
    }
}
BENCHMARK(BM_ListRead);

static void BM_VectorRead(benchmark::State& st)
{
    for (auto _ : st)
    {
        st.PauseTiming();
        vector<int*, 256> v;
        vector<int, 256> testContainer;
        for (uint64_t k = 0; k < testContainer.capacity(); ++k)
        {
            testContainer.emplace_back(k);
        }
        st.ResumeTiming();

        for (auto& el : testContainer)
        {
            benchmark::DoNotOptimize(v.emplace_back(&el));
        }
    }
}
BENCHMARK(BM_VectorRead);

BENCHMARK_MAIN();
