#include <pybind11_ext/boost/circular_buffer.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <boost/optional.hpp>
#include <pybind11/embed.h>

#include <string>
#include <vector>

using namespace std;
using namespace testing;
namespace py = pybind11;


TEST(numpy_circular_buffer, create__char__single)
{
    py::detail::numpy_circular_buffer<char> buffer(1);
    ASSERT_EQ(buffer.capacity(), getpagesize());

}

TEST(numpy_circular_buffer, create__int__single)
{
    py::detail::numpy_circular_buffer<int> buffer(1);
    ASSERT_EQ(buffer.capacity(), getpagesize() / sizeof(int));
}

TEST(numpy_circular_buffer, create__custom__not_aligned__single)
{
    struct test_type
    {
        int field_1;
        double field_2;
        char field_3;
    };

    ASSERT_ANY_THROW(py::detail::numpy_circular_buffer<test_type>(1));
}

TEST(numpy_circular_buffer, create__custom__aligned__single)
{
    struct alignas(16) test_type
    {
        int field_1;
        double field_2;
        char field_3;
    };

    py::detail::numpy_circular_buffer<test_type> buffer(1);
    ASSERT_EQ(buffer.capacity(), getpagesize() / sizeof(test_type));
}

TEST(numpy_circular_buffer, to_array__int)
{
    py::scoped_interpreter interpreter;

    py::detail::numpy_circular_buffer<int> buffer(1024);

    // empty
    {
        auto array = buffer.to_array();
        ASSERT_EQ(array.size(), buffer.size());
    }

    // fill with data
    {
        buffer.push_back(1);
        buffer.push_back(2);
        buffer.push_back(3);

        auto array = buffer.to_array();
        ASSERT_EQ(array.size(), buffer.size());
        ASSERT_EQ(array.at(0), 1u);
        ASSERT_EQ(array.at(1), 2u);
        ASSERT_EQ(array.at(2), 3u);
    }

    // consume head
    {
        buffer.pop_front();

        auto array = buffer.to_array();
        ASSERT_EQ(array.size(), buffer.size());
        ASSERT_EQ(array.at(0), 2u);
        ASSERT_EQ(array.at(1), 3u);
    }

    // fill capacity
    {
        for (auto i = 0u; i < 2 * buffer.capacity(); i++)
            buffer.push_back(i);

        auto array = buffer.to_array();
        ASSERT_EQ(array.size(), buffer.size());

        for (auto i = 0u; i < buffer.size(); i++)
            ASSERT_EQ(buffer.at(i), array.at(i));
    }

    // consume all
    {
        while (!buffer.empty())
            buffer.pop_front();

        auto array = buffer.to_array();
        ASSERT_EQ(array.size(), 0u);
    }
}

TEST(numpy_circular_buffer, to_array__custom)
{
    struct alignas(16) test_type
    {
        int field_1;
        double field_2;
        char field_3;
    };

    py::scoped_interpreter interpreter;
    py::detail::numpy_circular_buffer<test_type> buffer(1);
    ASSERT_EQ(buffer.capacity(), getpagesize() / sizeof(test_type));

    // empty
    {
        auto array = buffer.to_array<int>(offsetof(test_type, field_1));
        ASSERT_EQ(array.size(), buffer.size());
    }

    // fill with data
    {
        buffer.push_back({1, 1.1, 'a'});
        buffer.push_back({2, 2.2, 'b'});
        buffer.push_back({3, 3.3, 'c'});

        auto array_field_1 = buffer.to_array<int>(offsetof(test_type, field_1));
        ASSERT_EQ(array_field_1.size(), buffer.size());
        ASSERT_EQ(array_field_1.at(0), 1u);
        ASSERT_EQ(array_field_1.at(1), 2u);
        ASSERT_EQ(array_field_1.at(2), 3u);

        auto array_field_2 = buffer.to_array<double>(offsetof(test_type, field_2));
        ASSERT_DOUBLE_EQ(array_field_2.size(), buffer.size());
        ASSERT_DOUBLE_EQ(array_field_2.at(0), 1.1);
        ASSERT_DOUBLE_EQ(array_field_2.at(1), 2.2);
        ASSERT_DOUBLE_EQ(array_field_2.at(2), 3.3);

        auto array_field_3 = buffer.to_array<char>(offsetof(test_type, field_3));
        ASSERT_EQ(array_field_3.size(), buffer.size());
        ASSERT_EQ(array_field_3.at(0), 'a');
        ASSERT_EQ(array_field_3.at(1), 'b');
        ASSERT_EQ(array_field_3.at(2), 'c');
    }

    // consume all
    {
        while (!buffer.empty())
            buffer.pop_front();

        auto array = buffer.to_array<int>();
        ASSERT_EQ(array.size(), 0u);
    }
}
