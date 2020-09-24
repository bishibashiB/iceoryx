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
PortPool::PortPool(PortPoolData& portPoolData) noexcept
    : m_portPoolData(&portPoolData)
{
}

cxx::vector<popo::InterfacePortData*, MAX_INTERFACE_NUMBER> PortPool::getInterfacePortDataList() noexcept
{
    return getVectorOfPointerFromList(m_portPoolData->m_interfacePortMembers);
}

cxx::vector<popo::ApplicationPortData*, MAX_PROCESS_NUMBER> PortPool::getApplicationPortDataList() noexcept
{
    return getVectorOfPointerFromList(m_portPoolData->m_applicationPortMembers);
}

cxx::vector<runtime::RunnableData*, MAX_RUNNABLE_NUMBER> PortPool::getRunnableDataList() noexcept
{
    return getVectorOfPointerFromList(m_portPoolData->m_runnableMembers);
}

cxx::expected<popo::InterfacePortData*, PortPoolError>
PortPool::addInterfacePort(const std::string& applicationName, const capro::Interfaces interface) noexcept
{
    if (!m_portPoolData->m_interfacePortMembers.full())
    {
        auto interfacePortData = &m_portPoolData->m_interfacePortMembers.emplace_back(
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
    if (!m_portPoolData->m_applicationPortMembers.full())
    {
        auto applicationPortData = &m_portPoolData->m_applicationPortMembers.emplace_back(
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
    if (!m_portPoolData->m_runnableMembers.full())
    {
        auto runnableData =
            &m_portPoolData->m_runnableMembers.emplace_back(process, runnable, runnableDeviceIdentifier);
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
    if (!m_portPoolData->m_conditionVariableMembers.full())
    {
        auto conditionVariableData = &m_portPoolData->m_conditionVariableMembers.emplace_back();
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
    m_portPoolData->m_interfacePortMembers.remove_if(
        [portDataPtr](popo::InterfacePortData& port) { return &port == portDataPtr; });
}

void PortPool::removeApplicationPort(popo::ApplicationPortData* const portDataPtr) noexcept
{
    m_portPoolData->m_applicationPortMembers.remove_if(
        [portDataPtr](popo::ApplicationPortData& port) { return &port == portDataPtr; });
}

void PortPool::removeRunnableData(runtime::RunnableData* const runnableDataPtr) noexcept
{
    m_portPoolData->m_runnableMembers.remove_if(
        [runnableDataPtr](runtime::RunnableData& r) { return &r == runnableDataPtr; });
}

std::atomic<uint64_t>* PortPool::serviceRegistryChangeCounter() noexcept
{
    return &m_portPoolData->m_serviceRegistryChangeCounter;
}

/// @deprecated #25
cxx::vector<SenderPortType::MemberType_t*, MAX_PUBLISHERS> PortPool::senderPortDataList() noexcept
{
    return getVectorOfPointerFromList(m_portPoolData->m_senderPortMembers);
}

/// @deprecated #25
cxx::vector<ReceiverPortType::MemberType_t*, MAX_SUBSCRIBERS> PortPool::receiverPortDataList() noexcept
{
    return getVectorOfPointerFromList(m_portPoolData->m_receiverPortMembers);
}

/// @deprecated #25
cxx::expected<SenderPortType::MemberType_t*, PortPoolError>
PortPool::addSenderPort(const capro::ServiceDescription& serviceDescription,
                        mepoo::MemoryManager* const memoryManager,
                        const std::string& applicationName,
                        const mepoo::MemoryInfo& memoryInfo) noexcept
{
    if (!m_portPoolData->m_senderPortMembers.full())
    {
        auto senderPortData = &m_portPoolData->m_senderPortMembers.emplace_back(
            serviceDescription, memoryManager, applicationName, memoryInfo);
        return cxx::success<SenderPortType::MemberType_t*>(senderPortData);
    }
    else
    {
        errorHandler(Error::kPORT_POOL__SENDERLIST_OVERFLOW, nullptr, ErrorLevel::MODERATE);
        return cxx::error<PortPoolError>(PortPoolError::SENDER_PORT_LIST_FULL);
    }
}

/// @deprecated #25
cxx::expected<ReceiverPortType::MemberType_t*, PortPoolError>
PortPool::addReceiverPort(const capro::ServiceDescription& serviceDescription,
                          const std::string& applicationName,
                          const mepoo::MemoryInfo& memoryInfo) noexcept
{
    if (!m_portPoolData->m_receiverPortMembers.full())
    {
        auto receiverPortData =
            &m_portPoolData->m_receiverPortMembers.emplace_back(serviceDescription, applicationName, memoryInfo);
        return cxx::success<ReceiverPortType::MemberType_t*>(receiverPortData);
    }
    else
    {
        errorHandler(Error::kPORT_POOL__RECEIVERLIST_OVERFLOW, nullptr, ErrorLevel::MODERATE);
        return cxx::error<PortPoolError>(PortPoolError::RECEIVER_PORT_LIST_FULL);
    }
}

/// @deprecated #25
void PortPool::removeSenderPort(SenderPortType::MemberType_t* const portDataPtr) noexcept
{
    m_portPoolData->m_senderPortMembers.remove_if(
        [portDataPtr](SenderPortType::MemberType_t& port) { return &port == portDataPtr; });
}

/// @deprecated #25
void PortPool::removeReceiverPort(ReceiverPortType::MemberType_t* const portDataPtr) noexcept
{
    m_portPoolData->m_receiverPortMembers.remove_if(
        [portDataPtr](ReceiverPortType::MemberType_t& port) { return &port == portDataPtr; });
}

cxx::vector<PublisherPortRouDiType::MemberType_t*, MAX_PUBLISHERS> PortPool::getPublisherPortDataList() noexcept
{
    return getVectorOfPointerFromList(m_portPoolData->m_publisherPortMembers);
}

cxx::vector<SubscriberPortType::MemberType_t*, MAX_SUBSCRIBERS> PortPool::getSubscriberPortDataList() noexcept
{
    return getVectorOfPointerFromList(m_portPoolData->m_subscriberPortMembers);
}

cxx::expected<PublisherPortRouDiType::MemberType_t*, PortPoolError>
PortPool::addPublisherPort(const capro::ServiceDescription& serviceDescription,
                           const uint64_t& historyCapacity,
                           mepoo::MemoryManager* const memoryManager,
                           const ProcessName_t& applicationName,
                           const mepoo::MemoryInfo& memoryInfo) noexcept
{
    if (!m_portPoolData->m_publisherPortMembers.full())
    {
        auto publisherPortData = &m_portPoolData->m_publisherPortMembers.emplace_back(
            serviceDescription, applicationName, memoryManager, historyCapacity, memoryInfo);
        return cxx::success<PublisherPortRouDiType::MemberType_t*>(publisherPortData);
    }
    else
    {
        errorHandler(Error::kPORT_POOL__PUBLISHERLIST_OVERFLOW, nullptr, ErrorLevel::MODERATE);
        return cxx::error<PortPoolError>(PortPoolError::PUBLISHER_PORT_LIST_FULL);
    }
}

cxx::expected<SubscriberPortType::MemberType_t*, PortPoolError>
PortPool::addSubscriberPort(const capro::ServiceDescription& serviceDescription,
                            const uint64_t& historyRequest,
                            const ProcessName_t& applicationName,
                            const mepoo::MemoryInfo& memoryInfo) noexcept
{
    if (!m_portPoolData->m_subscriberPortMembers.full())
    {
        auto subscriberPortData = constructSubscriber<iox::build::CommunicationPolicy>(
            serviceDescription, historyRequest, applicationName, memoryInfo);

        return cxx::success<SubscriberPortType::MemberType_t*>(subscriberPortData);
    }
    else
    {
        errorHandler(Error::kPORT_POOL__SUBSCRIBERLIST_OVERFLOW, nullptr, ErrorLevel::MODERATE);
        return cxx::error<PortPoolError>(PortPoolError::SUBSCRIBER_PORT_LIST_FULL);
    }
}

void PortPool::removePublisherPort(PublisherPortRouDiType::MemberType_t* const portDataPtr) noexcept
{
    m_portPoolData->m_publisherPortMembers.remove_if(
        [portDataPtr](PublisherPortRouDiType::MemberType_t& port) { return &port == portDataPtr; });
}

void PortPool::removeSubscriberPort(SubscriberPortType::MemberType_t* const portDataPtr) noexcept
{
    m_portPoolData->m_subscriberPortMembers.remove_if(
        [portDataPtr](SubscriberPortType::MemberType_t& port) { return &port == portDataPtr; });
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
