#pragma once

#include <boost/circular_buffer.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>

#include <list>
#include <memory>
#include <stdexcept>
#include <type_traits>

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


namespace pybind11 { namespace detail {

inline allocated_block::allocated_block(size_t capacity)
        :
            shm_key_(generate_shm_key()),
            capacity_(capacity)
{
    auto fd = ::shm_open(shm_key_.c_str(), O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd == FAILURE)
    {
        std::stringstream ss;
        ss << "shm_open: error(" << errno << ") " << ::strerror(errno);
        throw std::invalid_argument(ss.str());
    }

    auto result = ::ftruncate(fd, capacity_);
    if (result == FAILURE)
    {
        std::stringstream ss;
        ss << "ftruncate: error(" << errno << ") " << ::strerror(errno);
        throw std::invalid_argument(ss.str());
    }

    // first mapping uses 2 * capacity to establish space also for the second mapping
    // as use of MAP_FIXED requires use of address which is valid. Otherwise on rare occasions
    // second mapping could be established in boundries which aren't valid
    auto map = ::mmap(NULL, 2ul * capacity_, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (map == MAP_FAILED)
    {
        std::stringstream ss;
        ss << "mmap: error(" << errno << ") " << ::strerror(errno);
        throw std::invalid_argument(ss.str());
    }

    first_map_ = reinterpret_cast<uint8_t*>(map);

    auto expected_second_map = first_map_ + capacity_;
    map = ::mmap(expected_second_map, capacity_, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_FIXED, fd, 0);
    if (map == MAP_FAILED)
    {
        std::stringstream ss;
        ss << "mmap: error(" << errno << ") " << ::strerror(errno);
        throw std::invalid_argument(ss.str());
    }
    else if (map != expected_second_map)
    {
        std::stringstream ss;
        ss << "mmap: requested mapped block 0x" << std::hex << expected_second_map << " but is 0x" << std::hex << map;
        throw std::invalid_argument(ss.str());
    }

    second_map_ = reinterpret_cast<uint8_t*>(map);
}

inline allocated_block::~allocated_block()
{
    if (second_map_ != nullptr)
    {
        ::munmap(second_map_, capacity_);
        second_map_ = nullptr;
    }

    if (first_map_ != nullptr)
    {
        ::munmap(first_map_, capacity_);
        first_map_ = nullptr;
    }

    ::shm_unlink(shm_key_.c_str());
}

template <typename pointer_type>
inline pointer_type allocated_block::pointer()
{
    return reinterpret_cast<pointer_type>(first_map_);
}

std::string allocated_block::generate_shm_key()
{
    static boost::uuids::random_generator generator;
    auto id = generator();

    return boost::lexical_cast<std::string>(id);
}

template <typename element_type>
inline typename allocator<element_type>::pointer_type allocator<element_type>::allocate(size_t n)
{
    allocated_blocks_.emplace_back(sizeof(element_type) * n);
    auto& emplaced = allocated_blocks_.back();

    return emplaced.template pointer<pointer_type>();
}

template <typename element_type>
void allocator<element_type>::deallocate(allocator::pointer_type p, size_t n)
{
    allocated_blocks_.remove_if([&](allocated_block& tested) { return tested.template pointer<pointer_type>() == p; });
}

template <typename T>
inline numpy_circular_buffer<T>::numpy_circular_buffer(size_t capacity)
    :
        base_circular_buffer(round_to_page_size(capacity))
{

}

template <typename T>
inline size_t numpy_circular_buffer<T>::round_to_page_size(size_t capacity)
{
    auto page_size = getpagesize();
    if (page_size % sizeof(T) != 0)
        throw std::runtime_error("Elements cannot fit into pages. Please align their size!");

    auto elements_per_page = page_size / sizeof(T);
    return capacity * elements_per_page == 0
           ? capacity
           : (capacity / elements_per_page + 1) * elements_per_page;
}

template <typename T>
template <typename V>
inline pybind11::array_t<V> numpy_circular_buffer<T>::to_array(ptrdiff_t offset)
{
    static_assert(std::is_scalar<V>::value, "scalar type is expected as array element!");

    static constexpr auto numpy_module = "numpy";
    static constexpr auto array_attribute = "array";
    static constexpr auto copy_arg = "copy";

    auto numpy = pybind11::module::import(numpy_module);
    auto array = numpy.attr(array_attribute).cast<pybind11::function>();

    auto first_element = base_circular_buffer::array_one();
    pybind11::buffer_info buffer(
            reinterpret_cast<uint8_t*>(first_element.first) + offset,   /* Pointer to buffer */
            sizeof(V),                                                  /* Size of one scalar */
            pybind11::format_descriptor<V>::format(),                   /* Python struct-style format descriptor */
            1,                                                          /* Number of dimensions */
            {base_circular_buffer::size()},                             /* Buffer dimensions */
            {sizeof(T)});                                               /* Number of entries between adjacent entries (for each per dimension) */

    pybind11::readonly_memoryview view(buffer);
    auto result = array(view, pybind11::arg(copy_arg) = false);

    return result;
}

} }
