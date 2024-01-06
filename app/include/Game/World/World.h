/**
 * @file World.h
 * @brief World class declaration.
 */

#ifndef WORLD_H
#define WORLD_H

#include "Utils/ForwardDeclarations/Game.h"
#include "Utils/Types/CppTypes.h"

#include <vector>

namespace TGEngine::Game
{

/**
 * @class World
 * @brief Levels storage.
 *
 * Singleton class taht contains all game levels
 */
class World final
{
public:

    /**
     * @brief Access to world instance.
     * @return ref to world instance
     */
    static World& instance();

private:

    World() = default;

public:

    /**
     * @brief Try to find level with given name.
     * @param name level's name
     * @return shared pointer to first level with given name or nullptr
     */
    [[nodiscard]]
    SharedPtr<Level> findLevelByName(const std::string& name) const;

    /**
     * @brief Try to find level with given actor.
     * @param actor shared pointer to target actor
     * @return shared pointer to level with given actor or nullptr
     */
    [[nodiscard]]
    SharedPtr<Level> findActorLocation(const SharedPtr<Actor>& actor) const;

private:

    std::vector<SharedPtr<Level>> levels;
};

} // namespace TGEngine::Game

#endif // WORLD_H