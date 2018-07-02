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
#include <Ring.hpp>

#include <chrono>
#include <iostream>

// Helpers
static size_t SideEffect = 0;

static void checkpoint(const char* aText, size_t aOpCount)
{
    using namespace std::chrono;
    high_resolution_clock::time_point now = high_resolution_clock::now();
    static high_resolution_clock::time_point was;
    duration<double> time_span = duration_cast<duration<double>>(now - was);
    if (0 != aOpCount)
    {
        double Mrps = aOpCount / 1000000. / time_span.count();
        std::cout << aText << ": " << Mrps << " Mrps" << std::endl;
    }
    was = now;
}

static int rand8()
{
    static int the_rand = rand();
    static int s = 0;
    s = (s + 1) % 16;
    return (the_rand >> s) % 8;
}

struct Test : Ring
{
    int m_Num;
    explicit Test(int aNum = 0) : Ring(0), m_Num(aNum) {}
};

static void ops()
{
    size_t COUNT = 1024 * 1024 * 128;

    Test root;
    Test r[8];
    for (int i = 0; i < 8; i++)
    {
        r[i].m_Num = i + 1;
        root.add(&r[i], true);
    }
    checkpoint("", 0);

    for (size_t i = 0; i < COUNT; i++)
    {
        SideEffect += rand8();
    }
    checkpoint("random", COUNT);

    for (size_t i = 0; i < COUNT; i++)
    {
        Test* p = &r[rand8()];
        p->remove();
        root.add(p, false);
    }
    checkpoint("random remove + addA", COUNT);

    for (size_t i = 0; i < COUNT; i++)
    {
        Test* p = &r[rand8()];
        p->remove();
        root.add(p, true);
    }
    checkpoint("random remove + addB", COUNT);

    for (size_t i = 0; i < COUNT; i++)
    {
        Test* p = &r[i % 8];
        p->remove();
        root.add(p, false);
    }
    checkpoint("sequent remove + addA", COUNT);

    for (size_t i = 0; i < COUNT; i++)
    {
        Test* p = &r[i % 8];
        p->remove();
        root.add(p, true);
    }
    checkpoint("sequent remove + addB", COUNT);

    for (Test* t = static_cast<Test*>(root.m_Neigh[1]); t != &root; t = static_cast<Test*>(t->m_Neigh[1]))
        SideEffect = SideEffect * 2 + t->m_Num;

}

int main()
{
    ops();
    std::cout << "Side effect (ignore it): " << SideEffect << std::endl;
}
