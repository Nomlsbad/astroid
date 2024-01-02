#include "Game/World/Level.h"

#include "Game/World/World.h"

using TGEngine::Game::Level;
using TGEngine::Game::World;

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