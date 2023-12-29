#ifndef CHANGELEVEL_H
#define CHANGELEVEL_H

#include "Gameplay/Actions/IAction.h"
#include "Utils/ForwardDeclarations/Core.h"

namespace TGEngine::Gameplay
{

class ChangeLevel final: public IAction
{
public:

    explicit ChangeLevel(WeakPtr<Core::Level> level);

    void doAction(const SharedPtr<Core::Actor>& target) const override;
    [[nodiscard]] bool check(const SharedPtr<Core::Actor>& target) const override;

private:

    WeakPtr<Core::Level> level;
};

} // namespace TGEngine::Gameplay

#endif // CHANGELEVEL_H