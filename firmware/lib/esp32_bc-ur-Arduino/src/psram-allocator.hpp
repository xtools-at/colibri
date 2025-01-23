#ifndef BC_UR_PSRAM_ALLOCATOR_HPP
#define BC_UR_PSRAM_ALLOCATOR_HPP

//
//  psram-allocator.hpp
//
//  Minimal std::allocator type that prefers SPI/PS-RAM if it is available.
//

#include <cstdlib>
#include <memory>

#include <esp_heap_caps.h>

template <typename T>
class PSRAMAllocator {
public:
    using value_type = T;

    PSRAMAllocator() noexcept = default;

    template <typename U>
    PSRAMAllocator(const PSRAMAllocator<U>& other) noexcept {};

    T* allocate(std::size_t n) {
      const size_t size = n * sizeof(T);
      return static_cast<T*>(heap_caps_malloc_prefer(size, MALLOC_CAP_DEFAULT | MALLOC_CAP_SPIRAM, MALLOC_CAP_DEFAULT));
    }

    void deallocate(T* ptr, std::size_t n) noexcept {
      std::free(ptr);
    }
};

template <typename T, typename U>
bool operator==(const PSRAMAllocator<T>&, const PSRAMAllocator<U>&) {
    return true;
}

template <typename T, typename U>
bool operator!=(const PSRAMAllocator<T>& lhs, const PSRAMAllocator<U>& rhs) {
    return !(lhs == rhs);
}

#endif