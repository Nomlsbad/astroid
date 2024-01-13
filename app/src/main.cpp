#include <functional>
#include <iostream>

template<class T>
constexpr std::string_view type_name()
{
    using namespace std;
#ifdef __clang__
    string_view p = __PRETTY_FUNCTION__;
    return string_view(p.data() + 34, p.size() - 34 - 1);
#elif defined(__GNUC__)
    string_view p = __PRETTY_FUNCTION__;
#if __cplusplus < 201'402
    return string_view(p.data() + 36, p.size() - 36 - 1);
#else
    return string_view(p.data() + 49, p.find(';', 49) - 49);
#endif
#elif defined(_MSC_VER)
    string_view p = __FUNCSIG__;
    return string_view(p.data() + 84, p.size() - 84 - 7);
#endif
    return std::string_view {};
}

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

class IDelegate
{
public:

    template<typename Func, typename... Args>
    requires std::invocable<Func, Args...>
    auto execute(Func&& func, Args&&... args)
    {
        return std::invoke(std::forward<Func>(func), std::forward<Args>(args)...);
    }

    virtual ~IDelegate() = default;
};

template<typename Lambda, typename... Payload>
class LambdaDelegate: public IDelegate
{
public:

    template<typename TLambda, typename... TPayload>
    explicit LambdaDelegate(TLambda&& lambda, TPayload&&... payload)
        : func(std::forward<TLambda>(lambda)),
          payload(std::forward<TPayload>(payload)...)
    {}

private:

    Lambda func;
    std::tuple<Payload...> payload;
};

template<typename Lambda, typename... Payload>
explicit LambdaDelegate(Lambda&& lambda, Payload&&... payload)
    -> LambdaDelegate<std::remove_reference_t<Lambda>, std::remove_reference_t<Payload>...>;

template <typename T>
struct Test
{
    explicit Test(T t) : t(t) {};
    T t;
};

template <typename RetVal, typename... Args>
struct Test<RetVal (Args...)>
{
};

std::string foo(int, double&, char&&);

int main()
{
    A aa;

    LambdaDelegate ld1([aa]() { return 1; });

    auto l = [aa]() { return 2; };
    LambdaDelegate ld2(l, 1);

    std::cout << type_name<decltype(ld1)>() << "\n";
    std::cout << type_name<decltype(ld2)>() << "\n";

    Test<decltype(foo)> t;
    std::cout << type_name<decltype(t)>() << "\n";

    return 0;
}