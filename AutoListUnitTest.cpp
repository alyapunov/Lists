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
#include <AutoList.hpp>

#include <iostream>
#include <vector>

namespace
{

struct Object
{
    int m_Data;
    Object(int aId) : m_Data(aId) {}
    AutoListLink m_Link;
};

using ObjectList = AutoList<Object, &Object::m_Link>;

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

void simple_check()
{
    ANNOUNCE();

    ObjectList sList;
    CHECK(sList.selfCheck(), 0);
    CHECK(sList, {});
    CHECK(sList.empty());

    Object a(1);
    Object b(2);
    Object c(3);
    CHECK(a.m_Link.selfCheck() == 0 && b.m_Link.selfCheck() == 0 && c.m_Link.selfCheck() == 0);
    CHECK(a.m_Link.isAlone() && b.m_Link.isAlone() && c.m_Link.isAlone());

    // LIFO
    CHECK(a.m_Link.isAlone());
    sList.insertFront(a);
    CHECK(!a.m_Link.isAlone());
    CHECK(sList.selfCheck(), 0);
    CHECK(sList, {1});
    CHECK(!sList.empty());

    CHECK(b.m_Link.isAlone());
    sList.insertFront(b);
    CHECK(!b.m_Link.isAlone());
    CHECK(sList.selfCheck(), 0);
    CHECK(sList, {2, 1});
    CHECK(!sList.empty());

    CHECK(c.m_Link.isAlone());
    sList.insertFront(c);
    CHECK(!c.m_Link.isAlone());
    CHECK(sList.selfCheck(), 0);
    CHECK(sList, {3, 2, 1});
    CHECK(!sList.empty());

    CHECK(a.m_Link.selfCheck() == 0 && b.m_Link.selfCheck() == 0 && c.m_Link.selfCheck() == 0);
    CHECK(!a.m_Link.isAlone() && !b.m_Link.isAlone() && !c.m_Link.isAlone());

    sList.removeItem(c);
    CHECK(c.m_Link.isAlone());
    CHECK(sList.selfCheck(), 0);
    CHECK(sList, {2, 1});
    CHECK(!sList.empty());

    sList.removeItem(b);
    CHECK(b.m_Link.isAlone());
    CHECK(sList.selfCheck(), 0);
    CHECK(sList, {1});
    CHECK(!sList.empty());

    sList.removeItem(a);
    CHECK(a.m_Link.isAlone());
    CHECK(sList.selfCheck(), 0);
    CHECK(sList, {});
    CHECK(sList.empty());

    CHECK(a.m_Link.selfCheck() == 0 && b.m_Link.selfCheck() == 0 && c.m_Link.selfCheck() == 0);
    CHECK(a.m_Link.isAlone() && b.m_Link.isAlone() && c.m_Link.isAlone());

    // FIFO
    CHECK(a.m_Link.isAlone());
    sList.insertFront(a);
    CHECK(!a.m_Link.isAlone());
    CHECK(sList.selfCheck(), 0);
    CHECK(sList, {1});
    CHECK(!sList.empty());

    CHECK(b.m_Link.isAlone());
    sList.insertFront(b);
    CHECK(!b.m_Link.isAlone());
    CHECK(sList.selfCheck(), 0);
    CHECK(sList, {2, 1});
    CHECK(!sList.empty());

    CHECK(c.m_Link.isAlone());
    sList.insertFront(c);
    CHECK(!c.m_Link.isAlone());
    CHECK(sList.selfCheck(), 0);
    CHECK(sList, {3, 2, 1});
    CHECK(!sList.empty());

    CHECK(a.m_Link.selfCheck() == 0 && b.m_Link.selfCheck() == 0 && c.m_Link.selfCheck() == 0);
    CHECK(!a.m_Link.isAlone() && !b.m_Link.isAlone() && !c.m_Link.isAlone());

    a.m_Link.remove();
    CHECK(a.m_Link.isAlone());
    CHECK(sList.selfCheck(), 0);
    CHECK(sList, {3, 2});
    CHECK(!sList.empty());

    b.m_Link.remove();
    CHECK(b.m_Link.isAlone());
    CHECK(sList.selfCheck(), 0);
    CHECK(sList, {3});
    CHECK(!sList.empty());

    c.m_Link.remove();
    CHECK(c.m_Link.isAlone());
    CHECK(sList.selfCheck(), 0);
    CHECK(sList, {});
    CHECK(sList.empty());

    CHECK(a.m_Link.selfCheck() == 0 && b.m_Link.selfCheck() == 0 && c.m_Link.selfCheck() == 0);
    CHECK(a.m_Link.isAlone() && b.m_Link.isAlone() && c.m_Link.isAlone());

    // LIFO Back
    CHECK(a.m_Link.isAlone());
    sList.insertBack(a);
    CHECK(!a.m_Link.isAlone());
    CHECK(sList.selfCheck(), 0);
    CHECK(sList, {1});
    CHECK(!sList.empty());

    CHECK(b.m_Link.isAlone());
    sList.insertBack(b);
    CHECK(!b.m_Link.isAlone());
    CHECK(sList.selfCheck(), 0);
    CHECK(sList, {1, 2});
    CHECK(!sList.empty());

    CHECK(c.m_Link.isAlone());
    sList.insertBack(c);
    CHECK(!c.m_Link.isAlone());
    CHECK(sList.selfCheck(), 0);
    CHECK(sList, {1, 2, 3});
    CHECK(!sList.empty());

    CHECK(a.m_Link.selfCheck() == 0 && b.m_Link.selfCheck() == 0 && c.m_Link.selfCheck() == 0);
    CHECK(!a.m_Link.isAlone() && !b.m_Link.isAlone() && !c.m_Link.isAlone());

    sList.removeItem(c);
    CHECK(c.m_Link.isAlone());
    CHECK(sList.selfCheck(), 0);
    CHECK(sList, {1, 2});
    CHECK(!sList.empty());

    sList.removeItem(b);
    CHECK(b.m_Link.isAlone());
    CHECK(sList.selfCheck(), 0);
    CHECK(sList, {1});
    CHECK(!sList.empty());

    sList.removeItem(a);
    CHECK(a.m_Link.isAlone());
    CHECK(sList.selfCheck(), 0);
    CHECK(sList, {});
    CHECK(sList.empty());

    CHECK(a.m_Link.selfCheck() == 0 && b.m_Link.selfCheck() == 0 && c.m_Link.selfCheck() == 0);
    CHECK(a.m_Link.isAlone() && b.m_Link.isAlone() && c.m_Link.isAlone());

    // FIFO back
    CHECK(a.m_Link.isAlone());
    sList.insertBack(a);
    CHECK(!a.m_Link.isAlone());
    CHECK(sList.selfCheck(), 0);
    CHECK(sList, {1});
    CHECK(!sList.empty());

    CHECK(b.m_Link.isAlone());
    sList.insertBack(b);
    CHECK(!b.m_Link.isAlone());
    CHECK(sList.selfCheck(), 0);
    CHECK(sList, {1, 2});
    CHECK(!sList.empty());

    CHECK(c.m_Link.isAlone());
    sList.insertBack(c);
    CHECK(!c.m_Link.isAlone());
    CHECK(sList.selfCheck(), 0);
    CHECK(sList, {1, 2, 3});
    CHECK(!sList.empty());

    CHECK(a.m_Link.selfCheck() == 0 && b.m_Link.selfCheck() == 0 && c.m_Link.selfCheck() == 0);
    CHECK(!a.m_Link.isAlone() && !b.m_Link.isAlone() && !c.m_Link.isAlone());

    a.m_Link.remove();
    CHECK(a.m_Link.isAlone());
    CHECK(sList.selfCheck(), 0);
    CHECK(sList, {2, 3});
    CHECK(!sList.empty());

    b.m_Link.remove();
    CHECK(b.m_Link.isAlone());
    CHECK(sList.selfCheck(), 0);
    CHECK(sList, {3});
    CHECK(!sList.empty());

    c.m_Link.remove();
    CHECK(c.m_Link.isAlone());
    CHECK(sList.selfCheck(), 0);
    CHECK(sList, {});
    CHECK(sList.empty());

    CHECK(a.m_Link.selfCheck() == 0 && b.m_Link.selfCheck() == 0 && c.m_Link.selfCheck() == 0);
    CHECK(a.m_Link.isAlone() && b.m_Link.isAlone() && c.m_Link.isAlone());

    // Middle
    CHECK(a.m_Link.isAlone());
    sList.insertBack(a);
    CHECK(!a.m_Link.isAlone());
    CHECK(sList.selfCheck(), 0);
    CHECK(sList, {1});
    CHECK(!sList.empty());

    CHECK(c.m_Link.isAlone());
    sList.insertBack(c);
    CHECK(!c.m_Link.isAlone());
    CHECK(sList.selfCheck(), 0);
    CHECK(sList, {1, 3});
    CHECK(!sList.empty());

    CHECK(b.m_Link.isAlone());
    sList.insertAfter(a, b);
    CHECK(!b.m_Link.isAlone());
    CHECK(sList.selfCheck(), 0);
    CHECK(sList, {1, 2, 3});
    CHECK(!sList.empty());

    CHECK(a.m_Link.selfCheck() == 0 && b.m_Link.selfCheck() == 0 && c.m_Link.selfCheck() == 0);
    CHECK(!a.m_Link.isAlone() && !b.m_Link.isAlone() && !c.m_Link.isAlone());

    b.m_Link.remove();
    CHECK(b.m_Link.isAlone());
    CHECK(sList.selfCheck(), 0);
    CHECK(sList, {1, 3});
    CHECK(!sList.empty());

    a.m_Link.remove();
    CHECK(a.m_Link.isAlone());
    CHECK(sList.selfCheck(), 0);
    CHECK(sList, {3});
    CHECK(!sList.empty());

    c.m_Link.remove();
    CHECK(c.m_Link.isAlone());
    CHECK(sList.selfCheck(), 0);
    CHECK(sList, {});
    CHECK(sList.empty());

    CHECK(a.m_Link.selfCheck() == 0 && b.m_Link.selfCheck() == 0 && c.m_Link.selfCheck() == 0);
    CHECK(a.m_Link.isAlone() && b.m_Link.isAlone() && c.m_Link.isAlone());

}


void list_ctors1()
{
    ANNOUNCE();

    ObjectList sList1;
    CHECK(sList1.empty());

    ObjectList sList2(sList1);
    CHECK(sList1.empty());
    CHECK(sList2.empty());
    CHECK(sList1.selfCheck(), 0);
    CHECK(sList2.selfCheck(), 0);

    Object a(1);
    Object b(2);
    Object c(3);
    CHECK(a.m_Link.selfCheck() == 0 && b.m_Link.selfCheck() == 0 && c.m_Link.selfCheck() == 0);

    sList2.insertFront(a);
    CHECK(sList2, {1});
    ObjectList sList3(sList2);
    CHECK(sList2, {1});
    CHECK(sList3, {});
    CHECK(sList2.selfCheck(), 0);
    CHECK(sList3.selfCheck(), 0);

    ObjectList sList4(std::move(sList2));
    CHECK(sList2, {});
    CHECK(sList4, {1});
    CHECK(sList2.selfCheck(), 0);
    CHECK(sList4.selfCheck(), 0);

    ObjectList sList5;
    sList5 = sList4;
    CHECK(sList4, {1});
    CHECK(sList5, {});
    CHECK(sList4.selfCheck(), 0);
    CHECK(sList5.selfCheck(), 0);

    ObjectList sList6;
    sList6 = std::move(sList4);
    CHECK(sList4, {});
    CHECK(sList6, {1});
    CHECK(sList4.selfCheck(), 0);
    CHECK(sList6.selfCheck(), 0);
}

void list_ctors2()
{
    ANNOUNCE();

    ObjectList sList1;
    CHECK(sList1.empty());

    ObjectList sList2(sList1);
    CHECK(sList1.empty());
    CHECK(sList2.empty());
    CHECK(sList1.selfCheck(), 0);
    CHECK(sList2.selfCheck(), 0);

    Object a(1);
    Object b(2);
    Object c(3);
    CHECK(a.m_Link.selfCheck() == 0 && b.m_Link.selfCheck() == 0 && c.m_Link.selfCheck() == 0);

    sList2.insertFront(c);
    sList2.insertFront(b);
    sList2.insertFront(a);
    CHECK(sList2, {1, 2, 3});
    ObjectList sList3(sList2);
    CHECK(sList2, {1, 2, 3});
    CHECK(sList3, {});
    CHECK(sList2.selfCheck(), 0);
    CHECK(sList3.selfCheck(), 0);

    ObjectList sList4(std::move(sList2));
    CHECK(sList2, {});
    CHECK(sList4, {1, 2, 3});
    CHECK(sList2.selfCheck(), 0);
    CHECK(sList4.selfCheck(), 0);

    ObjectList sList5;
    sList5 = sList4;
    CHECK(sList4, {1, 2, 3});
    CHECK(sList5, {});
    CHECK(sList4.selfCheck(), 0);
    CHECK(sList5.selfCheck(), 0);

    ObjectList sList6;
    sList6 = std::move(sList4);
    CHECK(sList4, {});
    CHECK(sList6, {1, 2, 3});
    CHECK(sList4.selfCheck(), 0);
    CHECK(sList6.selfCheck(), 0);
}

void list_ctors3()
{
    ANNOUNCE();

    ObjectList sList1;
    CHECK(sList1.empty());

    ObjectList sList2(sList1);
    CHECK(sList1.empty());
    CHECK(sList2.empty());
    CHECK(sList1.selfCheck(), 0);
    CHECK(sList2.selfCheck(), 0);

    Object a(1);
    Object b(2);
    Object c(3);
    CHECK(a.m_Link.selfCheck() == 0 && b.m_Link.selfCheck() == 0 && c.m_Link.selfCheck() == 0);

    sList2.insertBack(a);
    sList2.insertBack(b);
    sList2.insertBack(c);
    CHECK(sList2, {1, 2, 3});
    ObjectList sList3(sList2);
    CHECK(sList2, {1, 2, 3});
    CHECK(sList3, {});
    CHECK(sList2.selfCheck(), 0);
    CHECK(sList3.selfCheck(), 0);

    ObjectList sList4(std::move(sList2));
    CHECK(sList2, {});
    CHECK(sList4, {1, 2, 3});
    CHECK(sList2.selfCheck(), 0);
    CHECK(sList4.selfCheck(), 0);

    ObjectList sList5;
    sList5 = sList4;
    CHECK(sList4, {1, 2, 3});
    CHECK(sList5, {});
    CHECK(sList4.selfCheck(), 0);
    CHECK(sList5.selfCheck(), 0);

    ObjectList sList6;
    sList6 = std::move(sList4);
    CHECK(sList4, {});
    CHECK(sList6, {1, 2, 3});
    CHECK(sList4.selfCheck(), 0);
    CHECK(sList6.selfCheck(), 0);

    Object d(4);
    Object e(5);
    sList4.insertFront(d);
    sList4.insertBack(e);
    CHECK(sList4, {4, 5});
    CHECK(sList6, {1, 2, 3});
    CHECK(sList4.selfCheck(), 0);
    CHECK(sList6.selfCheck(), 0);

    sList6 = std::move(sList4);
    CHECK(sList4, {1, 2, 3});
    CHECK(sList6, {4, 5});
    CHECK(sList4.selfCheck(), 0);
    CHECK(sList6.selfCheck(), 0);
}

void iterations()
{
    ANNOUNCE();

    ObjectList sList;
    Object obj[5] = {0, 1, 2, 3, 4};
    for (size_t i = 0; i < 5; i++)
        sList.insertBack(obj[i]);
    CHECK(sList.selfCheck(), 0);
    CHECK(sList, {0, 1, 2, 3, 4});

    bool sDel = false;
    for (auto sItr = sList.begin(); sItr != sList.end(); )
    {
        if (sDel)
            sList.removeItem(*sItr++);
        else
            ++sItr;
        sDel = !sDel;
    }
    CHECK(sList.selfCheck(), 0);
    CHECK(sList, {0, 2, 4});

    for (auto sItr = sList.begin(); sItr != sList.end(); )
    {
        if (sDel)
            sList.removeItem(*sItr++);
        else
            ++sItr;
        sDel = !sDel;
    }
    CHECK(sList.selfCheck(), 0);
    CHECK(sList, {2});

    for (auto sItr = sList.begin(); sItr != sList.end(); )
    {
        if (sDel)
            sList.removeItem(*sItr++);
        else
            ++sItr;
        sDel = !sDel;
    }
    CHECK(sList.selfCheck(), 0);
    CHECK(sList, {2});

    for (auto sItr = sList.begin(); sItr != sList.end(); )
    {
        if (sDel)
            sList.removeItem(*sItr++);
        else
            ++sItr;
        sDel = !sDel;
    }
    CHECK(sList.selfCheck(), 0);
    CHECK(sList, {});
}

void link_ctors()
{
    ANNOUNCE();

    ObjectList sList;
    CHECK(sList.selfCheck(), 0);
    CHECK(sList, {});
    {
        Object obj(1);
        sList.insertFront(obj);
        CHECK(sList.selfCheck(), 0);
        CHECK(sList, {1});
    }
    CHECK(sList.selfCheck(), 0);
    CHECK(sList, {});

    {
        Object obj(1);
        sList.insertFront(obj);
        CHECK(sList.selfCheck(), 0);
        CHECK(sList, {1});
        {
            Object obj2(2);
            sList.insertBack(obj2);
            CHECK(sList.selfCheck(), 0);
            CHECK(sList, {1, 2});
        }
        CHECK(sList.selfCheck(), 0);
        CHECK(sList, {1});
    }
    CHECK(sList.selfCheck(), 0);
    CHECK(sList, {});

    {
        Object obj(1);
        CHECK(obj.m_Link.isAlone());

        Object obj2(obj);
        CHECK(obj.m_Link.isAlone());
        CHECK(obj2.m_Link.isAlone());

        Object obj3(std::move(obj));
        CHECK(obj.m_Link.isAlone());
        CHECK(obj2.m_Link.isAlone());
        CHECK(obj3.m_Link.isAlone());

        Object obj4(0);
        obj4 = obj2;
        Object obj5(0);
        obj5 = std::move(obj3);
        CHECK(obj.m_Link.isAlone());
        CHECK(obj2.m_Link.isAlone());
        CHECK(obj3.m_Link.isAlone());
        CHECK(obj4.m_Link.isAlone());
        CHECK(obj5.m_Link.isAlone());
    }

    {
        Object obj(1);
        sList.insertFront(obj);
        CHECK(sList.selfCheck(), 0);
        CHECK(sList, {1});

        {
            Object obj2(obj);
            obj2.m_Data = 2;
            CHECK(sList.selfCheck(), 0);
            CHECK(sList, {1, 2});
        }
        CHECK(sList.selfCheck(), 0);
        CHECK(sList, {1});

        {
            Object obj2(2);
            obj2 = obj;
            obj2.m_Data = 2;
            CHECK(sList.selfCheck(), 0);
            CHECK(sList, {1, 2});
        }
        CHECK(sList.selfCheck(), 0);
        CHECK(sList, {1});

        {
            Object obj2(std::move(obj));
            obj2.m_Data = 2;
            CHECK(sList.selfCheck(), 0);
            CHECK(sList, {2});
            CHECK(obj.m_Link.isAlone());
        }

        CHECK(sList.selfCheck(), 0);
        CHECK(sList, {});
        CHECK(obj.m_Link.isAlone());

        sList.insertFront(obj);
        CHECK(sList.selfCheck(), 0);
        CHECK(sList, {1});
        {
            Object obj2(2);
            obj2 = std::move(obj);
            obj2.m_Data = 2;
            CHECK(sList.selfCheck(), 0);
            CHECK(sList, {2});
            CHECK(obj.m_Link.isAlone());
        }

        CHECK(sList.selfCheck(), 0);
        CHECK(sList, {});
        CHECK(obj.m_Link.isAlone());
    }
    {
        Object obj(1);
        {
            ObjectList sList2;
            sList.insertFront(obj);
            CHECK(sList.selfCheck(), 0);
            CHECK(sList, {1});

            {
                Object obj2(2);
                sList2.insertBack(obj2);
                CHECK(sList2.selfCheck(), 0);
                CHECK(sList2, {2});

                obj = std::move(obj2);
                obj.m_Data = 1;

                CHECK(sList.selfCheck(), 0);
                CHECK(sList, {2});
                CHECK(sList2.selfCheck(), 0);
                CHECK(sList2, {1});
                CHECK(!obj.m_Link.isAlone());
            }
        }
        CHECK(sList.selfCheck(), 0);
        CHECK(sList, {});
        CHECK(obj.m_Link.isAlone());
    }
    CHECK(sList.selfCheck(), 0);
    CHECK(sList, {});
}

void massive_test()
{
    ObjectList sList;
    std::vector<int> sReference;
    const size_t ITER_COUNT = 1000000;
    const size_t SIZE_LIM = 10;
    for (size_t i = 0; i < ITER_COUNT; i++)
    {
        bool sAdd = true;
        if (sReference.size() == SIZE_LIM)
            sAdd = false;
        else if (!sReference.empty() && (rand() & 1) == 0)
            sAdd = false;
        bool sBegin = (rand() & 1) == 0;
        if (sAdd)
        {
            int sValue = rand();
            Object* sObj = new Object(sValue);
            if (sBegin)
            {
                sList.insertFront(*sObj);
                sReference.insert(sReference.begin(), sValue);
            }
            else
            {
                sList.insertBack(*sObj);
                sReference.insert(sReference.end(), sValue);
            }
        }
        else
        {
            if (sBegin)
            {
                delete &sList.front();
                sReference.erase(sReference.begin());
            }
            else
            {
                delete &sList.back();
                sReference.pop_back();
            }
        }

        CHECK(sList, sReference);
    }
}

} // anonymous namespace

int main()
{
    simple_check();
    list_ctors1();
    list_ctors2();
    list_ctors3();
    iterations();
    link_ctors();
    massive_test();

    if (rc == 0)
        std::cout << "Success" << std::endl;
    else
        std::cout << "Failed" << std::endl;
    return rc;
}
