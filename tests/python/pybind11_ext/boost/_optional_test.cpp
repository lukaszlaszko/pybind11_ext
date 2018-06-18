#include <pybind11_ext/boost/optional.hpp>

#include <boost/optional.hpp>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <string>
#include <tuple>
#include <vector>

using namespace std;
namespace py = pybind11;


PYBIND11_MODULE(_optional_test, m) {

    m.def("with_optional_float", [](boost::optional<double> arg)
    {
        return make_tuple(bool(arg), arg ? *arg : 0.0);
    });

    m.def("with_optional_str", [](boost::optional<string> arg)
    {
        return make_tuple(bool(arg), arg ? *arg : string());
    });

    m.def("with_optional_list", [](boost::optional<vector<int>> arg)
    {
        return make_tuple(bool(arg), arg ? *arg : vector<int>());
    });

}

