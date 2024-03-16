#ifndef TGENGINE_CALLABLE_H
#define TGENGINE_CALLABLE_H

namespace TGEngine::Types
{

namespace internal
{

template<typename RetVal, typename T, bool IsConst, typename... Args>
struct MethodDeductionType
{
    using Type = RetVal (T::*)(Args...);
};

template<typename RetVal, typename T, typename... Args>
struct MethodDeductionType<RetVal, T, true, Args...>
{
    using Type = RetVal (T::*)(Args...) const;
};

} // namespace internal

template<typename RetVal, typename T, bool IsConst, typename... Args>
using MethodPtr = typename internal::MethodDeductionType<RetVal, T, IsConst, Args...>::Type;

template<typename T, typename RetVal, typename... Args>
using NotConstMethodPtr = RetVal (T::*)(Args...);

template<typename T, typename RetVal, typename... Args>
using ConstMethodPtr = RetVal (T::*)(Args...) const;

template<typename RetVal, typename... Args>
using FunctionPtr = RetVal (*)(Args...);

} // namespace TGEngine::Types


#endif // TGENGINE_CALLABLE_H
