#ifndef CHUNK_ARRAY_H
#define CHUNK_ARRAY_H

#include <cmath>
#include <cstddef>
#include <vector>

const unsigned int default_half_size = 12;

template <typename T, T(factory)(int x, int y) = T::T>
class chunk_array {
   private:
    std::vector<std::vector<T>> data;

   public:
    class chunk_array_it {
       public:
        using value_type = decltype(data);
        static chunk_array_it begin(chunk_array* c) {
            value_type* vv = &(c->data);
            auto outer = vv->begin();
            return chunk_array_it{outer, outer->begin(), vv};
        }
        static chunk_array_it end(chunk_array* c) {
            value_type* vv = &(c->data);
            return chunk_array_it{vv->end(), {}, vv};
        }
        chunk_array_it operator++(int) {  // postfix++
            chunk_array_it ret = *this;
            ++(*this);
            return ret;
        }
        chunk_array_it& operator++() {  // ++prefix
            inner++;
            while (outer != vv->end() && inner == outer->end()) {
                outer++;
                if (outer != vv->end())
                    inner = outer->begin();
            }
            return *this;
        }

        friend bool operator==(const chunk_array_it& lhs,
                               const chunk_array_it& rhs) {
            if (lhs.outer == lhs.vv->end()) return lhs.outer == rhs.outer;
            return (lhs.outer == rhs.outer) && (lhs.inner == rhs.inner);
        }
        friend bool operator!=(const chunk_array_it& lhs,
                               const chunk_array_it& rhs) {
            return !(lhs == rhs);
        }

        typename value_type::value_type::value_type& operator*() {
            return *inner;
        }

       private:
        typename value_type::iterator outer;
        typename value_type::value_type::iterator inner;
        value_type* vv;
        chunk_array_it(chunk_array_it&) = default;
        chunk_array_it(typename value_type::iterator outer,
                       typename value_type::value_type::iterator inner,
                       value_type* vv)
            : outer(outer), inner(inner), vv(vv) {}
    };

    using iterator = chunk_array_it;
    using value_type = T;

    chunk_array(unsigned int half_size = default_half_size)
        : half_size(half_size) {
        resize_emplace();
    }

    [[nodiscard]] unsigned int size() const { return 2 * half_size + 1; }

    void move_current_position(int x_rel, int y_rel) {
        current_position.x += x_rel;
        current_position.y += y_rel;
        if (static_cast<unsigned int>(std::abs(x_rel)) >= size() ||
            static_cast<unsigned int>(std::abs(y_rel)) >= size()) {
            resize_emplace();
        }

        int x_min = -static_cast<int>(half_size);
        int x_max = x_rel - static_cast<int>(half_size);
        if (x_rel < 0) {
            x_min = static_cast<int>(half_size) + x_rel + 1;
            x_max = static_cast<int>(half_size) + 1;
        }
        for (int i = x_min; i < x_max; i++) {
            for (int j = 0; j < static_cast<int>(size()); j++) {
                auto [i2, j2] =
                    get_indices_from_id(current_position.x - x_rel + i,
                                        current_position.y - y_rel + j);
                auto [x, y] = get_id_from_indices(i2, j2);

                // TODO: make sure the old chunk deallocate its content
                data[i2][j2] = T{x, y};
            }
        }

        int y_min = -static_cast<int>(half_size);
        int y_max = y_rel - static_cast<int>(half_size);
        if (y_rel < 0) {
            y_min = static_cast<int>(half_size) + y_rel + 1;
            y_max = static_cast<int>(half_size) + 1;
        }
        for (int i = y_min; i < y_max; i++) {
            for (int j = 0; j < static_cast<int>(size()); j++) {
                auto [i2, j2] =
                    get_indices_from_id(current_position.x - x_rel + i,
                                        current_position.y - y_rel + j);
                auto [x, y] = get_id_from_indices(i2, j2);
                // TODO: make sure the old chunk deallocate its content
                data[i2][j2] = factory(x, y);
            }
        }
    }

    void set_current_position(int abs_x, int abs_y) {
        move_current_position(abs_x - current_position.x,
                              abs_y - current_position.y);
    }

    iterator begin() { return chunk_array_it::begin(this); }

    iterator end() { return chunk_array_it::end(this); }

   private:
    [[nodiscard]] std::pair<int, int> get_id_from_indices(std::size_t i,
                                                          std::size_t j) const {
        auto [current_i, current_j] =
            get_indices_from_id(current_position.x, current_position.y);
        int dx = (i - current_i) % size(),
            dy = (j - current_j) % size();

        if (dx > static_cast<int>(half_size)) dx -= size();
        if (dy > static_cast<int>(half_size)) dy -= size();

        return {current_position.x + dx, current_position.y + dy};
    }

    [[nodiscard]] std::pair<std::size_t, std::size_t> get_indices_from_id(
        int x, int y) const {
        return {x % size(), y % size()};
    }

    void resize_emplace() {
        data.clear();
        data.reserve(size());

        for (std::size_t i = 0; i < size(); i++) {
            data.emplace_back();
            data[i].reserve(size());
            for (std::size_t j = 0; j < size(); j++) {
                auto [x, y] = get_id_from_indices(i, j);
                data[i].emplace_back(factory(x, y));  // Will use move ctor
            }
        }
    }

    unsigned int half_size;
    struct {
        int x, y;
    } current_position = {};
};

#endif  // !CHUNK_ARRAY_H
