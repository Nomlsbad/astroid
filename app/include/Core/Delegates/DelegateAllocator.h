#ifndef DELEGATEALLOCATOR_H
#define DELEGATEALLOCATOR_H

#include "Core/Memory/StackStorage.h"
#include "DelegateTypes.h"

namespace TGEngine::Core
{

template<typename DelegateType>
class DelegateAllocator
{
public:

    inline static constexpr size_t size = 32;
    inline static constexpr size_t align = 8;

    using Storage = StackStorage<size, align>;

    using value_type = DelegateType;

    explicit DelegateAllocator(Storage& storage)
        : storage(storage)
    {}

    DelegateType* allocate(size_t n)
    {
       const size_t bytes = n * sizeof(DelegateType);
       void* ptr = canAllocateInStorage(bytes) ? storage.push(bytes) : std::malloc(bytes);
       return static_cast<DelegateType*>(ptr);
    }

    void deallocate(DelegateType* delegate, size_t)
    {
        const size_t used = storage.used();
        hasStorageAllocation(delegate) ? storage.pop(delegate, used) : std::free(delegate);
    }

    [[nodiscard]]
    bool canAllocateInStorage(size_t bytes) const
    {
        return storage.used() + bytes <= size && storage.isEmpty();
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

#endif // DELEGATEALLOCATOR_H
