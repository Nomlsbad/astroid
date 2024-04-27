#include "Core/Delegates/Delegate.h"

#include <iostream>

struct A
{
    A() = default;

    A(const A&) { std::cout << "Copy constructor A\n"; }

    A(A&&) noexcept { std::cout << "Move constructor A\n"; }

    A& operator= (const A&)
    {
        std::cout << "Copy assigment A\n";
        return *this;
    }

    A& operator= (A&&) noexcept
    {
        std::cout << "Move assigment A\n";
        return *this;
    }
};

using namespace TGEngine::Core;
using namespace TGEngine::Types;

int f(int)
{
    return 2;
}

int g(int n, const A&)
{
    return n * n;
}

struct S
{
    int foo(int, A&) { return 1; }

    int bar(int, A) const { return -1; }

    int hul(int, double) {return 0;}
};

int main()
{
    S s;
    const S cs;

    const A ca;
    A a;

    Delegate<int(int)> delegate;

    auto l = [](int, double) { return 1; };
    delegate.bindLambda(l ,3.14);
    delegate.bindLambda([](int) {return 1;});

    delegate.bindStatic(&g, A());
    delegate.bindStatic(&f);

    delegate.bindMethod(&S::foo, &s, ca);
    delegate.bindMethod(&S::hul, &s, 1.0);
    delegate.bindMethod(&S::bar, &cs, a);

    Delegate delegate1 = delegate;
    std::cout << delegate1.execute(1) << " \n";

    return 0;
}