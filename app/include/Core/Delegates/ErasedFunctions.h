#ifndef TGENGINE_DELEGATE_INTERNALS_H
#define TGENGINE_DELEGATE_INTERNALS_H

#include <memory>

namespace TGEngine::Core::_DelegateInternals
{

struct ErasedFunctions
{
    template<typename DelegateType, typename... Args>
    constexpr static DelegateType::RetVal execute(DelegateType::Erased* delegate, Args&&... args)
    {
        return static_cast<DelegateType*>(delegate)->execute(std::forward<Args>(args)...);
    }

    template<typename DelegateType>
    constexpr static DelegateType::Erased* construct(DelegateType::Erased* at, const DelegateType::Erased* from)
    {
        auto p = static_cast<DelegateType*>(at);
        auto arg = static_cast<const DelegateType*>(from);

        return std::construct_at(p, *arg);
    }
};


} // namespace TGEngine::Core::_DelegateInternals

#endif // TGENGINE_DELEGATE_INTERNALS_H
