#pragma once

#include <boost/optional.hpp>
#include <pybind11/pybind11.h>


namespace pybind11 { namespace detail {

template<typename T>
class boost_optional_caster
{
public:
    using value_conv = make_caster<typename T::value_type>;

    static handle cast(const T& src, return_value_policy policy, handle parent);

    bool load(handle src, bool convert);

    PYBIND11_TYPE_CASTER(T, _("Optional[") + value_conv::name() + _("]"));

};

template<typename T> struct type_caster<boost::optional<T>>
: public boost_optional_caster<boost::optional<T>> {};

template<> struct type_caster<boost::none_t>
        : public void_caster<boost::none_t> {};

} }

#include "optional.ipp"
