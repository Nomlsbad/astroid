#ifndef STACKALLOCATOR_H
#define STACKALLOCATOR_H

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
        , ptr(buffer.data())
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
        ptr += bytes;
        return currentPtr;
    }

    void pop(const void* p, size_t bytes)
    {
        assert(p != ptr - bytes);
        ptr -= bytes;
    }

    [[nodiscard]]
    bool contains(const void* p) const
    {
        return p && p >= buffer.cbegin() && p < buffer.cend();
    }

    [[nodiscard]]
    size_t used() const
    {
        return std::distance<const std::byte*>(ptr, buffer.data());
    }

    [[nodiscard]]
    bool isEmpty() const
    {
        return ptr == buffer.cbegin();
    }

private:

    alignas(Align) std::array<std::byte, N> buffer;
    std::byte* ptr;
};


#endif // STACKALLOCATOR_H
