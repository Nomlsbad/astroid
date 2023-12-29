#ifndef ACTOR_H
#define ACTOR_H

#include <Utils/Types/CppTypes.h>

namespace TRPGEngine::Core
{

class Actor: public std::enable_shared_from_this<Actor>
{
public:

    Actor() = default;
};

} // namespace TRPGEngine::Core

#endif // ACTOR_H