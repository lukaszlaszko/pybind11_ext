#pragma once

#include <boost/date_time/posix_time/posix_time.hpp>
#include <pybind11/pybind11.h>


namespace pybind11 { namespace detail {

long get_usecs(boost::posix_time::time_duration const& d);

inline long get_usecs(boost::posix_time::time_duration const& d)
{
    using namespace boost::posix_time;

    auto resolution = boost::posix_time::time_duration::ticks_per_second();
    auto fractional_seconds = d.fractional_seconds();
    if (resolution > 1000000)
        return fractional_seconds / (resolution / 1000000);
    else
        return fractional_seconds * (1000000 / resolution);
}

template <>
class type_caster<boost::posix_time::time_duration>
{
public:
PYBIND11_TYPE_CASTER(boost::posix_time::time_duration, _("datetime.timedelta"));

    bool load(handle src, bool);
    static handle cast(const boost::posix_time::time_duration &src, return_value_policy /* policy */, handle /* parent */);

};

}} // namespace pybind11::detail

#include "time_duration.ipp"