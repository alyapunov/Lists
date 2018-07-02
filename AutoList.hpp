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

#include <cassert>
#include <iterator>

#include <Ring.hpp>

class AutoListLink
{
public:
    AutoListLink() : m_Ring(0) {}
    AutoListLink(const AutoListLink& aLink)
    {
        if (aLink.isAlone())
            m_Ring.init();
        else
            aLink.m_Ring.add(&m_Ring);
    }
    AutoListLink(AutoListLink&& aLink) noexcept
    {
        aLink.m_Ring.add(&m_Ring);
        aLink.m_Ring.remove();
        aLink.m_Ring.init();
    }
    ~AutoListLink()
    {
        m_Ring.remove();
    }
    AutoListLink& operator=(const AutoListLink& aLink)
    {
        m_Ring.remove();
        if (aLink.isAlone())
            m_Ring.init();
        else
            aLink.m_Ring.add(&m_Ring, false);
        return *this;
    }
    AutoListLink& operator=(AutoListLink&& aLink) noexcept
    {
        m_Ring.swap(&aLink.m_Ring);
        return *this;
    }
    bool isAlone() const
    {
        return m_Ring.isAlone();
    }
    void remove()
    {
        m_Ring.remove();
        m_Ring.init();
    }
    int selfCheck() const
    {
        return m_Ring.selfCheck();
    }

    mutable Ring m_Ring;
};

template <class Item, AutoListLink Item::*LinkMember>
class AutoList
{
public:
    AutoList() : m_Ring(0) {}
    ~AutoList()
    {
        m_Ring.remove();
    }

    AutoList(const AutoList&) : m_Ring(0) {}
    AutoList& operator=(const AutoList&)
    {
        m_Ring.remove();
        m_Ring.init();
        return *this;
    }

    AutoList(AutoList&& aList) noexcept
    {
        aList.m_Ring.add(&m_Ring);
        aList.m_Ring.remove();
        aList.m_Ring.init();
    }
    AutoList& operator=(AutoList&& aList) noexcept
    {
        m_Ring.swap(&aList.m_Ring);
        return *this;
    }

    void insertFront(Item& aItem)
    {
        m_Ring.add(&((aItem.*LinkMember).m_Ring), false);
    }
    void insertBack(Item& aItem)
    {
        m_Ring.add(&((aItem.*LinkMember).m_Ring), true);
    }
    void insertAfter(Item& aExistingItem, Item& aNewItem)
    {
        (aExistingItem.*LinkMember).m_Ring.add(&((aNewItem.*LinkMember).m_Ring), false);
    }
    void removeItem(Item& aItem)
    {
        (aItem.*LinkMember).remove();
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