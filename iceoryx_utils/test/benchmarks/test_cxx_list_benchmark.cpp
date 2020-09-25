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

#define ListSizeInTest (1024U)
#define TEST_LIST_ELEMENT_DEFAULT_VALUE (-99L)

class ContainerFixture : public benchmark::Fixture
{
  public:
    static uint64_t cTor;
    static uint64_t customCTor;
    static uint64_t copyCTor;
    static uint64_t moveCTor;
    static uint64_t moveAssignment;
    static uint64_t copyAssignment;
    static uint64_t dTor;
    static int64_t classValue;

    class TestListElement
    {
      public:
        TestListElement()
        {
            cTor++;
            classValue = m_value;
        }

        TestListElement(const int64_t value)
            : m_value(value)
        {
            customCTor++;
            classValue = m_value;
        }

        TestListElement(const TestListElement& rhs)
        {
            copyCTor++;
            m_value = rhs.m_value;
            classValue = m_value;
        }

        TestListElement(TestListElement&& rhs)
        {
            moveCTor++;
            m_value = rhs.m_value;
            classValue = m_value;
        }

        TestListElement& operator=(const TestListElement& rhs)
        {
            copyAssignment++;
            m_value = rhs.m_value;
            classValue = m_value;
            return *this;
        }

        TestListElement& operator=(TestListElement&& rhs)
        {
            moveAssignment++;
            m_value = rhs.m_value;
            classValue = m_value;
            return *this;
        }

        bool operator==(const TestListElement& rhs) const
        {
            return rhs.m_value == m_value;
        }

        ~TestListElement()
        {
            dTor++;
            classValue = m_value;
        }

        int64_t m_value = TEST_LIST_ELEMENT_DEFAULT_VALUE;
    };


    void SetUp()
    {
        cTor = 0U;
        customCTor = 0U;
        copyCTor = 0U;
        moveCTor = 0U;
        moveAssignment = 0U;
        copyAssignment = 0U;
        dTor = 0U;
        classValue = 0U;
    }

