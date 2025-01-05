#include "Core/Memory/StackStorage.h"

#include <algorithm>
#include <array>
#include <gtest/gtest.h>

using namespace TGEngine::Core;

TEST(StackStorageTest, StorageIsEmptyByDefault)
{
    constexpr size_t size = 64;
    const StackStorage<size> storage;

    const bool isEmpty = storage.isEmpty();

    EXPECT_TRUE(isEmpty);
}

TEST(StackStorageTest, CantAllocateMoreThanSize)
{
    constexpr size_t size = 64;
    const StackStorage<size> storage;

    const bool canAllocate = storage.canAllocate(size + 1);

    EXPECT_FALSE(canAllocate);
}

TEST(StackStorageTest, CantAllocateZeroBytes)
{
    constexpr size_t size = 64;
    const StackStorage<size> storage;

    constexpr bool canAllocate = storage.canAllocate(0);

    EXPECT_FALSE(canAllocate);
}

TEST(StackStorageTest, CanAllocateBetweenZeroAndSizeBytes)
{
    constexpr size_t size = 64;
    StackStorage<size> storage;

    bool canAllocateForAllSizes = true;
    for (size_t bytes = 1; bytes < size; ++bytes)
    {
        canAllocateForAllSizes &= storage.canAllocate(bytes);
    }

    EXPECT_TRUE(canAllocateForAllSizes);
}

TEST(StackStorageTest, PushReturnsNullptrIfCantAllocate)
{
    constexpr size_t size = 64;
    StackStorage<size> storage;

    void* zeroBytesBlock = storage.allocate(0);
    void* largeBlock = storage.allocate(size + 1);

    EXPECT_EQ(zeroBytesBlock, nullptr);
    EXPECT_EQ(largeBlock, nullptr);
}

TEST(StackStorageTest, StorageIsntEmptyAfterPush)
{
    constexpr size_t size = 64;
    StackStorage<size> storage;

    std::ignore = storage.allocate(size);
    const bool isEmpty = storage.isEmpty();

    EXPECT_FALSE(isEmpty);
}

TEST(StackStorageTest, StorageIsEmptyWhenUsedEqualsZero)
{
    constexpr size_t size = 64;
    StackStorage<size> storage;

    const bool isEmpty = storage.isEmpty();
    const size_t used = storage.used();

    EXPECT_TRUE(isEmpty);
    EXPECT_EQ(used, 0);
}

TEST(StackStorageTest, StorageIsntEmptyWhenUsedDoesntEqualZero)
{
    constexpr size_t size = 64;
    StackStorage<size> storage;

    std::ignore = storage.allocate(1);
    const bool isEmpty = storage.isEmpty();
    const size_t used = storage.used();

    EXPECT_FALSE(isEmpty);
    EXPECT_NE(used, 0);
}

TEST(StackStorageTest, StorageAlignsData)
{
    constexpr size_t size = 64;
    constexpr size_t align = 8;
    StackStorage<size, align> storage;

    std::array<std::uintptr_t, 3> addresses {};
    addresses[0] = reinterpret_cast<std::uintptr_t>(storage.allocate(align));
    addresses[1] = reinterpret_cast<std::uintptr_t>(storage.allocate(1));
    addresses[2] = reinterpret_cast<std::uintptr_t>(storage.allocate(2 * align - 5));

    for (auto address: addresses)
    {
        EXPECT_TRUE(address % align == 0);
    }
    EXPECT_EQ(storage.used(), 32);
}

TEST(StackStorageTest, AligmentEqualsMaxAlignByDefault)
{
    constexpr size_t size = 64;
    StackStorage<size> storage;

    const auto adress = reinterpret_cast<std::uintptr_t>(storage.allocate(1));

    EXPECT_TRUE(adress % alignof(std::max_align_t) == 0);
}

TEST(StackStorageTest, CanPushWhileHasFreeSpace)
{
    constexpr size_t size = 64;
    constexpr size_t align = 8;
    StackStorage<size, align> storage;

    constexpr size_t n = size / align + 1;
    std::array<void*, n> addresses {};
    std::ranges::generate(addresses, [&storage] { return storage.allocate(1); });

    for (auto iter = addresses.begin(); iter < std::prev(addresses.end()); ++iter)
    {
        EXPECT_NE(*iter, nullptr);
    }
    EXPECT_EQ(addresses.back(), nullptr);
}

