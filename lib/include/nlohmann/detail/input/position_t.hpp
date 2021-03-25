#pragma once

#include <cstddef> // size_t

namespace nlohmann
{
namespace detail
{
/// struct to capture the start position of the current token
struct position_t
{
    /// the total number of characters stream_read
    std::size_t chars_read_total = 0;
    /// the number of characters stream_read in the current line
    std::size_t chars_read_current_line = 0;
    /// the number of lines stream_read
    std::size_t lines_read = 0;

    /// conversion to size_t to preserve SAX interface
    constexpr operator size_t() const
    {
        return chars_read_total;
    }
};

} // namespace detail
} // namespace nlohmann
