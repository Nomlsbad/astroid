#ifndef TGENGINE_STACK_ALLOCATOR_H
#define TGENGINE_STACK_ALLOCATOR_H

#include <array>
#include <cassert>
#include <cstddef>
#include <memory>

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

    static constexpr size_t size = N;
    static constexpr size_t align = Align;

public:

    [[nodiscard]]
    void* push(size_t bytes)
    {
        void* currentPtr = ptr;
        ptr += alignUp(bytes);
        return currentPtr;
    }

    void pop(const void* p, size_t bytes)
    {
        bytes = alignUp(bytes);
        assert(p == ptr - bytes);
        ptr -= bytes;
    }

    [[nodiscard]]
    bool contains(const void* p) const
    {
        return p && p >= buffer && p < buffer + N;
    }

    [[nodiscard]]
    size_t used() const
    {
        return static_cast<size_t>(ptr - buffer);
    }

    [[nodiscard]]
    bool isEmpty() const
    {
        return ptr == buffer;
    }

private:

    static size_t alignUp(std::size_t n) noexcept { return (n + (align - 1)) & ~(align - 1); }

private:

    alignas(align) std::byte buffer[size];
    std::byte* ptr;
};


#endif // TGENGINE_STACK_ALLOCATOR_H
