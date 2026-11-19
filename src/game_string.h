#pragma once

#include <cstring>
#include <iterator>
#include "../sdk/GAME/GameString.hpp"
#include "../sdk/GAME/GameWideString.hpp"

namespace GAME {
template<class String>
auto string_data(String& value) -> decltype(&value.inlineBuffer[0])
{
    if (value.capacity < std::size(value.inlineBuffer)) return value.inlineBuffer;
    decltype(&value.inlineBuffer[0]) heap;
    std::memcpy(&heap, value.inlineBuffer, sizeof(heap));
    return heap;
}

// The caller keeps the null-terminated text alive for the native call.
inline GameString borrowed_string(const char* text, uint32_t size)
{
    GameString value{};
    value.size = size;
    if (size < sizeof(value.inlineBuffer))
    {
        value.capacity = sizeof(value.inlineBuffer) - 1;
        std::memcpy(value.inlineBuffer, text, size);
    }
    else
    {
        value.capacity = size;
        std::memcpy(value.inlineBuffer, &text, sizeof(text));
    }
    return value;
}
}
