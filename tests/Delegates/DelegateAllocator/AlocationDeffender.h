#ifndef TGENGINE_ALOCATION_DEFFENDER_H
#define TGENGINE_ALOCATION_DEFFENDER_H

#include <vector>

template<typename Allocator>
class AllocationDeffender
{
public:

    using DelegateType = typename Allocator::value_type;

    explicit AllocationDeffender(Allocator& allocator)
        : allocator(allocator)
        , allocations()
    {}

    const DelegateType* allocate(std::size_t n)
    {
        DelegateType* delegate = allocator.allocate(n);
        allocations.push_back({delegate, n});
        return delegate;
    }

    ~AllocationDeffender()
    {
        for (auto iter = allocations.rbegin(); iter != allocations.rend(); ++iter)
        {
            auto& [delegate, n] = *iter;
            allocator.deallocate(delegate, n);
        }
    }

private:

    Allocator& allocator;
    std::vector<std::pair<DelegateType*, std::size_t>> allocations;
};

#endif //TGENGINE_ALOCATION_DEFFENDER_H
