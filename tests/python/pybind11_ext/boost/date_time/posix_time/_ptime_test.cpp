#include <pybind11_ext/boost/date_time/posix_time/ptime.hpp>

#include <boost/date_time/posix_time/ptime.hpp>
#include <pybind11/pybind11.h>

#include <sstream>
#include <stdexcept>
#include <string>

using namespace std;
using namespace boost::posix_time;
namespace py = pybind11;


PYBIND11_MODULE(_ptime_test, m) {

    m.def("compare_ptime", [](const ptime& ptime_arg, const string& str_arg)
    {
        if (!str_arg.empty())
        {
            auto timepoint = time_from_string(str_arg);
            if (timepoint != ptime_arg)
            {
                stringstream message;
                message << "ptime different from parsed timepoint! expected: " 
                        << timepoint << " is: " << ptime_arg; 
                
                throw runtime_error(message.str());
            }
        }
        else
        {
            if (!ptime_arg.is_not_a_date_time())
                throw runtime_error("ptime should be not_a_date_time!");
        }

    });

}


