#pragma once

#include "time_duration.hpp"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>

#include <datetime.h>


namespace pybind11 { namespace detail {

inline bool type_caster<boost::posix_time::ptime>::load(handle src, bool)
{
    using namespace boost::posix_time;
    using namespace boost::gregorian;

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
        // If invoked with datetime.datetime object
    else if (PyDateTime_Check(src.ptr()))
    {
        auto year = PyDateTime_GET_YEAR(src.ptr());
        auto month = PyDateTime_GET_MONTH(src.ptr());
        auto day = PyDateTime_GET_DAY(src.ptr());

        auto hour = PyDateTime_DATE_GET_HOUR(src.ptr());
        auto minute = PyDateTime_DATE_GET_MINUTE(src.ptr());
        auto second = PyDateTime_DATE_GET_SECOND(src.ptr());
        auto microsecond = PyDateTime_DATE_GET_MICROSECOND(src.ptr());

        date _date(year, month, day);
        time_duration time(hour, minute, second, 0);
        time += microseconds(microsecond);

        value = ptime(_date, time);
        return true;
    }
    else
        return false;
}

inline handle type_caster<boost::posix_time::ptime>
    ::cast(const boost::posix_time::ptime &src, return_value_policy /* policy */, handle /* parent */)
{
    using namespace boost::posix_time;

    if (!PyDateTimeAPI) { PyDateTime_IMPORT; }

    if (src.is_not_a_date_time())
        return none();

    auto date = src.date();
    auto time = src.time_of_day();
    return PyDateTime_FromDateAndTime(
            date.year(),
            date.month(),
            date.day(),
            time.hours(),
            time.minutes(),
            time.seconds(),
            get_usecs(time));
}

} }
