#include "Core/Delegates/Delegate.h"
#include "Core/Memory/StackStorage.h"

#include <array>
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

int f(A)
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
};

int main()
{
    S s;
    const S cs;

    const A ca;
    A a;

    Delegate<int(int)> delegate;

    auto l = [](int, A&) { return 1; };
    delegate.bindLambda(l, A());
    std::cout << delegate.execute(5) << "\n";

    delegate.bindStatic(&g, a);

    delegate.bindMethod(&S::foo, &s, ca);
    delegate.bindMethod(&S::bar, &cs, a);

    return 0;
}