#ifndef IACTION_H
#define IACTION_H

#include "Utils/ForwardDeclarations/Game.h"
#include "Utils/Types/CppTypes.h"

namespace TGEngine::Game
{

class IAction
{
public:

    virtual ~IAction() = default;

    virtual void doAction(const SharedPtr<Actor>& target) const = 0;
    [[nodiscard]] virtual bool check(const SharedPtr<Actor>& target) const = 0;
};

} // namespace TGEngine::Game

#endif // IACTION_H