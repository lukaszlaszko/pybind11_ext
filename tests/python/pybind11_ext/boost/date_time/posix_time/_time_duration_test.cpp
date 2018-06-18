#include <pybind11_ext/boost/date_time/posix_time/time_duration.hpp>

#include <boost/date_time/posix_time/posix_time_duration.hpp>
#include <pybind11/pybind11.h>

#include <sstream>
#include <stdexcept>
#include <string>

using namespace std;
using namespace boost::posix_time;
namespace py = pybind11;


PYBIND11_MODULE(_time_duration_test, m) {

    m.def("compare_duration", [](const time_duration& duration_arg, const string& str_arg)
    {
        if (!str_arg.empty())
        {
            auto duration = duration_from_string(str_arg);
            if (duration != duration_arg)
            {
                stringstream message;
                message << "Given duration is different from parsed duration! expected: "
                        << duration << " is: " << duration_arg;

                throw runtime_error(message.str());
            }
        }
        else
        {
            if (!duration_arg.is_not_a_date_time())
                throw runtime_error("duration should be not_a_date_time!");
        }

    });

    m.def("parse_duration", [](const string& str_arg) -> time_duration
    {
        if (str_arg.empty())
            return not_a_date_time;

        return duration_from_string(str_arg);
    });

}


