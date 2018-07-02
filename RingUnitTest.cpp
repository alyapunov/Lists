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

#include <iostream>
#include <vector>


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

#define CHECK(...) check(__VA_ARGS__, __func__, __FILE__, __LINE__)

struct Announcer
{
    const char* m_Func;
    explicit Announcer(const char* aFunc) : m_Func(aFunc)
    {
        std::cout << "======================= Test \"" << m_Func << "\" started =======================" << std::endl;
    }
    ~Announcer()
    {
        std::cout << "======================= Test \"" << m_Func << "\" finished ======================" << std::endl;
    }
};

#define ANNOUNCE() Announcer sAnn(__func__)

const size_t ONE = 1;

struct Test : Ring
{
    int m_Num;
    explicit Test(int aNum = 0) : Ring(0), m_Num(aNum) {}
};

template <class T>
void checkRing(const Test* aRing, const T& aList)
{
    CHECK(aRing->selfCheck(), 0);
    CHECK(aRing->calcSize(), aList.size());
    CHECK(aRing->isAlone(), aList.size() == 1);
    const Test* sRunner = aRing;
    for (int sVal : aList)
    {
        CHECK(sRunner->m_Num, sVal);
        sRunner = static_cast<const Test*>(sRunner->m_Neigh[1]);
    }
    CHECK(sRunner == aRing);
}

void checkRing(const Test* aRing, const std::initializer_list<int>& aList)
{
    return checkRing<std::initializer_list<int>>(aRing, aList);
}

static void test_split_join(int aSize1, int aSize2, bool aInvert)
{
    std::vector<int> list1;
    std::vector<int> list2;

    std::vector<Test> r1;
    r1.resize(aSize1);
    for (int i = 0; i < aSize1; i++)
    {
        list1.push_back(i);
        r1[i].init();
        r1[i].m_Num = i;
        if (i > 0)
            r1[0].add(&r1[i], true);
    }
    checkRing(&r1[0], list1);

    std::vector<Test> r2;
    r2.resize(aSize2);
    for (int i = 0; i < aSize2; i++)
    {
        list2.push_back(i + aSize1);
        r2[i].init();
        r2[i].m_Num = i + aSize1;
        if (i > 0)
            r2[0].add(&r2[i], true);
    }
    checkRing(&r2[0], list2);

    if (!aInvert)
    {
        std::vector<int> list_joined;
        for (int i = 0; i < aSize1 + aSize2; i++)
            list_joined.push_back(i);

        r1[0].join(&r2[0], false);
        checkRing(&r1[0], list_joined);
        r1[0].split(&r2[0], false);

    }
    else
    {
        std::vector<int> list_joined;
        list_joined.push_back(0);
        for (int i = 1; i < aSize2; i++)
            list_joined.push_back(i + aSize1);
        list_joined.push_back(aSize1);
        for (int i = 1; i < aSize1; i++)
            list_joined.push_back(i);

        r1[0].join(&r2[0], true);
        checkRing(&r1[0], list_joined);
        r1[0].split(&r2[0], true);
    }

    checkRing(&r1[0], list1);
    checkRing(&r2[0], list2);
}

static void test_swap(int aSize1, int aSize2)
{
    std::vector<int> list1;
    std::vector<int> list2;
    std::vector<int> swap_list1;
    std::vector<int> swap_list2;

    std::vector<Test> r1;
    r1.resize(aSize1);
    for (int i = 0; i < aSize1; i++)
    {
        list1.push_back(i);
        if (i == 0)
            swap_list1.insert(swap_list1.begin(), i);
        else
            swap_list2.push_back(i);
        r1[i].init();
        r1[i].m_Num = i;
        if (i > 0)
            r1[0].add(&r1[i], true);
    }
    checkRing(&r1[0], list1);

    std::vector<Test> r2;
    r2.resize(aSize2);
    for (int i = 0; i < aSize2; i++)
    {
        list2.push_back(i + aSize1);
        if (i == 0)
            swap_list2.insert(swap_list2.begin(), i + aSize1);
        else
            swap_list1.push_back(i + aSize1);
        r2[i].init();
        r2[i].m_Num = i + aSize1;
        if (i > 0)
            r2[0].add(&r2[i], true);
    }
    checkRing(&r2[0], list2);

    r1[0].swap(&r2[0]);
    checkRing(&r1[0], swap_list1);
    checkRing(&r2[0], swap_list2);

    r2[0].swap(&r1[0]);
    checkRing(&r1[0], list1);
    checkRing(&r2[0], list2);
}


static void simple()
{
    ANNOUNCE();
    {
        Ring r;
        r.init();
        CHECK(r.selfCheck(), 0);
        CHECK(r.isAlone());
        CHECK(r.calcSize(), ONE);
    }
    {
        Ring r(0);
        CHECK(r.selfCheck(), 0);
        CHECK(r.isAlone());
        CHECK(r.calcSize(), ONE);
    }
    {
        Test r(0);
        checkRing(&r, {0});

        Test more[10];
        std::vector<int> sComp = {0};
        for (int i = 0; i < 10; i++)
        {
            more[i].m_Num = i + 1;
            r.add(&more[i], false);
            sComp.insert(sComp.begin() + 1, i + 1);
            checkRing(&r, sComp);
        }
        for (int i = 0; i < 10; i++)
        {
            more[i].remove();
            sComp.pop_back();
            checkRing(&r, sComp);
        }
    }
    {
        Test r(0);
        checkRing(&r, {0});

        Test more[10];
        std::vector<int> sComp = {0};
        for (int i = 0; i < 10; i++)
        {
            more[i].m_Num = i + 1;
            r.add(&more[i], true);
            sComp.push_back(i + 1);
            checkRing(&r, sComp);
        }
        for (int i = 0; i < 10; i++)
        {
            more[i].remove();
            sComp.erase(sComp.begin() + 1);
            checkRing(&r, sComp);
        }
    }
    test_split_join(3, 3, false);
    test_split_join(1, 3, false);
    test_split_join(3, 1, false);
    test_split_join(1, 1, false);
    test_split_join(3, 3, true);
    test_split_join(1, 3, true);
    test_split_join(3, 1, true);
    test_split_join(1, 1, true);
    test_swap(3, 3);
    test_swap(3, 1);
    test_swap(1, 3);
    test_swap(1, 1);
}

int main()
{
    simple();

    std::cout << (0 == rc ? "Success" : "Finished with errors") << std::endl;
    return rc;
}