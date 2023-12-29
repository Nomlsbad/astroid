#include "Core/World/World.h"

#include "Core/World/Level.h"

using TGEngine::Core::Level;
using TGEngine::Core::World;
using TGEngine::Types::SharedPtr;

World& World::instance()
{
    static World world;
    return world;
}

SharedPtr<Level> World::findActorLocation(const SharedPtr<Actor>& actor) const
{
    for (const auto& level: levels)
    {
        if (level->isAtLevel(actor)) return level;
    }
    return nullptr;
}