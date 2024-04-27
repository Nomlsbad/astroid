#ifndef TGENGINE_DELEGATE_ALLOCATOR_H
#define TGENGINE_DELEGATE_ALLOCATOR_H

#include "Core/Memory/StackStorage.h"
#include "DelegateTypes.h"

namespace TGEngine::Core
{

// TODO: implement DelegateType concept
template<typename DelegateType, typename Storage>
class DelegateAllocator
{
public:

    using value_type = DelegateType;
    using ErasedDelegate = DelegateType::Erased;

    constexpr explicit DelegateAllocator(Storage& storage)
        : storage(storage)
    {}

    constexpr DelegateAllocator(const DelegateAllocator& other)
        : storage(other.storage)
    {}

    template<typename ODelegateType>
    constexpr explicit DelegateAllocator(const DelegateAllocator<ODelegateType, Storage>& other)
        : storage(other.storage)
    {}

    template<typename ODelegateType, typename OStorage>
    bool operator== (const DelegateAllocator<ODelegateType, OStorage>& allocator) const
    {
        return std::is_same_v<OStorage, Storage> && &storage == &allocator.storage;
    }

public:

    [[nodiscard]]
    DelegateType* allocate(size_t)
    {
        const size_t bytes = sizeof(DelegateType);
        auto ptr = canAllocateInStorage(bytes) ? storage.push(bytes) : std::malloc(bytes);
        return static_cast<DelegateType*>(ptr);
    }

    void deallocate(DelegateType* delegate, size_t)
    {
        const size_t used = storage.used();
        hasStorageAllocation(delegate) ? storage.pop(delegate, used) : std::free(delegate);
    }

    [[nodiscard]]
    ErasedDelegate* allocateCopy(const Storage& copyStorage)
    {
        const size_t bytes = copyStorage.used();
        auto ptr = canAllocateInStorage(bytes) ? storage.push(bytes) : std::malloc(bytes);
        return static_cast<ErasedDelegate*>(ptr);
    }

private:

    [[nodiscard]]
    bool canAllocateInStorage(size_t bytes) const
    {
        return storage.isEmpty() && storage.canAllocate(bytes);
    }

    [[nodiscard]]
    bool hasStorageAllocation(DelegateType* delegate) const
    {
        return storage.contains(delegate);
    }

private:

    Storage& storage;
};

} // namespace TGEngine::Core

#endif // TGENGINE_DELEGATE_ALLOCATOR_H
