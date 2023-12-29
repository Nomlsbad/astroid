#ifndef IACTION_H
#define IACTION_H

#include "Utils/ForwardDeclarations/Core.h"
#include "Utils/Types/CppTypes.h"

namespace TGEngine::Gameplay
{

class IAction
{
public:

    virtual ~IAction() = default;

    virtual void doAction(const SharedPtr<Core::Actor>& target) const = 0;
    [[nodiscard]] virtual bool check(const SharedPtr<Core::Actor>& target) const = 0;
};

} // namespace TGEngine::Gameplay

#endif // IACTION_H