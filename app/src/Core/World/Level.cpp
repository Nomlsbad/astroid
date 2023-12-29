#include "Core/World/Level.h"

#include "Core/World/World.h"

using TGEngine::Core::Level;
using TGEngine::Core::World;

Level::Level(std::string name): name(std::move(name)) {}

void Level::transferActor(const SharedPtr<Actor>& actor)
{
    const auto& level = World::instance().findActorLocation(actor);
    if (!level) return;

    actors.insert(level->actors.extract(actor));
}

bool Level::isAtLevel(const SharedPtr<Actor>& actor) const
{
    return actors.contains(actor);
}