#pragma once

#include "readonly_memoryview.hpp"

#include <pybind11_ext/boost/circular_buffer.hpp>

#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>


namespace pybind11 {

template <typename T, typename V = T>
readonly_memoryview to_memoryview(detail::numpy_circular_buffer<T>& buffer, ptrdiff_t offset = 0u);

template <typename T, typename V = T>
array_t<V> to_array(detail::numpy_circular_buffer<T>& buffer, ptrdiff_t offset = 0u);

}

#include "numpy_circular_buffer.ipp"
