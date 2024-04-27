#ifndef DELEGATE_H
#define DELEGATE_H

#include "DelegateAllocator.h"
#include "DelegateTypes.h"

#include <optional>

namespace TGEngine::Core
{

template<typename DelegateSignature>
class Delegate;

template<typename RetVal, typename... Args>
class Delegate<RetVal(Args...)>
{
public:

    Delegate()
        : executer(nullptr)
        , constructor(nullptr)
        , erasedDelegate(nullptr)
        , storage()
        , defaultAllocator(storage)
    {}

    Delegate(const Delegate& delegate)
        : executer(delegate.executer)
        , constructor(delegate.constructor)
        , erasedDelegate(nullptr)
        , storage()
        , defaultAllocator(storage)
    {
        erasedDelegate = defaultAllocator.allocateCopy(delegate.storage);
        constructor(erasedDelegate, delegate.erasedDelegate);
    }

    Delegate& operator= (const Delegate& delegate)
    {
        if (this == &delegate) return *this;
        unbind();

        executer = delegate.executer;
        constructor = delegate.constructor;
        erasedDelegate = defaultAllocator.allocateCopy(delegate.storage);
        constructor(erasedDelegate, delegate.erasedDelegate);

        return *this;
    }

    ~Delegate() { unbind(); }

public:

    template<typename... ErasedArgs>
    void bindStatic(Types::FunctionPtr<RetVal, Args..., ErasedArgs...> function, std::decay_t<ErasedArgs>... payload)
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
    void bindMethod(Types::NotConstMethodPtr<T, RetVal, Args..., ErasedArgs...> method, T* object,
                    std::decay_t<ErasedArgs>... payload)
    {
        using namespace _DelegateInternals;
        using DelegateType = MethodDelegate<decltype(method), T, false, RetVal(Args...), decltype(payload)...>;
        bind<DelegateType>(method, object, std::move(payload)...);
    }

    template<typename T, typename... ErasedArgs>
    void bindMethod(Types::ConstMethodPtr<T, RetVal, Args..., ErasedArgs...> method, const T* object,
                    std::decay_t<ErasedArgs>... payload)
    {
        using namespace _DelegateInternals;
        using DelegateType = MethodDelegate<decltype(method), T, true, RetVal(Args...), decltype(payload)...>;
        bind<DelegateType>(method, object, std::move(payload)...);
    }

private:

    template<typename DelegateType, typename... ConstructionArgs>
    void bind(ConstructionArgs&&... args)
    {
        using Allocator = std::allocator_traits<DefaultAllocator>::template rebind_alloc<DelegateType>;
        using AllocTraits = std::allocator_traits<Allocator>;

        if (isBound()) unbind();

        Allocator allocator(storage);
        DelegateType* delegate = AllocTraits::allocate(allocator, 1);
        AllocTraits::construct(allocator, delegate, std::forward<ConstructionArgs>(args)...);

        erasedDelegate = delegate;
        executer = _DelegateInternals::ErasedFunctions::execute<DelegateType>;
        constructor = static_cast<Constructor>(_DelegateInternals::ErasedFunctions::construct<DelegateType>);
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

private:

    using ErasedDelegate = _DelegateInternals::IDelegate<RetVal(Args...)>;
    using Executer = Types::FunctionPtr<RetVal, ErasedDelegate*, Args&&...>;
    using Constructor = Types::FunctionPtr<ErasedDelegate*, ErasedDelegate*, const ErasedDelegate*>;

    Executer executer; // Try to move to IDelegate
    Constructor constructor;
    ErasedDelegate* erasedDelegate;

    struct ControlBlock;
    friend struct ControlBlock;

    ControlBlock block;

private:

    static constexpr size_t storageSize = 64;
    static constexpr size_t storageAlign = 8;

    using Storage = StackStorage<storageSize, storageAlign>;
    using DefaultAllocator = DelegateAllocator<ErasedDelegate, Storage>;

    Storage storage; // Try to call new for storage
    DefaultAllocator defaultAllocator;
};

template<typename RetVal, typename... Args>
struct Delegate<RetVal(Args...)>::ControlBlock
{
    ControlBlock()
        : executer(nullptr)
        , constructor(nullptr)
        , erasedDelegate(nullptr)
    {}

    explicit ControlBlock(DefaultAllocator& allocator, const Delegate& delegate)
        : executer(delegate.block.executer)
        , constructor(delegate.block.constructor)
        , erasedDelegate(constructor(allocator.allocateCopy(delegate.storage)), delegate.erasedDelegate)
    {}

    using ErasedDelegate = _DelegateInternals::IDelegate<RetVal(Args...)>;
    using Executer = Types::FunctionPtr<RetVal, ErasedDelegate*, Args&&...>;
    using Constructor = Types::FunctionPtr<ErasedDelegate*, ErasedDelegate*, const ErasedDelegate*>;

    Executer executer; // Try to move to IDelegate
    Constructor constructor;
    ErasedDelegate* erasedDelegate;
};

} // namespace TGEngine::Core


#endif // DELEGATE_H
