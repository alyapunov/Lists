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

#include <cstddef>

struct Ring
{
    Ring* m_Neigh[2]; // generally {m_Prev, m_Next}

    // Makes uninitialized structure.
    Ring() = default;
    // Initializes alone ring.
    Ring(int) : m_Neigh{this, this} {}

    // Copy ctor/assign are not actually implemented and do nothing.
    Ring(const Ring&) {}
    void operator=(const Ring&) {}

    void init()
    {
        m_Neigh[0] = m_Neigh[1] = this;
    };

    // Add new element a to the ring after this (if not aInverted)
    void add(Ring* a, bool aInvert = false)
    {
        link(a, m_Neigh[!aInvert], aInvert);
        link(this, a, aInvert);
    }

    void remove()
    {
        link(m_Neigh[0], m_Neigh[1], false);
    }

    // Add ring a to the ring after this ring and it's elements (if not aInverted)
    void join(Ring* a, bool aInvert = false)
    {
        Ring* s = a->m_Neigh[aInvert];
        link(m_Neigh[aInvert], a, aInvert);
        link(s, this, aInvert);
    }

    // Leave in this ring element from this up to element a (if not aInverted)
    // All other elements forms another ring (with element a).
    void split(Ring* a, bool aInvert = false)
    {
        Ring* s = a->m_Neigh[aInvert];
        link(m_Neigh[aInvert], a, aInvert);
        link(s, this, aInvert);
    }

    void swap(Ring* a)
    {
        join(a, false);
        split(a, true);
    }

    bool isAlone() const
    {
        return this == m_Neigh[0];
    }

    size_t calcSize() const
    {
        size_t sRes = 1;
        for (const Ring* sRing = m_Neigh[0]; this != sRing; sRing = sRing->m_Neigh[0])
            ++sRes;
        return sRes;
    }

    int selfCheck() const
    {
        const Ring* sRing = this;
        do {
            if (sRing != sRing->m_Neigh[1]->m_Neigh[0])
                return 1;
            sRing = sRing->m_Neigh[1];
        } while (sRing != this);
        return 0;
    }

private:
    static void link(Ring* aPrev, Ring* aNext, bool aInvert)
    {
        aPrev->m_Neigh[!aInvert] = aNext;
        aNext->m_Neigh[aInvert] = aPrev;
    }
};
