#include <pybind11_ext/boost/optional.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <boost/optional.hpp>
#include <pybind11/embed.h>

#include <string>

using namespace std;
using namespace testing;
namespace py = pybind11;

namespace boost_optional {

template <typename T, bool has_value_t, T value_t = 0>
struct test_fixture
{
    using type = boost::optional<T>;
    using optional_type = boost::optional<T>;

    static const bool is_none = !has_value_t;
    static const T value = value_t;

    static boost::optional<T> get_value()
    {
        if (is_none)
            return boost::none;

        return T(value);
    }

};

static const char empty[] = "";

template <bool has_value_t, const char* value_t = empty>
struct test_fixture__string
{
    using type = boost::optional<string>;
    using optional_type = boost::optional<string>;

    static const bool is_none = !has_value_t;

    static boost::optional<string> get_value()
    {
        if (is_none)
            return boost::none;

        return string(value_t);
    }

};

static const double zero = 0.0;

template <bool has_value_t, const double& value_t = zero>
struct test_fixture__double
{
    using type = boost::optional<double>;
    using optional_type = boost::optional<double>;

    static const bool is_none = !has_value_t;

    static boost::optional<double> get_value()
    {
        if (is_none)
            return boost::none;

        return value_t;
    }

};

}

template <typename T>
class boost_optional_test : public Test
{ };

TYPED_TEST_CASE_P(boost_optional_test);

TYPED_TEST_P(boost_optional_test, cast_load)
{
    py::scoped_interpreter interpreter;

    auto original_value = TypeParam::get_value();
    auto is_none = TypeParam::is_none;

    // convert CXX -> Python
    py::object py_value = py::cast(original_value);
    ASSERT_EQ(py_value.is_none(), is_none);

    // convert Python -> CXX
    auto restored_value = py::cast<typename TypeParam::optional_type>(py_value);
    if (is_none)
    {
        ASSERT_FALSE(restored_value);
    }
    else
    {
        ASSERT_TRUE(restored_value);
        ASSERT_EQ(*restored_value, *original_value);
    }
}

REGISTER_TYPED_TEST_CASE_P(boost_optional_test, cast_load);

static const char test_string[] = "abc";
static const double test_double = 78.96;
using test_fixtures = Types<
        boost_optional::test_fixture<int, true, 5>,
        boost_optional::test_fixture<int, false>,
        boost_optional::test_fixture<unsigned int, true, 5>,
        boost_optional::test_fixture<unsigned int, false>,
        boost_optional::test_fixture<long long, true, 5>,
        boost_optional::test_fixture<long long, false>,
        boost_optional::test_fixture<unsigned long long, true, 5>,
        boost_optional::test_fixture<unsigned long long, false>,
        boost_optional::test_fixture__string<true, test_string>,
        boost_optional::test_fixture__string<false>,
        boost_optional::test_fixture__double<true, test_double>,
        boost_optional::test_fixture__double<false>>;
INSTANTIATE_TYPED_TEST_CASE_P(basic, boost_optional_test, test_fixtures);

