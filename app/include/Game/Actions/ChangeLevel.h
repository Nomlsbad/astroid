#ifndef CHANGELEVEL_H
#define CHANGELEVEL_H

#include "Game/Actions/IAction.h"
#include "Utils/ForwardDeclarations/Game.h"

namespace TGEngine::Game
{

/**
 * @class ChangeLevel
 * @brief Action for changing level.
 */
class ChangeLevel final: public IAction
{
public:

    /**
     * @brief Action's constuctor.
     * @param level pointer to a new level
     */
    explicit ChangeLevel(WeakPtr<Level> level);

    [[nodiscard]]
    bool check(const SharedPtr<Actor>& target) const override;
    void doAction(const SharedPtr<Actor>& target) const override;

private:

    WeakPtr<Level> level;
};

} // namespace TGEngine::Game

#endif // CHANGELEVEL_H