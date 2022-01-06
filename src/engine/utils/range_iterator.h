#ifndef RANGE_ITERATOR_H_
#define RANGE_ITERATOR_H_

#include <array>
#include <concepts>
#include <type_traits>

#include "logging.h"

// https://stackoverflow.com/a/36934374
template<bool...> struct bool_pack;
template<bool... bs> 
using all_true = std::is_same<bool_pack<bs..., true>, bool_pack<true, bs...>>;

template<bool... bs> 
inline constexpr bool all_true_v = all_true<bs...>::value;


template <std::unsigned_integral T, T... Bounds>
class range_it {
   public:
    static constexpr std::size_t len = sizeof...(Bounds);
    static_assert(len >= 1);
    static_assert(all_true_v<(Bounds > 0)...>);

    using seq_container = std::array<T, len>;
    static constexpr seq_container bounds{Bounds...};

    range_it& begin() {
        auto l = log << "Range bounds: ";
        for (auto b : bounds) l <<  b << " ";
        return *this;
    }

    static range_it end() {
        range_it r;
        r.current_seq = bounds;
        return r;
    }

    range_it operator++(int) {  // postfix++
        range_it ret = *this;
        ++(*this);
        return ret;
    }

    range_it& operator++() {  // ++prefix
        std::size_t i = 0;

        // completed
        [[unlikely]] if (current_seq[0] == bounds[0]) return *this;

        for (; i < len; i++) {
            current_seq[i] += 1;

            [[likely]] if (current_seq[i] < bounds[i]) break;

            current_seq[i] = 0;
        }

        // That means the range has done a full turn
        if (i == len && current_seq[i - 1] == 0) current_seq = bounds;

        return *this;
    }

    seq_container operator*() { return current_seq; }

    bool operator==(const range_it& other) const {
        if (current_seq[0] == bounds[0]) return true;  // completed

        return current_seq == other.current_seq;
    }

    bool operator!=(const range_it& other) const { return !(*this == other); }

   private:
    seq_container current_seq{0};
    Logger log {Logger::get({"Range"})};
};

#endif  // !RANGE_ITERATOR_H_
