import _optional_test as fixtures
from hamcrest import *


def test_optional_float():
    has_value, _ = fixtures.with_optional_float(None)
    assert_that(has_value, is_(False))

    has_value, value = fixtures.with_optional_float(10)
    assert_that(has_value, is_(True))
    assert_that(value, is_(10))


def test_optional_str():
    has_value, _ = fixtures.with_optional_str(None)
    assert_that(has_value, is_(False))

    has_value, value = fixtures.with_optional_str("abc")
    assert_that(has_value, is_(True))
    assert_that(value, is_("abc"))


def test_optional_list():
    has_value, _ = fixtures.with_optional_list(None)
    assert_that(has_value, is_(False))

    has_value, value = fixtures.with_optional_list([1, 2, 3])
    assert_that(has_value, is_(True))
    assert_that(value, has_items(1, 2, 3))