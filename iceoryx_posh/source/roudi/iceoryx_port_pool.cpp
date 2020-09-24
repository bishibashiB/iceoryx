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

#include "iceoryx_posh/internal/roudi/iceoryx_port_pool.hpp"
#include "iceoryx_posh/iceoryx_posh_types.hpp"
#include "iceoryx_posh/internal/roudi/port_pool_data.hpp"

namespace iox
{
namespace roudi
{
IceOryxPortPool::IceOryxPortPool(PortPoolData& portPoolData) noexcept
    : PortPool(portPoolData)
    , m_portPoolData(&portPoolData)
{
}

/// @deprecated #25
cxx::vector<SenderPortType::MemberType_t*, MAX_PUBLISHERS> IceOryxPortPool::senderPortDataList() noexcept
{
    return getVectorOfPointerFromList(m_portPoolData->m_senderPortMembers);
}

/// @deprecated #25
cxx::vector<ReceiverPortType::MemberType_t*, MAX_SUBSCRIBERS> IceOryxPortPool::receiverPortDataList() noexcept
{
    return getVectorOfPointerFromList(m_portPoolData->m_receiverPortMembers);
}

/// @deprecated #25
cxx::expected<SenderPortType::MemberType_t*, PortPoolError>
IceOryxPortPool::addSenderPort(const capro::ServiceDescription& serviceDescription,
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
IceOryxPortPool::addReceiverPort(const capro::ServiceDescription& serviceDescription,
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
void IceOryxPortPool::removeSenderPort(SenderPortType::MemberType_t* const portDataPtr) noexcept
{
    m_portPoolData->m_senderPortMembers.remove_if(
        [portDataPtr](SenderPortType::MemberType_t& port) { return &port == portDataPtr; });
}

/// @deprecated #25
void IceOryxPortPool::removeReceiverPort(ReceiverPortType::MemberType_t* const portDataPtr) noexcept
{
    m_portPoolData->m_receiverPortMembers.remove_if(
        [portDataPtr](ReceiverPortType::MemberType_t& port) { return &port == portDataPtr; });
}

cxx::vector<PublisherPortRouDiType::MemberType_t*, MAX_PUBLISHERS> IceOryxPortPool::getPublisherPortDataList() noexcept
{
    return getVectorOfPointerFromList(m_portPoolData->m_publisherPortMembers);
}

cxx::vector<SubscriberPortType::MemberType_t*, MAX_SUBSCRIBERS>
IceOryxPortPool::getSubscriberPortDataList() noexcept
{
    return getVectorOfPointerFromList(m_portPoolData->m_subscriberPortMembers);
}

cxx::expected<PublisherPortRouDiType::MemberType_t*, PortPoolError>
IceOryxPortPool::addPublisherPort(const capro::ServiceDescription& serviceDescription,
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
IceOryxPortPool::addSubscriberPort(const capro::ServiceDescription& serviceDescription,
                                   const uint64_t& historyRequest,
                                   const ProcessName_t& applicationName,
                                   const mepoo::MemoryInfo& memoryInfo) noexcept
{
    if (!m_portPoolData->m_subscriberPortMembers.full())
    {
        auto subscriberPortData = &m_portPoolData->m_subscriberPortMembers.emplace_back(
            serviceDescription, applicationName, SUBSCRIBER_PORT_QUEUE_TYPE, historyRequest, memoryInfo);
        return cxx::success<SubscriberPortType::MemberType_t*>(subscriberPortData);
    }
    else
    {
        errorHandler(Error::kPORT_POOL__SUBSCRIBERLIST_OVERFLOW, nullptr, ErrorLevel::MODERATE);
        return cxx::error<PortPoolError>(PortPoolError::SUBSCRIBER_PORT_LIST_FULL);
    }
}

void IceOryxPortPool::removePublisherPort(PublisherPortRouDiType::MemberType_t* const portDataPtr) noexcept
{
    m_portPoolData->m_publisherPortMembers.remove_if([portDataPtr](iox::popo::PublisherPortData& port) {
        return reinterpret_cast<void*>(&port) == reinterpret_cast<const void*>(portDataPtr);
    });
}

void IceOryxPortPool::removeSubscriberPort(SubscriberPortProducerType::MemberType_t* const portDataPtr) noexcept
{
    m_portPoolData->m_subscriberPortMembers.remove_if([portDataPtr](iox::popo::SubscriberPortData& port) {
        return reinterpret_cast<void*>(&port) == reinterpret_cast<const void*>(portDataPtr);
    });
}

template <typename T, uint64_t VectorCapacity>
typename cxx::vector<T*, VectorCapacity>
IceOryxPortPool::getVectorOfPointerFromList(cxx::list<T, VectorCapacity>& rhsList) noexcept
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
