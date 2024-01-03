#ifndef TYPES_H
#define TYPES_H

#include <memory>
#include <set>

namespace TGEngine::Types
{

template <typename T>
using SharedPtr = std::shared_ptr<T>;

template <typename T>
using WeakPtr = std::weak_ptr<T>;

} // namespace TGEngine::Types

namespace TGEngine::Game
{
using namespace TGEngine::Types;
} // namespace TGEngine::Game

#endif // TYPES_H