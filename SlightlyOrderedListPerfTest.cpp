#include <SlightlyOrderedList.hpp>
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
#include <chrono>
#include <iostream>

namespace
{
struct Object
{
    SlightlyOrderedListLink m_Link;
};

using ObjectList = SlightlyOrderedList<Object, &Object::m_Link>;

static size_t SideEffect = 0;
}

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

bool rand_bool()
{
    static int rnd = rand();
    static int max = RAND_MAX;
    if (0 == RAND_MAX)
    {
        rnd = rand();
        max = RAND_MAX;
    }
    bool res = rnd & 1;
    rnd >>= 1;
    max >>= 1;
    return res;
}

static int rand8()
{
    static int the_rand = rand();
    static int s = 0;
    s = (s + 1) % 16;
    return (the_rand >> s) % 8;
}

static void small_sizes()
{
    //warmup
    {
        volatile int s = 0;
        for (size_t i = 0; i < 200000000; i++)
            s *= 10;
        if (s != 0)
            exit(1);
    }

    const size_t COUNT = 1024 * 1024;
    checkpoint("", 0);

    {
        for (size_t i = 0; i < COUNT; i++)
        {
            SideEffect += rand8();
        }
        checkpoint("random", COUNT);

        Object sObjects[8];
        ObjectList sList;
        for (size_t i = 0; i < 8; i++)
            sList.insert(sObjects[i]);

        for (size_t i = 0; i < COUNT; i++)
        {
            Object& o = sObjects[rand8()];
            sList.remove(o);
            sList.insert(o);
        }

        checkpoint("Small size random add/remove", COUNT);

        for (size_t i = 0; i < COUNT; i++)
        {
            Object& o = sObjects[i % 8];
            sList.remove(o);
            sList.insert(o);
        }

        checkpoint("Small size sequent add/remove", COUNT);
    }


}

static void big_sizes()
{
    const size_t SIZE = 16 * 1024;
    checkpoint("", 0);

    {
        Object sObjects[SIZE];
        checkpoint("Construction warmup", SIZE);
    }

    checkpoint("Destruction warmup", SIZE);


    {
        Object sObjects[SIZE];
        checkpoint("Construction", SIZE);
    }
    checkpoint("Destruction", SIZE);

    {
        Object sObjects[SIZE];
        checkpoint("Construction", SIZE);
        ObjectList sList;

        for (size_t i = 0; i < SIZE; i++)
            sList.insert(sObjects[i]);
        checkpoint("Addition", SIZE);
        for (size_t i = 0; i < SIZE; i++)
            sList.remove(sObjects[i]);
        checkpoint("Removing", SIZE);

        for (size_t i = 0; i < SIZE / 2; i++)
        {
            sList.insert(sObjects[i]);
            sList.insert(sObjects[SIZE - 1 - i]);
        }
        checkpoint("Addition (mix)", SIZE);

        for (size_t i = 0; i < SIZE; i++)
            sList.remove(sObjects[i]);
        checkpoint("Removing", SIZE);

    }
    checkpoint("Destruction", SIZE);
}

int main()
{
    small_sizes();
    big_sizes();
    std::cout << "Side effect (ignore it): " << SideEffect << std::endl;
}