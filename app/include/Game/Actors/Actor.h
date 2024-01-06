#ifndef ACTOR_H
#define ACTOR_H

#include <Utils/Types/CppTypes.h>

namespace TGEngine::Game
{

/**
 * @class Actor
 * @brief Minimal class that can be added to level.
 */
class Actor: public std::enable_shared_from_this<Actor>
{
public:

    /**
     * @brief Actor's constructor.
     */
    Actor() = default;

    /**
     * @brief Actor's destructor.
     */
    virtual ~Actor();
};

} // namespace TGEngine::Game

#endif // ACTOR_H