#pragma once

#include <boost/circular_buffer.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>

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

inline std::string allocated_block::generate_shm_key()
{
    auto temp = boost::filesystem::unique_path();
    return temp.native();
}

template <typename element_type>
inline typename allocator<element_type>::pointer_type allocator<element_type>::allocate(size_t n)
{
    allocated_blocks_.emplace_back(sizeof(element_type) * n);
    auto& emplaced = allocated_blocks_.back();

    return emplaced.template pointer<pointer_type>();
}

template <typename element_type>
inline void allocator<element_type>::deallocate(allocator::pointer_type p, size_t n)
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

} }
