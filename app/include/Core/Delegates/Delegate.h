#ifndef DELEGATE_H
#define DELEGATE_H

#include "DelegateAllocator.h"
#include "DelegateTypes.h"

#include <iostream>
#include <optional>
#include <type_traits>


namespace TGEngine::Core
{

template<typename Function, typename RetVal, typename... Args>
concept Invocable = requires(Function func, Args... args) {
    {
        func(args...)
    } -> std::same_as<RetVal>;
};

template<typename DelegateSignature>
class Binder;

template<typename RetVal, typename... Args>
class Binder<RetVal(Args...)>
{
public:

    Binder()
        : executer(nullptr)
        , delegate(nullptr)
    {}

    Binder(const Binder&) = delete;
    Binder& operator= (const Binder&) = delete;

    Binder(Binder&& otherBinder) noexcept
        : executer(otherBinder.executer)
        , delegate(otherBinder.delegate)
    {
        otherBinder.executer = nullptr;
        otherBinder.delegate = nullptr;
    }

    Binder& operator= (Binder&& otherBinder) noexcept
    {
        Binder binder = std::move(otherBinder);
        swap(binder);

        return *this;
    }

    ~Binder() { unbind(); }

private:

    void swap(Binder& binder) noexcept
    {
        std::swap(executer, binder.executer);
        std::swap(delegate, binder.delegate);
    }

public:

    template<typename... ErasedArgs>
    void bindStatic(Types::FunctionPtr<RetVal, Args..., ErasedArgs...> function, std::decay_t<ErasedArgs>... payload)
    {
        using DelegateType = StaticDelegate<decltype(function), RetVal(Args...), decltype(payload)...>;
        bind<DelegateType>(function, std::move(payload)...);
    }

    template<typename Lambda, typename... Payload>
    void bindLambda(Lambda&& lambda, Payload... payload)
    requires Invocable<Lambda, RetVal, Args..., Payload...>
    {
        using DelegateType = LambdaDelegate<Lambda, RetVal(Args...), Payload...>;
        bind<DelegateType>(std::forward<Lambda>(lambda), std::move(payload)...);
    }

    template<typename T, typename... ErasedArgs>
    void bindMethod(Types::NotConstMethodPtr<T, RetVal, Args..., ErasedArgs...> method, T* object,
                    std::decay_t<ErasedArgs>... payload)
    {
        constexpr bool IsConst = false;
        using DelegateType = MethodDelegate<decltype(method), T, IsConst, RetVal(Args...), decltype(payload)...>;
        bind<DelegateType>(method, object, std::move(payload)...);
    }

    template<typename T, typename... ErasedArgs>
    void bindMethod(Types::ConstMethodPtr<T, RetVal, Args..., ErasedArgs...> method, const T* object,
                    std::decay_t<ErasedArgs>... payload)
    {
        constexpr bool IsConst = true;
        using DelegateType = MethodDelegate<decltype(method), T, IsConst, RetVal(Args...), decltype(payload)...>;
        bind<DelegateType>(method, object, std::move(payload)...);
    }

private:

    template<typename DelegateType, typename Callable, typename... ConstructionArgs>
    void bind(Callable&& callable, ConstructionArgs&&... args)
    {
        if (isBound()) unbind();
        executer = execute<DelegateType>;
        delegate = new DelegateType(std::forward<Callable>(callable), std::forward<ConstructionArgs>(args)...);
    }

public:

    [[nodiscard]]
    bool isBound() const
    {
        return delegate != nullptr;
    }

    void unbind() const
    {
        ///
        delete delegate;
    }

private:

    using ErasedDelegate = IDelegate<RetVal, Args...>*;

    template<typename DelegateType>
    constexpr static RetVal execute(ErasedDelegate delegate, Args&&... args)
    {
        return static_cast<DelegateType&>(*delegate).execute(std::forward<Args>(args)...);
    }

protected:

    using Executer = Types::FunctionPtr<RetVal, ErasedDelegate, Args&&...>;

    Executer executer;
    ErasedDelegate delegate;
};


template<typename DelegateSignature>
class Delegate;

template<typename RetVal, typename... Args>
class Delegate<RetVal(Args...)> final: public Binder<RetVal(Args...)>
{
public:

    using Super = Binder<RetVal(Args...)>;

    [[nodiscard]]
    RetVal execute(Args... args) const
    {
        return Super::executer(Super::delegate, std::forward<Args>(args)...);
    }

    [[nodiscard]]
    std::optional<RetVal> executeIfBound(Args... args) const
    {
        if (!Super::isBound()) return std::nullopt;
        return Super::executer(Super::delegate, std::forward<Args>(args)...);
    }
};


} // namespace TGEngine::Core


#endif // DELEGATE_H