    bool isSetupState()
    {
        if (cTor == 0U && customCTor == 0U && copyCTor == 0U && moveCTor == 0U && moveAssignment == 0U
            && copyAssignment == 0U && dTor == 0U && classValue == 0U)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
};

void FreeSetUp()
{
    ContainerFixture::cTor = 0U;
    ContainerFixture::customCTor = 0U;
    ContainerFixture::copyCTor = 0U;
    ContainerFixture::moveCTor = 0U;
    ContainerFixture::moveAssignment = 0U;
    ContainerFixture::copyAssignment = 0U;
    ContainerFixture::dTor = 0U;
    ContainerFixture::classValue = 0L;
}

// ContainerFixture statics
uint64_t ContainerFixture::cTor;
uint64_t ContainerFixture::customCTor;
uint64_t ContainerFixture::copyCTor;
uint64_t ContainerFixture::moveCTor;
uint64_t ContainerFixture::moveAssignment;
uint64_t ContainerFixture::copyAssignment;
uint64_t ContainerFixture::dTor;
int64_t ContainerFixture::classValue;

template <typename ContainerType>
void fillContainer(ContainerType& c, uint64_t fillCnt)
{
    for (uint64_t k = 0; k < fillCnt; ++k)
    {
        benchmark::DoNotOptimize(c.emplace_back(k));
        benchmark::ClobberMemory();
    }
}


template <typename ContainerType>
typename ContainerType::iterator iterateContainer(ContainerType& c, int64_t cnt)
{
    auto iter = c.begin();
    for (int64_t l = 0; l < cnt; ++l)
    {
        benchmark::DoNotOptimize(++iter);
        benchmark::ClobberMemory();
    }
    return iter;
}
template <typename ContainerType>
typename ContainerType::iterator iterateContainer(ContainerType& c)
{
    return iterateContainer(c, c.size());
}

template <typename ContainerTypeFrom, typename ContainerTypeTo>
void readAndCopyPointerOfContainerElem(ContainerTypeFrom& frC, ContainerTypeTo& toC)
{
    for (auto& el : frC)
    {
        benchmark::DoNotOptimize(toC.emplace_back(&el));
        benchmark::DoNotOptimize(toC.back());
        benchmark::ClobberMemory();
    }
}

template <typename ContainerTypeFrom, typename ContainerTypeTo>
static void BM_typedListRead(benchmark::State& st)
{
    ContainerTypeTo v;
    ContainerTypeFrom testContainer;
    fillContainer<ContainerTypeFrom>(testContainer, st.range(0));
    for (auto _ : st)
    {
        readAndCopyPointerOfContainerElem(testContainer, v);
    }
}
BENCHMARK_TEMPLATE(BM_typedListRead, iox::cxx::vector<int, ListSizeInTest>, iox::cxx::vector<int*, ListSizeInTest>)
    ->Arg(ListSizeInTest);
// BENCHMARK_TEMPLATE(BM_typedListRead,
//                    iox::cxx::vector<ContainerFixture::TestListElement, ListSizeInTest>,
//                    iox::cxx::vector<ContainerFixture::TestListElement*, ListSizeInTest>)
//     ->Arg(ListSizeInTest);
BENCHMARK_TEMPLATE(BM_typedListRead, iox::cxx::list<int, ListSizeInTest>, iox::cxx::vector<int*, ListSizeInTest>)
    ->Arg(ListSizeInTest);


template <typename ContainerType>
static void BM_typedListIncr(benchmark::State& st)
{
    ContainerType testContainer;
    fillContainer<ContainerType>(testContainer, st.range(0));
    for (auto _ : st)
    {
        iterateContainer(testContainer);
    }
}
BENCHMARK_TEMPLATE(BM_typedListIncr, iox::cxx::vector<int, ListSizeInTest>)->Arg(ListSizeInTest);
BENCHMARK_TEMPLATE(BM_typedListIncr, iox::cxx::vector<ContainerFixture::TestListElement, ListSizeInTest>)
    ->Arg(ListSizeInTest);
BENCHMARK_TEMPLATE(BM_typedListIncr, iox::cxx::list<int, ListSizeInTest>)->Arg(ListSizeInTest);


template <typename ContainerType>
static void BM_typedListRemove(benchmark::State& st)
{
    ContainerType testContainer;
    fillContainer<ContainerType>(testContainer, st.range(0));
    testContainer.pop_back();

    for (auto _ : st)
    {
        st.PauseTiming();
        FreeSetUp();
        testContainer.emplace_back(42);
        auto iter = iterateContainer(testContainer, st.range(1));
        st.ResumeTiming();

        benchmark::DoNotOptimize(testContainer.erase(iter));
        benchmark::ClobberMemory();
    }
    st.counters["customCTor"] = ContainerFixture::customCTor;
    st.counters["dTor"] = ContainerFixture::dTor;
    st.counters["copyCTor"] = ContainerFixture::copyCTor;
    st.counters["moveCTor"] = ContainerFixture::moveCTor;
    st.counters["copyAssignment"] = ContainerFixture::copyAssignment;
    st.counters["moveAssignment"] = ContainerFixture::moveAssignment;
}
BENCHMARK_TEMPLATE(BM_typedListRemove, iox::cxx::vector<int, ListSizeInTest>)
    ->Args({ListSizeInTest, 1U})
    ->Args({ListSizeInTest, ListSizeInTest / 2U})
    ->Args({ListSizeInTest, ListSizeInTest - 1U});
BENCHMARK_TEMPLATE(BM_typedListRemove, iox::cxx::vector<ContainerFixture::TestListElement, ListSizeInTest>)
    ->Args({ListSizeInTest, 1U})
    ->Args({ListSizeInTest, ListSizeInTest / 2U})
    ->Args({ListSizeInTest, ListSizeInTest - 1U});
BENCHMARK_TEMPLATE(BM_typedListRemove, iox::cxx::list<int, ListSizeInTest>)
    ->Args({ListSizeInTest, 1U})
    ->Args({ListSizeInTest, ListSizeInTest / 2U})
    ->Args({ListSizeInTest, ListSizeInTest - 1U});
BENCHMARK_TEMPLATE(BM_typedListRemove, iox::cxx::list<ContainerFixture::TestListElement, ListSizeInTest>)
    ->Args({ListSizeInTest, 1U})
    ->Args({ListSizeInTest, ListSizeInTest / 2U})
    ->Args({ListSizeInTest, ListSizeInTest - 1U});

BENCHMARK_MAIN();
