#ifndef TGENGINE_DELEGATE_ALLOCATOR_H
#define TGENGINE_DELEGATE_ALLOCATOR_H

#include "DelegateTypes.h"

namespace TGEngine::Core::_DelegateInternals
{

template<DelegateType Delegate, typename Storage>
class DelegateAllocator
{
public:

    using value_type = Delegate;
    using ErasedDelegate = Delegate::Erased;

    constexpr explicit DelegateAllocator(Storage& storage)
        : storage(storage)
    {}

    constexpr DelegateAllocator(const DelegateAllocator& other)
        : storage(other.storage)
    {}

    template<typename OtherDelegate>
    constexpr explicit DelegateAllocator(const DelegateAllocator<OtherDelegate, Storage>& other)
        : storage(other.storage)
    {}

    template<DelegateType OtherDelegate, typename OStorage>
    friend class DelegateAllocator;

    template<DelegateType OtherDelegate, typename OStorage>
    bool operator== (const DelegateAllocator<OtherDelegate, OStorage>& allocator) const
    {
        return &storage == &allocator.storage && std::is_base_of_v<ErasedDelegate, OtherDelegate>;
    }

public:

    [[nodiscard]]
    Delegate* allocate(size_t)
    {
        const size_t bytes = sizeof(Delegate);
        return static_cast<Delegate*>(reserve(bytes));
    }

    [[nodiscard]]
    ErasedDelegate* allocateCopy(const Storage& copyStorage)
    {
        const size_t bytes = copyStorage.used();
        return static_cast<ErasedDelegate*>(reserve(bytes));
    }

    void deallocate(Delegate* delegate, size_t)
    {
        const size_t used = storage.used();
        hasStorageAllocation(delegate) ? storage.deallocate(delegate, used) : std::free(delegate);
    }

private:

    void* reserve(std::size_t bytes)
    {
        return canAllocateInStorage(bytes) ? storage.allocate(bytes) : std::aligned_alloc(alignof(Delegate), bytes);
    }

    bool canAllocateInStorage(size_t bytes) const { return storage.isEmpty() && storage.canAllocate(bytes); }

    bool hasStorageAllocation(Delegate* delegate) const { return storage.contains(delegate); }

private:

    Storage& storage;
};

} // namespace TGEngine::Core::_DelegateInternals

#endif // TGENGINE_DELEGATE_ALLOCATOR_H
