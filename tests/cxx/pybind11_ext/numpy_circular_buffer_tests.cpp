#include <pybind11_ext/numpy_circular_buffer.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <boost/optional.hpp>
#include <pybind11/embed.h>

#include <string>
#include <vector>

using namespace std;
using namespace testing;
namespace py = pybind11;


TEST(numpy_circular_buffer, to_array__int)
{
    py::detail::numpy_circular_buffer<int> buffer(1024);

    // empty
    {
        auto array = py::to_array(buffer);
        ASSERT_EQ(array.size(), ssize_t(buffer.size()));
    }

    // fill with data
    {
        buffer.push_back(1);
        buffer.push_back(2);
        buffer.push_back(3);

        auto array = py::to_array(buffer);
        ASSERT_EQ(array.size(), ssize_t(buffer.size()));
        ASSERT_EQ(array.at(0), 1);
        ASSERT_EQ(array.at(1), 2);
        ASSERT_EQ(array.at(2), 3);
    }

    // consume head
    {
        buffer.pop_front();

        auto array = py::to_array(buffer);
        ASSERT_EQ(array.size(), ssize_t(buffer.size()));
        ASSERT_EQ(array.at(0), 2);
        ASSERT_EQ(array.at(1), 3);
    }

    // fill capacity
    {
        for (auto i = 0u; i < 2 * buffer.capacity(); i++)
            buffer.push_back(i);

        auto array = py::to_array(buffer);
        ASSERT_EQ(array.size(), ssize_t(buffer.size()));

        for (auto i = 0u; i < buffer.size(); i++)
            ASSERT_EQ(buffer.at(i), array.at(i));
    }

    // consume all
    {
        while (!buffer.empty())
            buffer.pop_front();

        auto array = py::to_array(buffer);
        ASSERT_EQ(array.size(), 0u);
    }
}

TEST(numpy_circular_buffer, to_array__custom)
{
    struct alignas(32) test_type
    {
        int field_1;
        double field_2;
        char field_3;
    };

    py::detail::numpy_circular_buffer<test_type> buffer(1);
    ASSERT_EQ(buffer.capacity(), size_t(getpagesize() / sizeof(test_type)));

    // empty
    {
        auto array = py::to_array<test_type, int>(buffer, offsetof(test_type, field_1));
        ASSERT_EQ(array.size(), ssize_t(buffer.size()));
    }

    // fill with data
    {
        buffer.push_back({1, 1.1, 'a'});
        buffer.push_back({2, 2.2, 'b'});
        buffer.push_back({3, 3.3, 'c'});

        auto array_field_1 = py::to_array<test_type, int>(buffer, offsetof(test_type, field_1));
        ASSERT_EQ(array_field_1.size(), ssize_t(buffer.size()));
        ASSERT_EQ(array_field_1.at(0), 1);
        ASSERT_EQ(array_field_1.at(1), 2);
        ASSERT_EQ(array_field_1.at(2), 3);

        auto array_field_2 = py::to_array<test_type, double>(buffer, offsetof(test_type, field_2));
        ASSERT_DOUBLE_EQ(array_field_2.size(), buffer.size());
        ASSERT_DOUBLE_EQ(array_field_2.at(0), 1.1);
        ASSERT_DOUBLE_EQ(array_field_2.at(1), 2.2);
        ASSERT_DOUBLE_EQ(array_field_2.at(2), 3.3);

        auto array_field_3 = py::to_array<test_type, char>(buffer, offsetof(test_type, field_3));
        ASSERT_EQ(array_field_3.size(), ssize_t(buffer.size()));
        ASSERT_EQ(array_field_3.at(0), 'a');
        ASSERT_EQ(array_field_3.at(1), 'b');
        ASSERT_EQ(array_field_3.at(2), 'c');
    }

    // consume all
    {
        while (!buffer.empty())
            buffer.pop_front();

        auto array = py::to_array<test_type, int>(buffer);
        ASSERT_EQ(array.size(), 0u);
    }
}
