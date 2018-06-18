#include <pybind11_ext/boost/date_time/posix_time/ptime.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <pybind11/embed.h>
#include <pybind11/pybind11.h>

#include <iostream>
#include <string>

using namespace std;
using namespace boost::posix_time;
using namespace testing;
namespace py = pybind11;


namespace boost_posix_time {

struct test_fixture
{
    ptime timepoint;

    bool is_none() const
    {
        return timepoint.is_not_a_date_time();
    }
};

}

class boost_posix_time_ptime_test : public TestWithParam<boost_posix_time::test_fixture>
{ };


TEST_P(boost_posix_time_ptime_test, cast_load)
{
    py::module::import("datetime");

    auto& fixture = GetParam();
    auto original = fixture.timepoint;

    // conversion CXX -> Python
    py::object py_timepoint = py::cast(original);
    ASSERT_EQ(py_timepoint.is_none(), fixture.is_none());

    // conversion Python -> CXX
    auto restored = py::cast<ptime>(py_timepoint);
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

INSTANTIATE_TEST_CASE_P(basic, boost_posix_time_ptime_test, Values(
        boost_posix_time::test_fixture { time_from_string("2018-01-01 15:43") },
        boost_posix_time::test_fixture { time_from_string("2018-01-01 15:43:22") },
        boost_posix_time::test_fixture { time_from_string("2018-01-01 15:43:22.678") },
        boost_posix_time::test_fixture { time_from_string("1960-01-01 15:43") },
        boost_posix_time::test_fixture { not_a_date_time }));
