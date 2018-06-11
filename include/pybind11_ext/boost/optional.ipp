#pragma once

namespace pybind11 { namespace detail {

template <typename T>
handle boost_optional_caster<T>::cast(const T& src, return_value_policy policy, handle parent)
{
    if (!src)
        return none().inc_ref();

    return value_conv::cast(*src, policy, parent);
}

template <typename T>
bool boost_optional_caster<T>::load(handle src, bool convert)
{
    if (!src)
        return false;

    if (src.is_none())
    {
        value = boost::none;  // nullopt
        return true;
    }

    value_conv inner_caster;
    if (!inner_caster.load(src, convert))
        return false;

    value.emplace(cast_op<typename T::value_type>(inner_caster));
    return true;
}


} }
