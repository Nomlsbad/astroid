#include "Core/Delegates/DelegateAllocator.h"

#include "AlocationDeffender.h"
#include "Core/Memory/StackStorage.h"
#include "Core/Types/Callable.h"

#include <gtest/gtest.h>

using namespace TGEngine::Core::_DelegateInternals;
using namespace TGEngine::Core;

using Storage = StackStorage<64, 8>;
using SmallStorage = StackStorage<8, 8>;

TEST(DelegateAllocatorTest, CopyIsEqualToOriginalAllocator)
{
    Storage storage;
    DelegateAllocator<IDelegate<void()>, Storage> allocator1(storage);
    DelegateAllocator allocator2 = allocator1;

    EXPECT_EQ(allocator1, allocator2);
}

TEST(DelegateAllocatorTest, AnyAllocatorsWithDifferentDelegateTypeSignatureArentEquals)
{
    Storage storage;
    DelegateAllocator<IDelegate<void()>, Storage> allocator1(storage);
    DelegateAllocator<IDelegate<int(int)>, Storage> allocator2(storage);

    EXPECT_NE(allocator1, allocator2);
}

TEST(DelegateAllocatorTest, AnyAllocatorsWithTheDifferentStoragesArentEquals)
{
    Storage storage1;
    DelegateAllocator<IDelegate<void()>, Storage> allocator1(storage1);

    Storage storage2;
    DelegateAllocator<IDelegate<void()>, Storage> allocator2(storage2);

    EXPECT_NE(allocator1, allocator2);
}

TEST(DelegateAllocatorTest, AllocatorsWithSameStorageAndCommonBaseDelegateTypeAreEquals)
{
    Storage storage;
    DelegateAllocator<IDelegate<void()>, Storage> allocator1(storage);
    DelegateAllocator<StaticDelegate<FunctionPtr<void>, void()>, Storage> allocator2(storage);
    DelegateAllocator<StaticDelegate<FunctionPtr<void>, void(), double, int>, Storage> allocator3(storage);

    EXPECT_EQ(allocator1, allocator2);
    EXPECT_EQ(allocator2, allocator3);
}

TEST(DelegateAllocatorTest, AlocatesInStorageIfEnoughFreeSpace)
{
    using DelegateType = StaticDelegate<FunctionPtr<void>, void()>;

    Storage storage;
    DelegateAllocator<DelegateType, Storage> allocator(storage);
    AllocationDeffender allocDeffender(allocator);

    const DelegateType* delegate = allocDeffender.allocate(1);

    EXPECT_NE(delegate, nullptr);
    EXPECT_TRUE(sizeof(DelegateType) <= storage.size);
    EXPECT_TRUE(storage.contains(delegate));
}

TEST(DelegateAllocatorTest, AlocatesInHeapIfNotEnoughFreeSpace)
{
    using DelegateType = StaticDelegate<FunctionPtr<void>, void(), double>;

    SmallStorage storage;
    DelegateAllocator<DelegateType, SmallStorage> allocator(storage);
    AllocationDeffender allocDeffender(allocator);

    const DelegateType* delegate = allocDeffender.allocate(1);

    EXPECT_NE(delegate, nullptr);
    EXPECT_TRUE(sizeof(DelegateType) > storage.size);
    EXPECT_FALSE(storage.contains(delegate));
}

TEST(DelegateAllocatorTest, AllocatesInStorageOnlyOneObject)
{
    using DelegateType = StaticDelegate<FunctionPtr<void>, void()>;

    Storage storage;
    DelegateAllocator<DelegateType, Storage> allocator(storage);
    AllocationDeffender allocDeffender(allocator);

    const DelegateType* delegate1 = allocDeffender.allocate(1);
    const DelegateType* delegate2 = allocDeffender.allocate(1);

    EXPECT_NE(delegate1, nullptr);
    EXPECT_NE(delegate2, nullptr);
    EXPECT_TRUE(2 * sizeof(DelegateType) <= storage.size);
    EXPECT_TRUE(storage.contains(delegate1));
    EXPECT_FALSE(storage.contains(delegate2));
}

