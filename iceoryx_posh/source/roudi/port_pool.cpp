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

cxx::list<popo::InterfacePortData, MAX_INTERFACE_NUMBER>& PortPool::interfacePortDataList() noexcept
{
    return m_portPoolDataBase->m_interfacePortMembers;
}

cxx::list<popo::ApplicationPortData, MAX_PROCESS_NUMBER>& PortPool::appliactionPortDataList() noexcept
{
    return m_portPoolDataBase->m_applicationPortMembers;
}

cxx::list<runtime::RunnableData, MAX_RUNNABLE_NUMBER>& PortPool::runnableDataList() noexcept
{
    return m_portPoolDataBase->m_runnableMembers;
}

cxx::expected<popo::InterfacePortData*, PortPoolError>
PortPool::addInterfacePort(const std::string& applicationName, const capro::Interfaces interface) noexcept
{
    if (!m_portPoolDataBase->m_interfacePortMembers.full())
    {
        auto interfacePortData = &m_portPoolDataBase->m_interfacePortMembers.emplace_front(
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
        auto applicationPortData = &m_portPoolDataBase->m_applicationPortMembers.emplace_front(
            iox::cxx::string<100>(iox::cxx::TruncateToCapacity, applicationName));
        return cxx::success<popo::ApplicationPortData*>(applicationPortData);
    }
    else
    {
        errorHandler(Error::kPORT_POOL__APPLICATIONLIST_OVERFLOW, nullptr, ErrorLevel::MODERATE);
        return cxx::error<PortPoolError>(PortPoolError::APPLICATION_PORT_LIST_FULL);
    }
}

cxx::expected<runtime::RunnableData*, PortPoolError>
PortPool::addRunnableData(const iox::cxx::CString100& process,
                          const iox::cxx::CString100& runnable,
                          const uint64_t runnableDeviceIdentifier) noexcept
{
    if (!m_portPoolDataBase->m_runnableMembers.full())
    {
        auto runnableData =
            &m_portPoolDataBase->m_runnableMembers.emplace_front(process, runnable, runnableDeviceIdentifier);
        return cxx::success<runtime::RunnableData*>(runnableData);
    }
    else
    {
        errorHandler(Error::kPORT_POOL__RUNNABLELIST_OVERFLOW, nullptr, ErrorLevel::MODERATE);
        return cxx::error<PortPoolError>(PortPoolError::RUNNABLE_DATA_LIST_FULL);
    }
}

void PortPool::removeInterfacePort(popo::InterfacePortData* const portData) noexcept
{
    m_portPoolDataBase->m_interfacePortMembers.remove_if(
        [&](popo::InterfacePortData& port) { return &port == portData; });
}

void PortPool::removeApplicationPort(popo::ApplicationPortData* const portData) noexcept
{
    m_portPoolDataBase->m_applicationPortMembers.remove_if(
        [&](popo::ApplicationPortData& port) { return &port == portData; });
}

void PortPool::removeRunnableData(runtime::RunnableData* const runnableData) noexcept
{
    m_portPoolDataBase->m_runnableMembers.remove_if([&](runtime::RunnableData& r) { return &r == runnableData; });
}

std::atomic<uint64_t>* PortPool::serviceRegistryChangeCounter() noexcept
{
    return &m_portPoolDataBase->m_serviceRegistryChangeCounter;
}

} // namespace roudi
} // namespace iox
