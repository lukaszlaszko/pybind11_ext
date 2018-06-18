#pragma once

#include <pybind11_ext/boost/circular_buffer.hpp>

#include <type_traits>


namespace pybind11 {

template <typename T, typename V>
inline readonly_memoryview to_memoryview(detail::numpy_circular_buffer<T>& c_buffer, ptrdiff_t offset)
{
    auto first_element = c_buffer.array_one();
    pybind11::buffer_info buffer(
            reinterpret_cast<uint8_t*>(first_element.first) + offset,   /* Pointer to buffer */
            sizeof(V),                                                  /* Size of one scalar */
            pybind11::format_descriptor<V>::format(),                   /* Python struct-style format descriptor */
            1,                                                          /* Number of dimensions */
            {c_buffer.size()},                                          /* Buffer dimensions */
            {sizeof(T)});                                               /* Number of entries between adjacent entries (for each per dimension) */

    return pybind11::readonly_memoryview(buffer);
}

template <typename T, typename V>
inline pybind11::array_t<V> to_array(detail::numpy_circular_buffer<T>& c_buffer, ptrdiff_t offset)
{
    static_assert(std::is_scalar<V>::value, "scalar type is expected as array element!");

    static constexpr auto numpy_module = "numpy";
    static constexpr auto array_attribute = "array";
    static constexpr auto copy_arg = "copy";

    auto numpy = pybind11::module::import(numpy_module);
    auto array = numpy.attr(array_attribute).cast<pybind11::function>();

    auto first_element = c_buffer.array_one();
    pybind11::buffer_info buffer(
            reinterpret_cast<uint8_t*>(first_element.first) + offset,   /* Pointer to buffer */
            sizeof(V),                                                  /* Size of one scalar */
            pybind11::format_descriptor<V>::format(),                   /* Python struct-style format descriptor */
            1,                                                          /* Number of dimensions */
            {c_buffer.size()},                                          /* Buffer dimensions */
            {sizeof(T)});                                               /* Number of entries between adjacent entries (for each per dimension) */

    pybind11::readonly_memoryview view(buffer);
    auto result = array(view, pybind11::arg(copy_arg) = false);

    return result;
}

}
