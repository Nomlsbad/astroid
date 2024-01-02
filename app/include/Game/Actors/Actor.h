#ifndef ACTOR_H
#define ACTOR_H

#include <Utils/Types/CppTypes.h>

namespace TGEngine::Game
{

class Actor: public std::enable_shared_from_this<Actor>
{
public:

    Actor() = default;
};

} // namespace TGEngine::Game

#endif // ACTOR_H