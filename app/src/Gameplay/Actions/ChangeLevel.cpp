#include "Gameplay/Actions/ChangeLevel.h"

#include "Core/World/Level.h"

using TGEngine::Core::Actor;
using TGEngine::Core::Level;
using TGEngine::Gameplay::ChangeLevel;

ChangeLevel::ChangeLevel(WeakPtr<Level> level): level(std::move(level)) {}

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