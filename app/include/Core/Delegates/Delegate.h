#ifndef TGENGINE_DELEGATE_H
#define TGENGINE_DELEGATE_H

#include "Core/Memory/StackStorage.h"
#include "Core/Types/Callable.h"
#include "DelegateAllocator.h"
#include "ErasedFunctions.h"

#include <optional>

namespace TGEngine::Core
{


template<typename DelegateSignature>
class Delegate;

template<typename RetVal, typename... Args>
class Delegate<RetVal(Args...)>
{
private:

    using ErasedDelegate = _DelegateInternals::IDelegate<RetVal(Args...)>;

    using Executer = FunctionPtr<RetVal, ErasedDelegate*, Args&&...>;
    using Constructor = FunctionPtr<ErasedDelegate*, ErasedDelegate*, const ErasedDelegate*>;

    static constexpr size_t storageSize = 64;
    static constexpr size_t storageAlign = 8;
    using Storage = StackStorage<storageSize, storageAlign>;
    using DefaultAllocator = _DelegateInternals::DelegateAllocator<ErasedDelegate, Storage>;

public:

    Delegate() = default;

    Delegate(const Delegate& delegate)
        : defaultAllocator(storage)
        , executer(delegate.executer)
        , constructor(delegate.constructor)
        , erasedDelegate(makeCopy(delegate))
    {}

    Delegate& operator= (const Delegate& delegate)
    {
        if (this == &delegate) return *this;
        unbind();

        executer = delegate.executer;
        constructor = delegate.constructor;
        erasedDelegate = makeCopy(delegate);

        return *this;
    }

    ~Delegate() { unbind(); }

private:

    ErasedDelegate* makeCopy(const Delegate& delegate)
    {
        ErasedDelegate* copy = defaultAllocator.allocateCopy(delegate.storage);
        return constructor(copy, delegate.erasedDelegate);
    }

public:

    [[nodiscard]]
    bool isBound() const
    {
        return erasedDelegate != nullptr;
    }

    void unbind()
    {
        using AllocTraits = std::allocator_traits<DefaultAllocator>;

        AllocTraits::destroy(defaultAllocator, erasedDelegate);
        AllocTraits::deallocate(defaultAllocator, erasedDelegate, 1);
    }

public:

    [[nodiscard]]
    RetVal execute(Args... args) const
    {
        return executer(erasedDelegate, std::forward<Args>(args)...);
    }

    [[nodiscard]]
    std::optional<RetVal> executeIfBound(Args... args) const
    {
        if (!isBound()) return std::nullopt;
        return executer(erasedDelegate, std::forward<Args>(args)...);
    }

public:

    template<typename... ErasedArgs>
    void bindStatic(FunctionPtr<RetVal, Args..., ErasedArgs...> function, std::decay_t<ErasedArgs>... payload)
    {
        using namespace _DelegateInternals;
        using DelegateType = StaticDelegate<decltype(function), RetVal(Args...), decltype(payload)...>;
        bind<DelegateType>(function, std::move(payload)...);
    }

    template<typename Lambda, typename... Payload>
    void bindLambda(Lambda&& lambda, Payload... payload)
    requires std::invocable<Lambda, Args..., Payload...>
    {
        using namespace _DelegateInternals;
        using DelegateType = LambdaDelegate<Lambda, RetVal(Args...), Payload...>;
        bind<DelegateType>(std::forward<Lambda>(lambda), std::move(payload)...);
    }

    template<typename T, typename... ErasedArgs>
    void bindMethod(NotConstMethodPtr<T, RetVal, Args..., ErasedArgs...> method, T* object,
                    std::decay_t<ErasedArgs>... payload)
    {
        constexpr bool isConst = false;
        using namespace _DelegateInternals;
        using DelegateType = MethodDelegate<decltype(method), T, isConst, RetVal(Args...), decltype(payload)...>;
        bind<DelegateType>(method, object, std::move(payload)...);
    }

    template<typename T, typename... ErasedArgs>
    void bindMethod(ConstMethodPtr<T, RetVal, Args..., ErasedArgs...> method, const T* object,
                    std::decay_t<ErasedArgs>... payload)
    {
        constexpr bool isConst = true;
        using namespace _DelegateInternals;
        using DelegateType = MethodDelegate<decltype(method), T, isConst, RetVal(Args...), decltype(payload)...>;
        bind<DelegateType>(method, object, std::move(payload)...);
    }

private:

    template<_DelegateInternals::DelegateType DelegateType, typename... ConstructionArgs>
    void bind(ConstructionArgs&&... args)
    {
        if (isBound()) unbind();

        erasedDelegate = makeDelegate<DelegateType>(std::forward<ConstructionArgs>(args)...);
        executer = _DelegateInternals::execute<DelegateType>;
        constructor = _DelegateInternals::construct<DelegateType>;
    }

    template<typename DelegateType, typename... ConstructionArgs>
    ErasedDelegate* makeDelegate(ConstructionArgs&&... args)
    {
        using Allocator = typename std::allocator_traits<DefaultAllocator>::template rebind_alloc<DelegateType>;
        using AllocTraits = std::allocator_traits<Allocator>;

        Allocator allocator(storage);
        DelegateType* delegate = AllocTraits::allocate(allocator, 1);
        AllocTraits::construct(allocator, delegate, std::forward<ConstructionArgs>(args)...);

        return delegate;
    }

private:

    Storage storage;
    DefaultAllocator defaultAllocator {storage};

    Executer executer {};
    Constructor constructor {};
    ErasedDelegate* erasedDelegate {};
};


} // namespace TGEngine::Core


#endif // TGENGINE_DELEGATE_H
