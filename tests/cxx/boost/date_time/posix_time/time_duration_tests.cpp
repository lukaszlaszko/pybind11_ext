#include <pybind11_ext/boost/date_time/posix_time/time_duration.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <pybind11/embed.h>
#include <pybind11/pybind11.h>

#include <limits>
#include <string>

using namespace std;
using namespace boost::posix_time;
using namespace testing;
namespace py = pybind11;


namespace boost_posix_time {

struct test_fixture
{
    time_duration duration;

    bool is_none() const
    {
        return duration.is_not_a_date_time();
    }
};

}

class boost_posix_time_time_duration_test : public TestWithParam<boost_posix_time::test_fixture>
{ };


TEST_P(boost_posix_time_time_duration_test, cast_load)
{
    py::scoped_interpreter interpreter;
    py::module::import("datetime");

    auto& fixture = GetParam();
    auto original = fixture.duration;

    // conversion CXX -> Python
    py::object py_duration = py::cast(original);
    ASSERT_EQ(py_duration.is_none(), fixture.is_none());

    // conversion Python -> CXX
    auto restored = py::cast<time_duration>(py_duration);
    if (fixture.is_none())
    {
        ASSERT_TRUE(restored.is_not_a_date_time());
    }
    else
    {
        ASSERT_FALSE(restored.is_not_a_date_time());
        ASSERT_EQ(restored, original);
    }
}

INSTANTIATE_TEST_CASE_P(basic, boost_posix_time_time_duration_test, Values(
        boost_posix_time::test_fixture { microsec(54) },
        boost_posix_time::test_fixture { seconds(10) },
        boost_posix_time::test_fixture { minutes(10) },
        boost_posix_time::test_fixture { hours(10) },
        boost_posix_time::test_fixture { hours(15) + minutes(4) + seconds(25) },
        boost_posix_time::test_fixture { not_a_date_time }));

INSTANTIATE_TEST_CASE_P(edges, boost_posix_time_time_duration_test, Values(
        boost_posix_time::test_fixture { seconds(0) }));
