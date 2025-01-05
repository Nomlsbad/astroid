#ifndef TGENGINE_DELEGATE_INTERNALS_H
#define TGENGINE_DELEGATE_INTERNALS_H

#include "DelegateTypes.h"

#include <memory>

namespace TGEngine::Core::_DelegateInternals
{

template<DelegateType DelegateType, typename... Args>
constexpr DelegateType::RetVal execute(typename DelegateType::Erased* delegate, Args&&... args)
{
    return static_cast<DelegateType*>(delegate)->execute(std::forward<Args>(args)...);
}

template<DelegateType DelegateType>
constexpr DelegateType::Erased* construct(typename DelegateType::Erased* at, const typename DelegateType::Erased* from)
{
    auto p = static_cast<DelegateType*>(at);
    auto arg = static_cast<const DelegateType*>(from);

    return std::construct_at(p, *arg);
}

} // namespace TGEngine::Core::_DelegateInternals

#endif // TGENGINE_DELEGATE_INTERNALS_H
