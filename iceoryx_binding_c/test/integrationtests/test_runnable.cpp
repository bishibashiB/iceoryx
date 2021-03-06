// Copyright (c) 2020 by Robert Bosch GmbH, Apex.AI Inc. All rights reserved.
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

#include "iceoryx_posh/runtime/posh_runtime.hpp"
#include "testutils/roudi_gtest.hpp"

using namespace iox;
using namespace iox::runtime;

extern "C" {
#include "iceoryx_binding_c/runnable.h"
#include "iceoryx_binding_c/runtime.h"
}

class iox_runnable_test : public RouDi_GTest
{
  public:
    void SetUp()
    {
        iox_runtime_register(m_processName.c_str());
        m_sut = iox_runnable_create(m_runnableName.c_str());
    }

    void TearDown()
    {
        iox_runnable_destroy(m_sut);
    }

    std::string m_runnableName = "hypnotoadIsWatchingUs";
    std::string m_processName = "/stoepselWillMarrySoon";

    iox_runnable_t m_sut;
};

TEST_F(iox_runnable_test, createdRunnableHasCorrectRunnableName)
{
    char name[100];
    ASSERT_EQ(iox_runnable_get_name(m_sut, name, 100), m_runnableName.size());
    EXPECT_EQ(std::string(name), m_runnableName);
}

TEST_F(iox_runnable_test, getRunnableNameBufferIsNullptr)
{
    auto nameLength = iox_runnable_get_name(m_sut, nullptr, 100);

    ASSERT_THAT(nameLength, Eq(0U));
}

TEST_F(iox_runnable_test, getRunnableNameBufferIsLessThanRunnableNameLength)
{
    constexpr uint64_t RUNNABLE_NAME_BUFFER_LENGTH{10};
    char truncatedRunnableName[RUNNABLE_NAME_BUFFER_LENGTH];
    for (auto& c : truncatedRunnableName)
    {
        c = '#';
    }
    auto nameLength = iox_runnable_get_name(m_sut, truncatedRunnableName, RUNNABLE_NAME_BUFFER_LENGTH);

    std::string expectedRunnableName = "hypnotoad";

    ASSERT_THAT(nameLength, Eq(m_runnableName.size()));
    EXPECT_THAT(truncatedRunnableName, StrEq(expectedRunnableName));
}

TEST_F(iox_runnable_test, createdRunnableHasCorrectProcessName)
{
    char name[100];
    ASSERT_EQ(iox_runnable_get_process_name(m_sut, name, 100), m_processName.size());
    EXPECT_EQ(std::string(name), m_processName);
}

TEST_F(iox_runnable_test, getRunnableProcessNameBufferIsNullptr)
{
    auto nameLength = iox_runnable_get_process_name(m_sut, nullptr, 100);

    ASSERT_THAT(nameLength, Eq(0U));
}

TEST_F(iox_runnable_test, getRunnableProcessNameBufferIsLessThanRunnableProcessNameLength)
{
    constexpr uint64_t PROCESS_NAME_BUFFER_LENGTH{10};
    char truncatedProcessName[PROCESS_NAME_BUFFER_LENGTH];
    for (auto& c : truncatedProcessName)
    {
        c = '#';
    }
    auto nameLength = iox_runnable_get_process_name(m_sut, truncatedProcessName, PROCESS_NAME_BUFFER_LENGTH);

    std::string expectedProcessName = "/stoepsel";

    ASSERT_THAT(nameLength, Eq(m_processName.size()));
    EXPECT_THAT(truncatedProcessName, StrEq(expectedProcessName));
}
