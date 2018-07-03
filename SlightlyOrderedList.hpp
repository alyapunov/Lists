/*
 * Copyright (c) 2018, Aleksandr Lyapunov
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#pragma once

#include <iterator>

#include <Ring.hpp>

class SlightlyOrderedListLink
{
public:
    SlightlyOrderedListLink() : m_Ring(0) {}
    ~SlightlyOrderedListLink() { }
    SlightlyOrderedListLink(const SlightlyOrderedListLink&) : m_Ring(0) {}
    SlightlyOrderedListLink& operator=(const SlightlyOrderedListLink&) { return *this; }
    bool isAlone() const { return m_Ring.isAlone(); }

    Ring m_Ring;
};

template <class Item, SlightlyOrderedListLink Item::*LinkMember, size_t ItemSize = sizeof(Item)>
class SlightlyOrderedList
{
public:
    SlightlyOrderedList() : m_Ring(0) {}
    ~SlightlyOrderedList() { }

    SlightlyOrderedList(const SlightlyOrderedList&) : m_Ring(0) {}
    SlightlyOrderedList& operator=(const SlightlyOrderedList&)
    {
        m_Ring.remove();
        m_Ring.init();
        m_AddrSum = 0;
        m_Size = 0;
        return *this;
    }

    SlightlyOrderedList(SlightlyOrderedList&& aList) noexcept
    {
        aList.m_Ring.add(&m_Ring);
        aList.m_Ring.remove();
        aList.m_Ring.init();
        std::swap(m_AddrSum, aList.m_AddrSum);
        std::swap(m_Size, aList.m_Size);
    }
    SlightlyOrderedList& operator=(SlightlyOrderedList&& aList) noexcept
    {
        m_Ring.swap(&aList.m_Ring);
        std::swap(m_AddrSum, aList.m_AddrSum);
        std::swap(m_Size, aList.m_Size);
        return *this;
    }

    void insert(Item& aItem)
    {
        uintptr_t sAddr = reinterpret_cast<uintptr_t>(&aItem) >> ADDR_SHIFT;
        m_AddrSum += sAddr;
        ++m_Size;
        m_Ring.add(&((aItem.*LinkMember).m_Ring), sAddr * m_Size > m_AddrSum);
    }
    void remove(Item& aItem)
    {
        uintptr_t sAddr = reinterpret_cast<uintptr_t>(&aItem) >> ADDR_SHIFT;
        m_AddrSum -= sAddr;
        --m_Size;
        (aItem.*LinkMember).m_Ring.remove();
        (aItem.*LinkMember).m_Ring.init();
    }
    bool empty() const
    {
        return m_Ring.isAlone();
    }
    int selfCheck() const
    {
        return m_Ring.selfCheck();
    }
    Item& front()
    {
        return *item(m_Ring.m_Neigh[1]);
    }
    const Item& front() const
    {
        return *item(m_Ring.m_Neigh[1]);
    }
    Item& back()
    {
        return *item(m_Ring.m_Neigh[0]);
    }
    const Item& back() const
    {
        return *item(m_Ring.m_Neigh[0]);
    }

    template <class TItem, class TRing>
    class iterator_common : std::iterator<std::bidirectional_iterator_tag, TItem>
    {
    public:
        explicit iterator_common(TRing* aRing) : m_Ring(aRing) {}
        TItem& operator*() const { return *item(m_Ring); }
        TItem* operator->() const { return item(m_Ring); }
        bool operator==(const iterator_common& aItr) { return m_Ring == aItr.m_Ring; }
        bool operator!=(const iterator_common& aItr) { return m_Ring != aItr.m_Ring; }
        iterator_common& operator++() { m_Ring = m_Ring->m_Neigh[1]; return *this; }
        iterator_common operator++(int) { iterator_common aTmp = *this; m_Ring = m_Ring->m_Neigh[1]; return aTmp; }
        iterator_common& operator--() { m_Ring = m_Ring->m_Neigh[0]; return *this; }
        iterator_common operator--(int) { iterator_common aTmp = *this; m_Ring = m_Ring->m_Neigh[0]; return aTmp; }
    private:
        TRing* m_Ring;
    };
    using iterator = iterator_common<Item, Ring>;
    using const_iterator = iterator_common<const Item, const Ring>;

    iterator begin() { return iterator(m_Ring.m_Neigh[1]); }
    iterator end() { return iterator(&m_Ring); }
    const_iterator begin() const { return const_iterator(m_Ring.m_Neigh[1]); }
    const_iterator end() const { return const_iterator(&m_Ring); }

private:
    Ring m_Ring;
    uintptr_t m_AddrSum = 0;
    size_t m_Size = 0;
    static constexpr int log2(size_t n)
    {
        return ( n == 1 ? 0 : 1 + log2(n / 2));
    }
    static constexpr int ADDR_SHIFT = log2(ItemSize);

    static Item* item(Ring* aLink)
    {
        const uintptr_t sOffset = reinterpret_cast<uintptr_t>(&(reinterpret_cast<Item*>(0)->*LinkMember));
        return reinterpret_cast<Item*>(reinterpret_cast<char*>(aLink) - sOffset);
    }
    static const Item* item(const Ring* aLink)
    {
        const uintptr_t sOffset = reinterpret_cast<uintptr_t>(&(reinterpret_cast<Item*>(0)->*LinkMember));
        return reinterpret_cast<const Item*>(reinterpret_cast<const char*>(aLink) - sOffset);
    }
};