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

} // namespace TRPGEngine::Types

namespace TGEngine::Core
{
using namespace TGEngine::Types;
} // namespace TRPGEngine::Core

namespace TGEngine::Gameplay
{
using namespace TGEngine::Types;
} // namespace TRPGEngine::Gameplay

#endif // TYPES_H
