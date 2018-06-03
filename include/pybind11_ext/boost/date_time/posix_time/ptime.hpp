#pragma once

#include <boost/date_time/posix_time/posix_time.hpp>
#include <pybind11/pybind11.h>


namespace pybind11 { namespace detail {

template <>
class type_caster<boost::posix_time::ptime>
{
public:
    PYBIND11_TYPE_CASTER(boost::posix_time::ptime, _("datetime.datetime"));

    bool load(handle src, bool);
    static handle cast(const boost::posix_time::ptime &src, return_value_policy /* policy */, handle /* parent */);

};

}} // namespace pybind11::detail

#include "ptime.ipp"
