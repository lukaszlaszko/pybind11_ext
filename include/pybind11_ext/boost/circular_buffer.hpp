#pragma once

#include <pybind11_ext/readonly_memoryview.hpp>

#include <boost/circular_buffer.hpp>
#include <pybind11/numpy.h>

#include <list>
#include <string>


namespace pybind11 { namespace detail {

class allocated_block
{
public:
    allocated_block(size_t capacity);
    ~allocated_block();

    template <typename pointer_type>
    pointer_type pointer();

private:
    static constexpr auto FAILURE = -1;

    static std::string generate_shm_key();

    const std::string shm_key_;
    const size_t capacity_;
    uint8_t* first_map_ {nullptr};
    uint8_t* second_map_ {nullptr};

};

template <typename T>
class allocator
{
public:
    using value_type = T;
    using pointer_type = T*;

    pointer_type allocate(size_t n);
    void deallocate(pointer_type p, size_t n);

private:
    std::list<allocated_block> allocated_blocks_;

};

template <typename T>
class numpy_circular_buffer : public boost::circular_buffer<T, allocator<T>>
{
public:
    explicit numpy_circular_buffer(size_t capacity);

    template <typename V = T>
    pybind11::array_t<V> to_array(size_t offset = 0u);


private:
    using base_circular_buffer = boost::circular_buffer<T, allocator<T>>;

    size_t round_to_page_size(size_t capacity);

};

} }

#include "circular_buffer.ipp"