TEST(StackStorageTest, ConstainsReturnsTrueForPointersFromStorage)
{
    constexpr size_t size = 64;
    StackStorage<size> storage;

    const void* p1 = storage.allocate(16);
    const void* p2 = storage.allocate(32);

    const bool p1InStorage = storage.contains(p1);
    const bool p2InStorage = storage.contains(p2);

    EXPECT_TRUE(p1InStorage);
    EXPECT_TRUE(p2InStorage);
}

TEST(StackStorageTest, ContainsReturnsFalseForNullptr)
{
    constexpr size_t size = 64;
    StackStorage<size> storage;

    void* p = storage.allocate(size + 1);
    const bool inStorage = storage.contains(p);

    EXPECT_EQ(p, nullptr);
    EXPECT_FALSE(inStorage);
}

TEST(StackStorageTest, ContainsReturnsFalseForPointersOutsideStorage)
{
    constexpr size_t size = 64;
    const StackStorage<size> storage;

    int a = 0;
    const bool aInStorage = storage.contains(&a);

    const void* p = new int(0);
    const bool pInStorage = storage.contains(p);

    EXPECT_FALSE(aInStorage);
    EXPECT_FALSE(pInStorage);
}

TEST(StackStorageTest, PopErasesStorage)
{
    constexpr size_t size = 64;
    StackStorage<size> storage;

    const void* p = storage.allocate(size);
    const bool storageWasFull = storage.used() == size;

    storage.deallocate(p, size);
    const bool storageIsEmpty = storage.isEmpty();

    EXPECT_TRUE(storageWasFull);
    EXPECT_TRUE(storageIsEmpty);
}

TEST(StackStorageTest, CanAllocateAfterPop)
{
    constexpr size_t size = 64;
    StackStorage<size> storage;

    const void* p = storage.allocate(1);
    const bool canAllocateBeforePop = storage.canAllocate(size);

    storage.deallocate(p, 1);
    const bool canAllocateAfterPop = storage.canAllocate(size);

    EXPECT_FALSE(canAllocateBeforePop);
    EXPECT_TRUE(canAllocateAfterPop);
}

TEST(StackStorageTest, PopSuportsAlign)
{
    constexpr size_t size = 64;
    constexpr size_t align = 8;
    StackStorage<size, align> storage;

    const void* p = storage.allocate(1);
    storage.deallocate(p, align);
    const bool storageIsEmpty = storage.isEmpty();

    EXPECT_TRUE(storageIsEmpty);
}

TEST(StackStorageTest, PopInReverseOrder)
{
    constexpr size_t size = 64;
    constexpr size_t align = 8;
    StackStorage<size, align> storage;

    const void* p1 = storage.allocate(1);
    const void* p2 = storage.allocate(1);

    storage.deallocate(p2, 1);
    storage.deallocate(p1, 1);

    const bool storageIsEmpty = storage.isEmpty();

    EXPECT_TRUE(storageIsEmpty);
}

TEST(StackStorageTest, CantPopPointerOutsideStorage)
{
    constexpr size_t size = 64;
    StackStorage<size> storage;

    EXPECT_DEBUG_DEATH(storage.deallocate(new int(0), sizeof(int)), "");
}

TEST(StackStorageTest, CantPopWrongSize)
{
    constexpr size_t size = 64;
    StackStorage<size, 1> storage;

    const void* p = storage.allocate(3);

    EXPECT_DEBUG_DEATH(storage.deallocate(p, 2), "");
    EXPECT_DEBUG_DEATH(storage.deallocate(p, 5), "");
}

TEST(StackStorageTest, CantPopInWrongOrder)
{
    constexpr size_t size = 64;
    StackStorage<size> storage;

    const void* p1 = storage.allocate(4);
    std::ignore = storage.allocate(4);

    EXPECT_DEBUG_DEATH(storage.deallocate(p1, 4), "");
}