TEST(DelegateAllocatorTest, AllocateReservesSizeofValueTypeBytesWhenNIsLarge)
{
    using DelegateType = StaticDelegate<FunctionPtr<void>, void()>;

    Storage storage;
    DelegateAllocator<DelegateType, Storage> allocator(storage);
    AllocationDeffender allocDeffender(allocator);

    const DelegateType* delegate = allocDeffender.allocate(100);

    EXPECT_NE(delegate, nullptr);
    EXPECT_EQ(storage.used(), sizeof(DelegateType));
}

TEST(DelegateAllocatorTest, AllocateReservesSizeofValueTypeBytesWhenNIsSmall)
{
    using DelegateType = StaticDelegate<FunctionPtr<void>, void()>;

    Storage storage;
    DelegateAllocator<DelegateType, Storage> allocator(storage);
    AllocationDeffender allocDeffender(allocator);

    const DelegateType* delegate = allocDeffender.allocate(0);

    EXPECT_NE(delegate, nullptr);
    EXPECT_EQ(storage.used(), sizeof(DelegateType));
}

TEST(DelegateAllocatorTest, AfterDeallocationStorageIsEmpty)
{
    Storage storage;
    using DelegateType = StaticDelegate<FunctionPtr<void>, void()>;
    DelegateAllocator<DelegateType, Storage> allocator(storage);

    DelegateType* delegate = allocator.allocate(1);
    const size_t usedAfterAlloc = storage.used();

    allocator.deallocate(delegate, 1);
    const size_t usedAfterDealloc = storage.used();

    EXPECT_NE(usedAfterAlloc, 0);
    EXPECT_EQ(usedAfterDealloc, 0);
}

TEST(DelegateAllocatorTest, DeallocateErasesWholeStorageWhenNIsLarge)
{
    Storage storage;
    using DelegateType = StaticDelegate<FunctionPtr<void>, void()>;
    DelegateAllocator<DelegateType, Storage> allocator(storage);

    DelegateType* delegate = allocator.allocate(1);
    const size_t usedAfterAlloc = storage.used();

    allocator.deallocate(delegate, 100);
    const size_t usedAfterDealloc = storage.used();

    EXPECT_NE(usedAfterAlloc, 0);
    EXPECT_EQ(usedAfterDealloc, 0);
}

TEST(DelegateAllocatorTest, DeallocateErasesWholeStorageWhenNIsSmall)
{
    Storage storage;
    using DelegateType = StaticDelegate<FunctionPtr<void>, void()>;
    DelegateAllocator<DelegateType, Storage> allocator(storage);

    DelegateType* delegate = allocator.allocate(1);
    const size_t usedAfterAlloc = storage.used();

    allocator.deallocate(delegate, 0);
    const size_t usedAfterDealloc = storage.used();

    EXPECT_NE(usedAfterAlloc, 0);
    EXPECT_EQ(usedAfterDealloc, 0);
}

TEST(DelegateAllocatorTest, AllocateCopyReservesTheSameAmountOfMemoryAsAllocatedInOtherStorage)
{
    Storage storage;
    using DelegateType = StaticDelegate<FunctionPtr<void>, void()>;
    DelegateAllocator<DelegateType, Storage> allocator(storage);

    Storage copyStorage;
    DelegateAllocator<DelegateType::Erased, Storage> erasedAllocator(copyStorage);

    DelegateType* srcP = allocator.allocate(1);
    DelegateType::Erased* dstP = erasedAllocator.allocateCopy(storage);

    EXPECT_NE(srcP, nullptr);
    EXPECT_NE(dstP, nullptr);
    EXPECT_EQ(storage.used(), copyStorage.used());

    allocator.deallocate(srcP, 1);
    erasedAllocator.deallocate(dstP, 1);
}

TEST(DelegateAllocatorTest, CantAllocateCopyInTheSameStorage)
{
    Storage storage;
    using DelegateType = StaticDelegate<FunctionPtr<void>, void()>;
    DelegateAllocator<DelegateType, Storage> allocator(storage);
    DelegateAllocator<DelegateType::Erased, Storage> erasedAllocator(storage);

    DelegateType* srcP = allocator.allocate(1);
    const size_t used = storage.used();

    DelegateType::Erased* dstP = erasedAllocator.allocateCopy(storage);

    EXPECT_NE(srcP, nullptr);
    EXPECT_NE(dstP, nullptr);
    EXPECT_EQ(storage.used(), used);
    EXPECT_FALSE(storage.contains(dstP));

    allocator.deallocate(srcP, 1);
    erasedAllocator.deallocate(dstP, 1);
}