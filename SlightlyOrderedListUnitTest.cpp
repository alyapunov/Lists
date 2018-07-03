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
#include <SlightlyOrderedList.hpp>

#include <iostream>
#include <vector>

struct Object
{
    int m_Data;
    Object(int aId = 0) : m_Data(aId) {}
    SlightlyOrderedListLink m_Link;
};

using ObjectList = SlightlyOrderedList<Object, &Object::m_Link>;

int rc = 0;

void check(bool exp, const char* funcname, const char *filename, int line)
{
    if (!exp)
    {
        rc = 1;
        std::cerr << "Check failed in " << funcname << " at " << filename << ":" << line << std::endl;
    }
}

template<class T>
void check(const T& x, const T& y, const char* funcname, const char *filename, int line)
{
    if (x != y)
    {
        rc = 1;
        std::cerr << "Check failed: " << x << " != " << y <<  " in " << funcname << " at " << filename << ":" << line << std::endl;
    }
}

void check(const ObjectList& aList, std::vector<int> aArr, const char* funcname, const char *filename, int line)
{
    bool sFailed = false;
    if (aList.empty() != (aArr.size() == 0))
        sFailed = true;

    int sFirst = 0 == aArr.size() ? 0 : *aArr.begin();
    int sLast = 0;
    auto sItr1 = aList.begin();
    auto sItr2 = aArr.begin();
    for (; sItr1 != aList.end() && sItr2 != aArr.end(); ++sItr1, ++sItr2)
    {
        if (sItr1->m_Data != *sItr2)
            sFailed = true;
        sLast = *sItr2;
    }
    if (sItr1 != aList.end() || sItr2 != aArr.end())
        sFailed = true;
    if (!aList.empty() && aList.front().m_Data != sFirst)
        sFailed = true;
    if (!aList.empty() && aList.back().m_Data != sLast)
        sFailed = true;

    if (aArr.begin() != aArr.end())
    {
        sItr1 = aList.end();
        --sItr1;
        sItr2 = aArr.end();
        --sItr2;
        for (; sItr1 != aList.begin() && sItr2 != aArr.begin(); --sItr1, --sItr2)
        {
            if (sItr1->m_Data != *sItr2)
                sFailed = true;
        }
        if (sItr1 != aList.begin() || sItr2 != aArr.begin())
            sFailed = true;
    }

    if (sFailed)
    {
        std::cerr << "Check failed: list {";
        bool sFirst = true;
        for (const Object& sObj : aList)
        {
            if (!sFirst)
                std::cerr << ", " << sObj.m_Data;
            else
                std::cerr << sObj.m_Data;
            sFirst = false;
        }
        std::cerr << "} expected to be {";
        sFirst = true;
        for (int sVal : aArr)
        {
            if (!sFirst)
                std::cerr << ", " << sVal;
            else
                std::cerr << sVal;
            sFirst = false;
        }

        std::cerr << "} in " << funcname << " at " << filename << ":" << line << std::endl;
        rc = 1;
    }
}

#define CHECK(...) check(__VA_ARGS__, __func__, __FILE__, __LINE__)

struct Announcer
{
    const char* m_Func;
    explicit Announcer(const char* aFunc) : m_Func(aFunc) { std::cout << "Test " << m_Func << " started" << std::endl; }
    ~Announcer() { std::cout << "Test " << m_Func << " finished" << std::endl; }
};

#define ANNOUNCE() Announcer sAnn(__func__)

