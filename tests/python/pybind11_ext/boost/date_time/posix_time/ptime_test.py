import _ptime_test as fixtures
from datetime import datetime
from hamcrest import *


def test_none_datetime():
    fixtures.compare_ptime(None, "")


def test_not_none_datetime():
    fixtures.compare_ptime(datetime(2018, 1, 1), "2018-01-01 00:00:00")
    fixtures.compare_ptime(datetime(2018, 1, 1, 15, 42), "2018-01-01 15:42:00")
    fixtures.compare_ptime(datetime(2018, 1, 1, 15, 42, 20, 567), "2018-01-01 15:42:20.000567")