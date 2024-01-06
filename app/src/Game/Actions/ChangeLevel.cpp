#include "Game/Actions/ChangeLevel.h"

#include "Game/World/Level.h"

using TGEngine::Game::Actor;
using TGEngine::Game::ChangeLevel;
using TGEngine::Game::Level;

ChangeLevel::ChangeLevel(WeakPtr<Level> level)
    : level(std::move(level))
{}

void ChangeLevel::doAction(const SharedPtr<Actor>& target) const
{
    if (!check(target)) return;

    const auto levelSP = level.lock();
    levelSP->transferActor(target);
}

bool ChangeLevel::check(const SharedPtr<Actor>& target) const
{
    return target && !level.expired();
}