static void simple()
{
    ANNOUNCE();

    ObjectList sList;
    Object sItems[5];
    for (size_t i = 0; i < 5; i++)
        sItems[i] = i;

    CHECK(sList, {});
    sList.insert(sItems[0]);
    CHECK(sList, {0});
    sList.insert(sItems[1]);
    CHECK(sList, {0, 1});
    sList.insert(sItems[2]);
    CHECK(sList, {0, 1, 2});
    sList.insert(sItems[3]);
    CHECK(sList, {0, 1, 2, 3});
    sList.insert(sItems[4]);
    CHECK(sList, {0, 1, 2, 3, 4});

    CHECK(sList.front().m_Data, 0);
    CHECK(sList.back().m_Data, 4);

    sList.remove(sItems[0]);
    CHECK(sList, {1, 2, 3, 4});
    sList.remove(sItems[1]);
    CHECK(sList, {2, 3, 4});
    sList.remove(sItems[2]);
    CHECK(sList, {3, 4});
    sList.remove(sItems[3]);
    CHECK(sList, {4});
    sList.remove(sItems[4]);
    CHECK(sList, {});

    CHECK(sList, {});
    sList.insert(sItems[4]);
    CHECK(sList, {4});
    sList.insert(sItems[3]);
    CHECK(sList, {3, 4});
    sList.insert(sItems[2]);
    CHECK(sList, {2, 3, 4});
    sList.insert(sItems[1]);
    CHECK(sList, {1, 2, 3, 4});
    sList.insert(sItems[0]);
    CHECK(sList, {0, 1, 2, 3, 4});

    CHECK(sList.front().m_Data, 0);
    CHECK(sList.back().m_Data, 4);

    sList.remove(sItems[4]);
    CHECK(sList, {0, 1, 2, 3});
    sList.remove(sItems[3]);
    CHECK(sList, {0, 1, 2});
    sList.remove(sItems[2]);
    CHECK(sList, {0, 1});
    sList.remove(sItems[1]);
    CHECK(sList, {0});
    sList.remove(sItems[0]);
    CHECK(sList, {});

    CHECK(sList, {});
    sList.insert(sItems[2]);
    CHECK(sList, {2});
    sList.insert(sItems[1]);
    CHECK(sList, {1, 2});
    sList.insert(sItems[3]);
    CHECK(sList, {1, 2, 3});
    sList.insert(sItems[0]);
    CHECK(sList, {0, 1, 2, 3});
    sList.insert(sItems[4]);
    CHECK(sList, {0, 1, 2, 3, 4});

    CHECK(sList.front().m_Data, 0);
    CHECK(sList.back().m_Data, 4);

    sList.remove(sItems[1]);
    CHECK(sList, {0, 2, 3, 4});
    sList.remove(sItems[3]);
    CHECK(sList, {0, 2, 4});
    sList.remove(sItems[0]);
    CHECK(sList, {2, 4});
    sList.remove(sItems[4]);
    CHECK(sList, {2});
    sList.remove(sItems[2]);
    CHECK(sList, {});

}

static void iterations()
{
    ANNOUNCE();

    ObjectList sList;
    Object obj[5] = {0, 1, 2, 3, 4};
    for (size_t i = 0; i < 5; i++)
        sList.insert(obj[i]);
    CHECK(sList.selfCheck(), 0);
    CHECK(sList, {0, 1, 2, 3, 4});

    bool sDel = false;
    for (auto sItr = sList.begin(); sItr != sList.end(); )
    {
        if (sDel)
            sList.remove(*sItr++);
        else
            ++sItr;
        sDel = !sDel;
    }
    CHECK(sList.selfCheck(), 0);
    CHECK(sList, {0, 2, 4});

    for (auto sItr = sList.begin(); sItr != sList.end(); )
    {
        if (sDel)
            sList.remove(*sItr++);
        else
            ++sItr;
        sDel = !sDel;
    }
    CHECK(sList.selfCheck(), 0);
    CHECK(sList, {2});

    for (auto sItr = sList.begin(); sItr != sList.end(); )
    {
        if (sDel)
            sList.remove(*sItr++);
        else
            ++sItr;
        sDel = !sDel;
    }
    CHECK(sList.selfCheck(), 0);
    CHECK(sList, {2});

    for (auto sItr = sList.begin(); sItr != sList.end(); )
    {
        if (sDel)
            sList.remove(*sItr++);
        else
            ++sItr;
        sDel = !sDel;
    }
    CHECK(sList.selfCheck(), 0);
    CHECK(sList, {});
}

int main()
{
    simple();
    iterations();

    if (rc == 0)
        std::cout << "Success" << std::endl;
    else
        std::cout << "Failed" << std::endl;
    return rc;
}
