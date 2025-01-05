#ifndef TGENGINE_DELEGATE_TYPES_H
#define TGENGINE_DELEGATE_TYPES_H

#include <iostream>
#include <tuple>

namespace TGEngine::Core::_DelegateInternals
{

template<typename DelegateSignature>
class IDelegate;

template<typename R, typename... Args>
class IDelegate<R(Args...)>
{
public:

    using RetVal = R;
    using Signature = RetVal(Args...);
    using Erased = IDelegate;

    virtual ~IDelegate() = default;
};

template<typename T>
concept DelegateType = requires {
    typename T::RetVal;
    typename T::Signature;
    typename T::Erased;

    static_cast<IDelegate<typename T::Signature>>(std::declval<T>());
} && std::same_as<typename T::Erased, IDelegate<typename T::Signature>>;


template<typename Function, typename DelegateSignature, typename... Payload>
class StaticDelegate;

template<typename Function, typename RetVal, typename... Args, typename... Payload>
class StaticDelegate<Function, RetVal(Args...), Payload...>: public IDelegate<RetVal(Args...)>
{
public:

    constexpr explicit StaticDelegate(Function function, Payload&&... payload)
        : function(function)
        , payload(std::forward<Payload>(payload)...)
    {}

    constexpr StaticDelegate(const StaticDelegate& delegate) = default;

    [[nodiscard]]
    constexpr RetVal execute(Args&&... args)
    {
        return executeWithPayload(std::forward<Args>(args)..., std::index_sequence_for<Payload...> {});
    }

private:

    template<std::size_t... I>
    constexpr RetVal executeWithPayload(Args&&... args, std::index_sequence<I...>)
    {
        return function(std::forward<Args>(args)..., std::get<I>(payload)...);
    }

private:

    Function function;

    [[no_unique_address]]
    std::tuple<Payload...> payload;
};


template<typename Lambda, typename DelegateSignature, typename... Payload>
class LambdaDelegate;

template<typename Lambda, typename RetVal, typename... Args, typename... Payload>
class LambdaDelegate<Lambda, RetVal(Args...), Payload...>: public IDelegate<RetVal(Args...)>
{
public:

    constexpr explicit LambdaDelegate(Lambda&& lambda, Payload&&... payload)
        : lambda(std::forward<Lambda>(lambda))
        , payload(std::forward<Payload>(payload)...)
    {}

    constexpr LambdaDelegate(const LambdaDelegate& delegate) = default;

    [[nodiscard]]
    constexpr RetVal execute(Args&&... args)
    {
        return executeWithPayload(std::forward<Args>(args)..., std::index_sequence_for<Payload...> {});
    }

private:

    template<std::size_t... I>
    constexpr RetVal executeWithPayload(Args&&... args, std::index_sequence<I...>)
    {
        return lambda(std::forward<Args>(args)..., std::get<I>(payload)...);
    }

private:

    [[no_unique_address]]
    std::remove_reference_t<Lambda> lambda;

    [[no_unique_address]]
    std::tuple<Payload...> payload;
};


template<typename Method, typename T, bool IsConst, typename DelegateSignature, typename... Payload>
class MethodDelegate;

template<typename Method, typename T, bool IsConst, typename RetVal, typename... Args, typename... Payload>
class MethodDelegate<Method, T, IsConst, RetVal(Args...), Payload...>: public IDelegate<RetVal(Args...)>
{
public:

    using Object = std::conditional_t<IsConst, const T*, T*>;

    constexpr explicit MethodDelegate(Method method, Object object, Payload&&... payload)
        : method(method)
        , object(object)
        , payload(std::forward<Payload>(payload)...)
    {}

    constexpr MethodDelegate(const MethodDelegate& delegate) = default;

    [[nodiscard]]
    constexpr RetVal execute(Args&&... args)
    {
        return executeWithPayload(std::forward<Args>(args)..., std::index_sequence_for<Payload...> {});
    }

private:

    template<std::size_t... I>
    constexpr RetVal executeWithPayload(Args&&... args, std::index_sequence<I...>)
    {
        return (object->*method)(std::forward<Args>(args)..., std::get<I>(payload)...);
    }

private:

    Method method;
    Object object;

    [[no_unique_address]]
    std::tuple<Payload...> payload;
};

} // namespace TGEngine::Core::_DelegateInternals

#endif // TGENGINE_DELEGATE_TYPES_H
