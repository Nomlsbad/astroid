#ifndef IACTION_H
#define IACTION_H

#include "Utils/ForwardDeclarations/Game.h"
#include "Utils/Types/CppTypes.h"

namespace TGEngine::Game
{

/**
 * @class IAction
 * @brief Interface for game actions.
 *
 * Any actions like move to another level or attack should
 * implement this interface
 */
class IAction
{
public:

    virtual ~IAction() = default;

    /**
     * @brief Checks can target do actiono or not.
     * @param target actor that is going to do action
     * @return @c true if target can start action, else - @c false
     */
    [[nodiscard]]
    virtual bool check(const SharedPtr<Actor>& target) const = 0;

    /**
     * @brief Executes for doing action.
     * @param target actor that is going to do action
     */
    virtual void doAction(const SharedPtr<Actor>& target) const = 0;
};

} // namespace TGEngine::Game

#endif // IACTION_H