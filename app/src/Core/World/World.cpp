#include "Core/World/World.h"

#include "Core/World/Level.h"

#include <ranges>

using TGEngine::Core::Level;
using TGEngine::Core::World;
using TGEngine::Types::SharedPtr;

World& World::instance()
{
    static World world;
    return world;
}

SharedPtr<Level> World::findLevelByName(const std::string& name) const
{
    const auto level = std::ranges::find_if(levels, [&name](const auto& level) { return level->getName() == name; });
    if (level == levels.cend()) return nullptr;

    return *level;
}

SharedPtr<Level> World::findActorLocation(const SharedPtr<Actor>& actor) const
{
    const auto level = std::ranges::find_if(levels, [&actor](const auto& level) { return level->isAtLevel(actor); });
    if (level == levels.cend()) return nullptr;

    return *level;
}