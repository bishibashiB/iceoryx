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

#include "iceoryx_posh/roudi/port_pool.hpp"
#include "iceoryx_posh/internal/roudi/port_pool_data.hpp"
#include "iceoryx_utils/cxx/string.hpp"

namespace iox
{
namespace roudi
{
PortPool::PortPool(PortPoolDataBase& portPoolDataBase) noexcept
    : m_portPoolDataBase(&portPoolDataBase)
{
}

cxx::vector<popo::InterfacePortData*, MAX_INTERFACE_NUMBER> PortPool::getInterfacePortDataList() noexcept
{
    return getVectorOfPointerFromList(m_portPoolDataBase->m_interfacePortMembers);
}

cxx::vector<popo::ApplicationPortData*, MAX_PROCESS_NUMBER> PortPool::getApplicationPortDataList() noexcept
{
    return getVectorOfPointerFromList(m_portPoolDataBase->m_applicationPortMembers);
}

cxx::vector<runtime::RunnableData*, MAX_RUNNABLE_NUMBER> PortPool::getRunnableDataList() noexcept
{
    return getVectorOfPointerFromList(m_portPoolDataBase->m_runnableMembers);
}

cxx::expected<popo::InterfacePortData*, PortPoolError>
PortPool::addInterfacePort(const std::string& applicationName, const capro::Interfaces interface) noexcept
{
    if (!m_portPoolDataBase->m_interfacePortMembers.full())
    {
        auto interfacePortData = &m_portPoolDataBase->m_interfacePortMembers.emplace_back(
            iox::cxx::string<100>(iox::cxx::TruncateToCapacity, applicationName), interface);
        return cxx::success<popo::InterfacePortData*>(interfacePortData);
    }
    else
    {
        errorHandler(Error::kPORT_POOL__INTERFACELIST_OVERFLOW, nullptr, ErrorLevel::MODERATE);
        return cxx::error<PortPoolError>(PortPoolError::INTERFACE_PORT_LIST_FULL);
    }
}

cxx::expected<popo::ApplicationPortData*, PortPoolError>
PortPool::addApplicationPort(const std::string& applicationName) noexcept
{
    if (!m_portPoolDataBase->m_applicationPortMembers.full())
    {
        auto applicationPortData = &m_portPoolDataBase->m_applicationPortMembers.emplace_back(
            iox::cxx::string<100>(iox::cxx::TruncateToCapacity, applicationName));
        return cxx::success<popo::ApplicationPortData*>(applicationPortData);
    }
    else
    {
        errorHandler(Error::kPORT_POOL__APPLICATIONLIST_OVERFLOW, nullptr, ErrorLevel::MODERATE);
        return cxx::error<PortPoolError>(PortPoolError::APPLICATION_PORT_LIST_FULL);
    }
}

cxx::expected<runtime::RunnableData*, PortPoolError> PortPool::addRunnableData(
    const ProcessName_t& process, const RunnableName_t& runnable, const uint64_t runnableDeviceIdentifier) noexcept
{
    if (!m_portPoolDataBase->m_runnableMembers.full())
    {
        auto runnableData =
            &m_portPoolDataBase->m_runnableMembers.emplace_back(process, runnable, runnableDeviceIdentifier);
        return cxx::success<runtime::RunnableData*>(runnableData);
    }
    else
    {
        errorHandler(Error::kPORT_POOL__RUNNABLELIST_OVERFLOW, nullptr, ErrorLevel::MODERATE);
        return cxx::error<PortPoolError>(PortPoolError::RUNNABLE_DATA_LIST_FULL);
    }
}

cxx::expected<popo::ConditionVariableData*, PortPoolError> PortPool::addConditionVariableData() noexcept
{
    if (!m_portPoolDataBase->m_conditionVariableMembers.full())
    {
        auto conditionVariableData = &m_portPoolDataBase->m_conditionVariableMembers.emplace_back();
        return cxx::success<popo::ConditionVariableData*>(conditionVariableData);
    }
    else
    {
        errorHandler(Error::kPORT_POOL__CONDITION_VARIABLE_LIST_OVERFLOW, nullptr, ErrorLevel::MODERATE);
        return cxx::error<PortPoolError>(PortPoolError::CONDITION_VARIABLE_LIST_FULL);
    }
}

void PortPool::removeInterfacePort(popo::InterfacePortData* const portDataPtr) noexcept
{
    m_portPoolDataBase->m_interfacePortMembers.remove_if(
        [portDataPtr](popo::InterfacePortData& port) { return &port == portDataPtr; });
}

void PortPool::removeApplicationPort(popo::ApplicationPortData* const portDataPtr) noexcept
{
    m_portPoolDataBase->m_applicationPortMembers.remove_if(
        [portDataPtr](popo::ApplicationPortData& port) { return &port == portDataPtr; });
}

void PortPool::removeRunnableData(runtime::RunnableData* const runnableDataPtr) noexcept
{
    m_portPoolDataBase->m_runnableMembers.remove_if(
        [runnableDataPtr](runtime::RunnableData& r) { return &r == runnableDataPtr; });
}

std::atomic<uint64_t>* PortPool::serviceRegistryChangeCounter() noexcept
{
    return &m_portPoolDataBase->m_serviceRegistryChangeCounter;
}

template <typename T, uint64_t VectorCapacity>
typename cxx::vector<T*, VectorCapacity>
PortPool::getVectorOfPointerFromList(cxx::list<T, VectorCapacity>& rhsList) noexcept
{
    cxx::vector<T*, VectorCapacity> portVector;
    for (auto& port : rhsList)
    {
        portVector.emplace_back(&port);
    }
    return portVector;
}

} // namespace roudi
} // namespace iox
