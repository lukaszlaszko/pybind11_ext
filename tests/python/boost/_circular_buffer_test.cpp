#include <pybind11_ext/boost/circular_buffer.hpp>

#include <boost/optional.hpp>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <string>
#include <tuple>
#include <vector>

using namespace std;
namespace py = pybind11;


PYBIND11_MODULE(_circular_buffer_test, m) {

    struct alignas(32) custom_type
    {
        int index;
        bool has_value;
        double value;
    };

    using int_circular_buffer = py::detail::numpy_circular_buffer<int>;
    using custom_circular_buffer = py::detail::numpy_circular_buffer<custom_type>;

    py::class_<int_circular_buffer>(m, "IntCircularBuffer")
            .def(py::init<size_t>())
            .def("push", [](int_circular_buffer& self, int element)
                    {
                        self.push_back(element);
                    })
            .def("pop", [](int_circular_buffer& self)
                    {
                        auto front = self.front();
                        self.pop_front();

                        return front;
                    })
            .def_property_readonly("capacity", &int_circular_buffer::capacity)
            .def_property_readonly("array", [](int_circular_buffer& self)
                    {
                        return self.to_array();
                    });

    py::class_<custom_circular_buffer>(m, "CustomCircularBuffer")
            .def(py::init<size_t>())
            .def("push", [](custom_circular_buffer& self, int index, bool has_value, double value)
                    {
                        self.push_back({index, has_value, value});
                    })
            .def("pop", [](custom_circular_buffer& self)
                    {
                        self.pop_front();
                    })
            .def_property_readonly("capacity", &custom_circular_buffer::capacity)
            .def_property_readonly("index_array", [](custom_circular_buffer& self)
                    {
                        return self.to_array<int>(offsetof(custom_type, index));
                    })
            .def_property_readonly("has_value_array", [](custom_circular_buffer& self)
                    {
                        return self.to_array<bool>(offsetof(custom_type, has_value));
                    })
            .def_property_readonly("value_array", [](custom_circular_buffer& self)
                    {
                        return self.to_array<double>(offsetof(custom_type, value));
                    });

}

