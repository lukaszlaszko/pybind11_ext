#pragma once

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>

#include <datetime.h>


namespace pybind11 { namespace detail {

inline bool type_caster<boost::posix_time::time_duration>::load(handle src, bool)
{
    using namespace boost::posix_time;

    // Lazy initialise the PyDateTime import
    if (!PyDateTimeAPI) { PyDateTime_IMPORT; }

    if (!src)
        return false;

    // If invoked with None
    if (PyNone_Check(src.ptr()))
    {
        value = not_a_date_time;
        return true;
    }
    // If invoked with datetime.delta object
    else if (PyDelta_Check(src.ptr()))
    {
        auto total_days = PyDateTime_DELTA_GET_DAYS(src.ptr());
        auto total_seconds = PyDateTime_DELTA_GET_SECONDS(src.ptr());
        auto total_microseconds = PyDateTime_DELTA_GET_MICROSECONDS(src.ptr());

        auto is_negative = (total_days < 0);
        if (is_negative)
            total_days = -total_days;

        value = time_duration(hours(24) * total_days + seconds(total_seconds) + microseconds(total_microseconds));
        if (is_negative)
            value = value.invert_sign();

        return true;
    }
        // If invoked with a float we assume it is seconds and convert
    else if (PyFloat_Check(src.ptr()))
    {
        auto total_seconds = PyFloat_AsDouble(src.ptr());
        auto total_microseconds = (total_seconds - int(total_seconds)) * microseconds::num_fractional_digits();

        value = time_duration(seconds(total_seconds) + microseconds(total_microseconds));
        return true;
    }
    else
        return false;
}

inline handle type_caster<boost::posix_time::time_duration>
    ::cast(const boost::posix_time::time_duration &src, return_value_policy /* policy */, handle /* parent */)
{
    using namespace boost::posix_time;

    if (!PyDateTimeAPI) { PyDateTime_IMPORT; }

    if (src.is_not_a_date_time())
        return none();

    auto total_days = src.hours() / 24;
    if (total_days < 0)
        total_days --;

    auto total_seconds = src.total_seconds() - total_days * (24 * 3600);
    auto total_microseconds = get_usecs(src);
    if (total_days < 0)
        total_microseconds = 1000000-1 - total_microseconds;

    return PyDelta_FromDSU(total_days, total_seconds, total_microseconds);
}

} }
