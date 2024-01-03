#ifndef WORLD_H
#define WORLD_H

#include "Utils/ForwardDeclarations/Game.h"
#include "Utils/Types/CppTypes.h"

#include <vector>

namespace TGEngine::Game
{

class World final
{
public:

    static World& instance();

private:

    World() = default;

public:

    [[nodiscard]] SharedPtr<Level> findLevelByName(const std::string& name) const;
    [[nodiscard]] SharedPtr<Level> findActorLocation(const SharedPtr<Actor>& actor) const;

private:

    std::vector<SharedPtr<Level>> levels;
};

} // namespace TGEngine::Game

#endif // WORLD_H
