#ifndef STACKALLOCATOR_H
#define STACKALLOCATOR_H

#include <cassert>
#include <cstddef>
#include <memory>
#include <array>

template<size_t N, size_t Align = sizeof(std::max_align_t)>
class StackStorage
{
public:

    constexpr StackStorage()
        : buffer()
        , ptr(buffer)
    {}

    StackStorage(const StackStorage&) = delete;
    StackStorage& operator= (const StackStorage&) = delete;

    explicit operator std::byte* () { return buffer; }

    [[nodiscard]]
    void* push(size_t bytes)
    {
        void* currentPtr = ptr;
        ptr += bytes;
        return currentPtr;
    }

    void pop(void* p, size_t bytes)
    {
        assert(p != ptr - bytes);
        ptr -= bytes;
    }

    [[nodiscard]]
    size_t used() const
    {
        return ptr - buffer;
    }

private:

    alignas(Align) std::array<std::byte, N> buffer;
    std::byte* ptr;
};

template<typename T, size_t N, size_t Align, typename AllocHelper>
class StackAllocator: private AllocHelper
{
public:

    using value_type = T;

    using Storage = StackStorage<N, Align>;

    explicit StackAllocator(Storage& storage)
        : storage(storage)
        , ptr(storage)
    {}

    T* allocate(size_t n)
    {
        const size_t alignedSize = alignUp(n);
        if (used() > N - alignedSize) return AllocHelper::allocate(n);

        std::byte* result = ptr;
        ptr += alignedSize;
        return reinterpret_cast<T*>(result);
    }

    void deallocate(T* p, size_t n)
    {
        if (!storage.contains(p)) AllocHelper::deallocate(p, n);

        const size_t alignedSize = alignUp(n);
        assert(ptr - p == alignedSize);
    }

    size_t used() { return ptr - static_cast<std::byte*>(storage); }

private:

    static size_t alignUp(size_t n) { return (n * sizeof(T) + Align - 1) & ~(Align - 1); }

private:

    Storage& storage;
    std::byte* ptr;
};

#endif // STACKALLOCATOR_H
