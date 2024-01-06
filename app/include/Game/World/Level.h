/**
 * @file  Level.h
 * @brief Level class declaration.
 */

#ifndef LEVEL_H
#define LEVEL_H

#include "Utils/ForwardDeclarations/Game.h"
#include "Utils/Types/CppTypes.h"

#include <set>

namespace TGEngine::Game
{

/**
 * @class Level
 * @brief Storage for game actors.
 *
 * Anyone actor is owned by the only ONE level object.
 * So any pawn or building must be placed in the level.
 */
class Level
{
public:

    /**
     * @brief Level constructor
     * @param name level's string name
     */
    explicit Level(std::string name);

    Level(const Level& level) = delete;
    Level& operator= (const Level& level) = delete;

    Level(Level&& level) = default;
    Level& operator= (Level&& level) = default;

public:

    /**
     * @brief Getter for level's name
     * @return string with level's name
     */
    [[nodiscard]]
    constexpr std::string getName() const
    {
        return name;
    }

    /**
     * @brief Moves actor from his level to that level
     * @param actor shared pointer to actor that is being moved
     */
    void transferActor(const SharedPtr<Actor>& actor);

    /**
     * @brief Checks does level hold some actor or not
     * @param actor shared pointer to target actor
     * @return true if level contains this actor, false in other case
     */
    [[nodiscard]]
    bool isAtLevel(const SharedPtr<Actor>& actor) const;

private:

    std::string name;

    std::set<SharedPtr<Actor>> actors;
};

} // namespace TGEngine::Game

#endif // LEVEL_H