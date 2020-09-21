// Copyright (c) 2019 by Robert Bosch GmbH. All rights reserved.
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
#ifndef IOX_POSH_POPO_RECEIVER_HANDLER_INL
#define IOX_POSH_POPO_RECEIVER_HANDLER_INL

namespace iox
{
namespace popo
{
template <uint32_t MaxReceivers, typename LockingPolicy>
inline ReceiverHandler<MaxReceivers, LockingPolicy>::AppContext::AppContext(ReceiverHandler_t& receiverHandler)
    : m_receiverHandler(receiverHandler)
{
}

template <uint32_t MaxReceivers, typename LockingPolicy>
inline bool ReceiverHandler<MaxReceivers, LockingPolicy>::AppContext::hasLastChunk()
{
    return m_receiverHandler.hasLastChunk();
}

template <uint32_t MaxReceivers, typename LockingPolicy>
inline void ReceiverHandler<MaxReceivers, LockingPolicy>::AppContext::deliverChunk(const mepoo::SharedChunk f_chunk)
{
    m_receiverHandler.deliverChunk(f_chunk);
}

template <uint32_t MaxReceivers, typename LockingPolicy>
inline void ReceiverHandler<MaxReceivers, LockingPolicy>::AppContext::updateLastChunk(const mepoo::SharedChunk f_chunk)
{
    m_receiverHandler.updateLastChunk(f_chunk);
}


template <uint32_t MaxReceivers, typename LockingPolicy>
inline bool ReceiverHandler<MaxReceivers, LockingPolicy>::AppContext::hasReceivers()
{
    return m_receiverHandler.hasReceivers();
}

template <uint32_t MaxReceivers, typename LockingPolicy>
inline void ReceiverHandler<MaxReceivers, LockingPolicy>::AppContext::enableDoDeliverOnSubscription()
{
    m_receiverHandler.enableDoDeliverOnSubscription();
}

template <uint32_t MaxReceivers, typename LockingPolicy>
inline typename ReceiverHandler<MaxReceivers, LockingPolicy>::ReceiverList_t&
ReceiverHandler<MaxReceivers, LockingPolicy>::AppContext::getReceiverList() noexcept
{
    return m_receiverHandler.getReceiverList();
}

template <uint32_t MaxReceivers, typename LockingPolicy>
inline ReceiverHandler<MaxReceivers, LockingPolicy>::RouDiContext::RouDiContext(ReceiverHandler_t& receiverHandler)
    : m_receiverHandler(receiverHandler)
{
}

template <uint32_t MaxReceivers, typename LockingPolicy>
inline bool ReceiverHandler<MaxReceivers, LockingPolicy>::RouDiContext::addNewReceiver(
    ReceiverPortType::MemberType_t* const receiver)
{
    return m_receiverHandler.addNewReceiver(receiver);
}

template <uint32_t MaxReceivers, typename LockingPolicy>
inline void ReceiverHandler<MaxReceivers, LockingPolicy>::RouDiContext::removeReceiver(
    ReceiverPortType::MemberType_t* const receiver)
{
    m_receiverHandler.removeReceiver(receiver);
}

template <uint32_t MaxReceivers, typename LockingPolicy>
inline void ReceiverHandler<MaxReceivers, LockingPolicy>::RouDiContext::removeAll()
{
    m_receiverHandler.removeAll();
}

template <uint32_t MaxReceivers, typename LockingPolicy>
inline void ReceiverHandler<MaxReceivers, LockingPolicy>::updateLastChunk(const mepoo::SharedChunk f_chunk)
{
    lockGuard_t lock(*this);

    if (m_doDeliverOnSubscription.load(std::memory_order_relaxed))
    {
        m_lastChunk = f_chunk;
    }
}

template <uint32_t MaxReceivers, typename LockingPolicy>
inline bool ReceiverHandler<MaxReceivers, LockingPolicy>::hasLastChunk()
{
    lockGuard_t lock(*this);
    return m_lastChunk != nullptr;
}

template <uint32_t MaxReceivers, typename LockingPolicy>
inline bool ReceiverHandler<MaxReceivers, LockingPolicy>::hasReceivers()
{
    lockGuard_t lock(*this);
    return !m_receiverList.empty();
}

template <uint32_t MaxReceivers, typename LockingPolicy>
inline bool ReceiverHandler<MaxReceivers, LockingPolicy>::addNewReceiver(ReceiverPortType::MemberType_t* const receiver)
{
    lockGuard_t lock(*this);
    auto l_alreadyKnownReceiver =
        std::find_if(m_receiverList.begin(), m_receiverList.end(), [&](ReceiverPortType::MemberType_t* receiverInList) {
            return receiverInList == receiver;
        });

    // check if the receiver port is not yet subscribed
    if (l_alreadyKnownReceiver == m_receiverList.end())
    {
        if (m_receiverList.size() < m_receiverList.capacity())
        {
            m_receiverList.push_back(receiver);

            if (m_doDeliverOnSubscription.load(std::memory_order_relaxed))
            {
                if (m_lastChunk != nullptr)
                {
                    ReceiverPortType(receiver).deliver(m_lastChunk);
                }
                else
                {
                    errorHandler(Error::kPOSH__SENDERPORT_FIELD_SUBSCRIBE_WITHOUT_DATA);
                    return false;
                }
            }
        }
        else
        {
            errorHandler(Error::kPOSH__SENDERPORT_SUBSCRIBER_LIST_OVERFLOW);
            return false;
        }
    }

    return true;
}

template <uint32_t MaxReceivers, typename LockingPolicy>
inline void ReceiverHandler<MaxReceivers, LockingPolicy>::removeReceiver(ReceiverPortType::MemberType_t* const receiver)
{
    lockGuard_t lock(*this);
    auto l_iter = std::find(m_receiverList.begin(), m_receiverList.end(), receiver);
    if (l_iter != m_receiverList.end())
    {
        m_receiverList.erase(l_iter);
    }
}

template <uint32_t MaxReceivers, typename LockingPolicy>
inline void ReceiverHandler<MaxReceivers, LockingPolicy>::removeAll()
{
    lockGuard_t lock(*this);
    m_receiverList.clear();
}

template <uint32_t MaxReceivers, typename LockingPolicy>
inline void ReceiverHandler<MaxReceivers, LockingPolicy>::enableDoDeliverOnSubscription()
{
    m_doDeliverOnSubscription.store(true, std::memory_order_relaxed);
}

template <uint32_t MaxReceivers, typename LockingPolicy>
inline bool ReceiverHandler<MaxReceivers, LockingPolicy>::doesDeliverOnSubscribe() const
{
    return m_doDeliverOnSubscription.load(std::memory_order_relaxed);
}

template <uint32_t MaxReceivers, typename LockingPolicy>
inline typename ReceiverHandler<MaxReceivers, LockingPolicy>::ReceiverList_t&
ReceiverHandler<MaxReceivers, LockingPolicy>::getReceiverList() noexcept
{
    return m_receiverList;
}

template <uint32_t MaxReceivers, typename LockingPolicy>
inline uint32_t ReceiverHandler<MaxReceivers, LockingPolicy>::getMaxDeliveryFiFoCapacity()
{
    lockGuard_t lock(*this);

    uint64_t maxDeliveryFiFoCapacity = 0u;

    for (auto receiver : m_receiverList)
    {
        ReceiverPort port(receiver);
        auto deliveryFiFoCapacity = port.getDeliveryFiFoCapacity();
        if (deliveryFiFoCapacity > maxDeliveryFiFoCapacity)
        {
            maxDeliveryFiFoCapacity = deliveryFiFoCapacity;
        }
    }

    return static_cast<uint32_t>(maxDeliveryFiFoCapacity);
}

} // namespace popo
} // namespace iox

#endif // IOX_POSH_POPO_RECEIVER_HANDLER_INL
