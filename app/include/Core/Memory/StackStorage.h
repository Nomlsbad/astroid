#ifndef TGENGINE_STACK_STORAGE_H
#define TGENGINE_STACK_STORAGE_H

#include <cassert>
#include <cstddef>

namespace TGEngine::Core
{

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
    constexpr void* allocate(size_t bytes)
    {
        if (!canAllocate(bytes)) return nullptr;

        void* currentPtr = ptr;
        ptr += alignUp(bytes);
        return currentPtr;
    }

    constexpr void deallocate([[maybe_unused]] const void* p, size_t bytes)
    {
        bytes = alignUp(bytes);
        assert(p == ptr - bytes);
        ptr -= bytes;
    }

    [[nodiscard]]
    constexpr bool canAllocate(size_t bytes) const
    {
        return bytes > 0 && used() + bytes <= size;
    }

    [[nodiscard]]
    constexpr bool contains(const void* p) const
    {
        return p && p >= buffer && p < buffer + N;
    }

    [[nodiscard]]
    constexpr size_t used() const
    {
        return static_cast<size_t>(ptr - buffer);
    }

    [[nodiscard]]
    constexpr bool isEmpty() const
    {
        return ptr == buffer;
    }

private:

    constexpr static size_t alignUp(std::size_t n) noexcept { return (n + (align - 1)) & ~(align - 1); }

private:

    alignas(align) std::byte buffer[size];
    std::byte* ptr;
};

} // namespace TGEngine::Core

#endif // TGENGINE_STACK_STORAGE_H
