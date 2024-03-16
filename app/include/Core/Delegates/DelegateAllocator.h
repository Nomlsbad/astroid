#ifndef DELEGATEALLOCATOR_H
#define DELEGATEALLOCATOR_H

#include "Core/Memory/StackAllocator.h"
#include "DelegateTypes.h"

namespace TGEngine::Core
{

template<typename DelegateSignature>
class DelegateAllocator
{
public:

    inline static constexpr size_t size = 32;
    inline static constexpr size_t align = 8;

    using Storage = StackStorage<size, align>;

    DelegateAllocator()
        : storage()
    {}

    using ErasedDelegate = IDelegate<DelegateSignature>*;

    ErasedDelegate allocate(size_t n)
    {
        if (storage.used() + n < size)
        {
            return storage.push(n);
        }

    }

private:

    Storage storage;
};

} // namespace TGEngine::Core

#endif // DELEGATEALLOCATOR_H
