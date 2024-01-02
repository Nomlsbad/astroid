#ifndef CHANGELEVEL_H
#define CHANGELEVEL_H

#include "Game/Actions/IAction.h"
#include "Utils/ForwardDeclarations/Game.h"

namespace TGEngine::Game
{

class ChangeLevel final: public IAction
{
public:

    explicit ChangeLevel(WeakPtr<Level> level);

    void doAction(const SharedPtr<Actor>& target) const override;
    [[nodiscard]] bool check(const SharedPtr<Actor>& target) const override;

private:

    WeakPtr<Level> level;
};

} // namespace TGEngine::Game

#endif // CHANGELEVEL_H