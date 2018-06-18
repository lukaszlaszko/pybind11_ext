#include <pybind11_ext/boost/circular_buffer.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <boost/optional.hpp>

#include <string>
#include <vector>

using namespace std;
using namespace testing;
namespace py = pybind11;


TEST(circular_buffer, create__char__single)
{
    py::detail::numpy_circular_buffer<char> buffer(1);
    ASSERT_EQ(buffer.capacity(), size_t(getpagesize()));
}

TEST(circular_buffer, create__int__single)
{
    py::detail::numpy_circular_buffer<int> buffer(1);
    ASSERT_EQ(buffer.capacity(), size_t(getpagesize() / sizeof(int)));
}

TEST(circular_buffer, create__custom__not_aligned__single)
{
    struct test_type
    {
        int field_1;
        double field_2;
        char field_3;
    };

    ASSERT_ANY_THROW(py::detail::numpy_circular_buffer<test_type>(1));
}

TEST(circular_buffer, create__custom__aligned__single)
{
    struct alignas(16) test_type
    {
        int field_1;
        double field_2;
        char field_3;
    };

    py::detail::numpy_circular_buffer<test_type> buffer(1);
    ASSERT_EQ(buffer.capacity(), size_t(getpagesize() / sizeof(test_type)));
}
