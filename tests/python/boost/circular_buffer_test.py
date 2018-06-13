import _circular_buffer_test as fixtures
import numpy as np
from hamcrest import *


def test_no_rotation():
    buffer = fixtures.IntCircularBuffer(50)
    assert_that(buffer.capacity, is_(1024))

    buffer.push(1);
    buffer.push(2);
    buffer.push(3);

    array = buffer.array
    assert_that(len(array), is_(3))
    assert_that(array, has_items(1,2,3))

    assert_that(buffer.pop(), is_(1))

    array = buffer.array
    assert_that(len(array), is_(2))
    assert_that(array, has_items(2,3))


def test_rotation():
    buffer = fixtures.IntCircularBuffer(1)
    assert_that(buffer.capacity, is_(1024))

    for i in range(1026):
        buffer.push(i)

    array = buffer.array
    assert_that(len(array), is_(1024))
    assert_that(array[0], is_(2))
    assert_that(array[-1], is_(1025))


def test_custom_no_rotation():
    buffer = fixtures.CustomCircularBuffer(1)
    assert_that(buffer.capacity, is_(128))

    buffer.push(1, True, 0.1)
    buffer.push(2, True, 5.4)
    buffer.push(3, False, 0)

    index_array = buffer.index_array
    assert_that(len(index_array), is_(3))
    assert_that(index_array, has_items(1, 2, 3))

    has_value_array = buffer.has_value_array
    assert_that(len(has_value_array), is_(3))
    assert_that(has_value_array, has_items(True, True, False))

    value_array = buffer.value_array
    assert_that(len(value_array), is_(3))
    assert_that(value_array, has_items(0.1, 5.4, 0))

    buffer.pop()
    index_array = buffer.index_array
    assert_that(len(index_array), is_(2))


def test_custom_rotation():
    buffer = fixtures.CustomCircularBuffer(1)
    assert_that(buffer.capacity, is_(128))

    for i in range(1, 150):
        buffer.push(i, True, 1 / float(i))

    index_array = buffer.index_array
    assert_that(len(index_array), is_(128))
    assert_that(index_array[0], is_not(1))
    assert_that(index_array[-1], is_(149))



