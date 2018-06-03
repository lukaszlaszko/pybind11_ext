import _time_duration_test as fixtures
from datetime import timedelta
from hamcrest import *


def test_none_timedelta():
    fixtures.compare_duration(None, "")


def test_not_none_timedelta():
    fixtures.compare_duration(timedelta(days=2), "48:00:00")
    fixtures.compare_duration(timedelta(hours=7), "07:00:00")
    fixtures.compare_duration(timedelta(minutes=56), "00:56:00")
    fixtures.compare_duration(timedelta(seconds=43), "00:00:43")
    fixtures.compare_duration(timedelta(microseconds=567), "00:00:00.000567")