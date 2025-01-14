#pragma once

#include <cassert>
#include <cstring> // For memcpy
#include <span>

#include "Vulkan/GPUBuffer.h"

template <typename T>
class DynamicBuffer
{
  public:
    /// Appends a range of elements to the buffer.
    void appendRange(std::span<const T> elements)
    {
        assert((currentSize + elements.size()) <= maxCapacity && "Buffer overflow detected.");

        auto mappedData = (T *)(gpuBuffer.info.pMappedData);
        std::memcpy((void *)&mappedData[currentSize], elements.data(), elements.size() * sizeof(T));

        currentSize += elements.size();
    }

    /// Appends a single element to the buffer.
    void appendElement(const T &element)
    {
        assert((currentSize + 1) <= maxCapacity && "Buffer overflow detected.");

        auto *mappedData = static_cast<T *>(gpuBuffer.info.pMappedData);
        std::memcpy(&mappedData[currentSize], &element, sizeof(T));

        ++currentSize;
    }

    /// Resets the buffer to an empty state.
    void reset() { currentSize = 0; }

    /// Returns the underlying Vulkan buffer handle.
    VkBuffer getHandle() const { return gpuBuffer.buffer; }

  public:
    GPUBuffer   gpuBuffer;
    std::size_t maxCapacity{0};
    std::size_t currentSize{0};
};
