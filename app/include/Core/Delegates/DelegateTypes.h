#ifndef TGENGINE_DELEGATE_TYPES_H
#define TGENGINE_DELEGATE_TYPES_H

#include "Core/Types/Callable.h"

#include <iostream>
#include <string_view>
#include <tuple>

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

namespace TGEngine::Core
{

template<typename RetVal, typename... Args>
class IDelegate
{
public:

    virtual ~IDelegate() = default;
};


template<typename Function, typename DelegateSignature, typename... Payload>
class StaticDelegate;

template<typename Function, typename RetVal, typename... Args, typename... Payload>
class StaticDelegate<Function, RetVal(Args...), Payload...>: public IDelegate<RetVal, Args...>
{
public:

    constexpr explicit StaticDelegate(Function function, Payload&&... payload)
        : function(function)
        , payload(std::forward<Payload>(payload)...)
    {
        std::cout << "\nStaticDelegate\n";
        std::cout << type_name<decltype(this->function)>() << "\n";
        std::cout << type_name<decltype(this->payload)>() << "\n";
    }

    [[nodiscard]]
    constexpr RetVal execute(Args&&... args)
    {
        return executeWithPayload(std::forward<Args>(args)..., std::index_sequence_for<Payload...> {});
    }

private:

    template<std::size_t... I>
    [[nodiscard]]
    constexpr RetVal executeWithPayload(Args&&... args, std::index_sequence<I...>)
    {
        return function(std::forward<Args>(args)..., std::get<I>(payload)...);
    }

private:

    Function function;
    std::tuple<Payload...> payload;
};


template<typename Lambda, typename DelegateSignature, typename... Payload>
class LambdaDelegate;

template<typename Lambda, typename RetVal, typename... Args, typename... Payload>
class LambdaDelegate<Lambda, RetVal(Args...), Payload...>: public IDelegate<RetVal, Args...>
{
public:

    constexpr explicit LambdaDelegate(Lambda&& lambda, Payload&&... payload)
        : lambda(std::forward<Lambda>(lambda))
        , payload(std::forward<Payload>(payload)...)
    {
        std::cout << "\nLambdaDelegate\n";
        std::cout << type_name<decltype(this->lambda)>() << "\n";
        std::cout << type_name<decltype(this->payload)>() << "\n";
    }

    [[nodiscard]]
    constexpr RetVal execute(Args&&... args)
    {
        return executeWithPayload(std::forward<Args>(args)..., std::index_sequence_for<Payload...> {});
    }

private:

    template<std::size_t... I>
    [[nodiscard]]
    constexpr RetVal executeWithPayload(Args&&... args, std::index_sequence<I...>)
    {
        return lambda(std::forward<Args>(args)..., std::get<I>(payload)...);
    }

private:

    std::remove_reference_t<Lambda> lambda;
    std::tuple<Payload...> payload;
};


template<typename Method, typename T, bool IsConst, typename DelegateSignature, typename... Payload>
class MethodDelegate;

template<typename Method, typename T, bool IsConst, typename RetVal, typename... Args, typename... Payload>
class MethodDelegate<Method, T, IsConst, RetVal(Args...), Payload...>: public IDelegate<RetVal, Args...>
{
public:

    using Object = std::conditional_t<IsConst, const T*, T*>;

    constexpr explicit MethodDelegate(Method method, Object object, Payload&&... payload)
        : method(method)
        , object(object)
        , payload(std::forward<Payload>(payload)...)
    {
        std::cout << "\nMethodDelegate\n";
        std::cout << type_name<decltype(this->method)>() << "\n";
        std::cout << type_name<decltype(this->object)>() << "\n";
        std::cout << type_name<decltype(this->payload)>() << "\n";
    }

    [[nodiscard]]
    constexpr RetVal execute(Args&&... args)
    {
        return executeWithPayload(std::forward<Args>(args)..., std::index_sequence_for<Payload...> {});
    }

private:

    template<std::size_t... I>
    [[nodiscard]]
    constexpr RetVal executeWithPayload(Args&&... args, std::index_sequence<I...>)
    {
        return (object->*method)(std::forward<Args>(args)..., std::get<I>(payload)...);
    }

private:

    Method method;
    Object object;
    std::tuple<Payload...> payload;
};

} // namespace TGEngine::Core

#endif // TGENGINE_DELEGATE_TYPES_H
