#ifndef PARTIAL_VECTOR__PARTIAL_VECTOR_H
#define PARTIAL_VECTOR__PARTIAL_VECTOR_H

#include <cmath>
#include <cstdint>
#include <stdexcept>
#include <vector>

#define PARTIAL_VECTOR_PART_MAX_BYTE_SIZE 16384

// Minimum 2 elements per part
template<typename ElementT, typename = typename std::enable_if<(sizeof(ElementT) <= PARTIAL_VECTOR_PART_MAX_BYTE_SIZE / 2), ElementT>::type>
class partial_vector {
private:
    const uint32_t max_part_size = PARTIAL_VECTOR_PART_MAX_BYTE_SIZE / sizeof(ElementT);

    std::vector<std::vector<ElementT>> parts;

    size_t   size       = 0;
    uint32_t part_count = 0;

    mutable std::vector<size_t> part_offsets;

    struct ElementInfo {
        uint32_t part_index;     // index from parts
        uint32_t element_offset; // offset relative to part
    };

    ElementInfo find_element(size_t element_index) const {
        uint32_t estimate_part_index  = part_offsets.size() == 0 ? 0 : std::min(part_offsets.size() - 1, element_index / max_part_size);
        size_t   estimate_part_offset = estimate_part_index == 0 ? 0 : part_offsets[estimate_part_index];

        if (estimate_part_offset > element_index) {
            for (int i = static_cast<int>(estimate_part_index) - 1; i >= 0; i--) {
                auto& part = parts[i];
                estimate_part_offset -= part.size();

                if (element_index >= estimate_part_offset && element_index < estimate_part_offset + part.size())
                    return ElementInfo {
                        .part_index     = static_cast<uint32_t>(i),
                        .element_offset = static_cast<uint32_t>(element_index - estimate_part_offset),
                    };
            }
        } else { // estimate_part_offset <= element_index
            for (uint32_t i = estimate_part_index; i < part_count; i++) {
                auto& part = parts[i];

                // Save new part offset
                if (i >= part_offsets.size()) part_offsets.push_back(estimate_part_offset);

                if (element_index >= estimate_part_offset && element_index < estimate_part_offset + part.size())
                    return ElementInfo {
                        .part_index     = static_cast<uint32_t>(i),
                        .element_offset = static_cast<uint32_t>(element_index - estimate_part_offset),
                    };

                estimate_part_offset += part.size();
            }
        }

        return ElementInfo { .part_index = UINT32_MAX, .element_offset = UINT32_MAX };
    }

    ElementInfo next_element(ElementInfo const& current_element) const {
        if (current_element.element_offset + 1 < parts[current_element.part_index].size())
            return ElementInfo { .part_index = current_element.part_index, .element_offset = current_element.element_offset + 1 };
        else
            return ElementInfo { .part_index = current_element.part_index + 1, .element_offset = 0 };
    }

    ElementInfo previous_element(ElementInfo const& current_element) const {
        if (current_element.element_offset > 0)
            return ElementInfo { .part_index = current_element.part_index, .element_offset = current_element.element_offset - 1 };
        else
            return ElementInfo { .part_index     = current_element.part_index - 1,
                                 .element_offset = static_cast<uint32_t>(parts[current_element.part_index - 1].size() - 1) };
    }

public:
    struct iterator {
    private:
        partial_vector<ElementT>* p_vector;
        ElementInfo               elem_info;
        uint64_t                  elem_index;

        iterator(partial_vector<ElementT>& p_vector, ElementInfo elem_info, uint64_t elem_index) noexcept
            : p_vector(&p_vector), elem_info(elem_info), elem_index(elem_index) {}

        friend partial_vector;

    public:
        typedef iterator                        self_type;
        typedef ElementT                        value_type;
        typedef ElementT&                       reference;
        typedef ElementT*                       pointer;
        typedef std::random_access_iterator_tag iterator_category;
        typedef ptrdiff_t                       difference_type;

        ElementT& operator*() noexcept {
            return p_vector->parts[elem_info.part_index][elem_info.element_offset];
        }

        iterator operator+(uint64_t n) const noexcept {
            iterator tmp = *this;
            tmp.elem_index += n;
            tmp.elem_info = p_vector->find_element(tmp.elem_index);
            return tmp;
        }

        iterator& operator++() noexcept {
            elem_info = p_vector->next_element(elem_info);
            elem_index++;
            return *this;
        }

        iterator operator++(int) noexcept {
            iterator tmp = *this;
            elem_info    = p_vector->next_element(elem_info);
            elem_index++;
            return tmp;
        }

        iterator operator-(uint64_t n) const noexcept {
            iterator tmp = *this;
            tmp.elem_index -= n;
            tmp.elem_info = p_vector->find_element(tmp.elem_index);
            return tmp;
        }

        iterator& operator--() noexcept {
            elem_info = p_vector->previous_element(elem_info);
            elem_index--;
            return *this;
        }

        iterator operator--(int) noexcept {
            iterator tmp = *this;
            elem_info    = p_vector->previous_element(elem_info);
            elem_index--;
            return tmp;
        }

        friend ptrdiff_t operator-(iterator const& a, iterator const& b) noexcept {
            return a.elem_index - b.elem_index;
        }

        friend bool operator<(iterator const& a, iterator const& b) noexcept {
            return a.elem_index < b.elem_index;
        }

        friend bool operator>(iterator const& a, iterator const& b) noexcept {
            return a.elem_index > b.elem_index;
        }

        friend bool operator==(iterator const& a, iterator const& b) noexcept {
            return a.elem_index == b.elem_index;
        }

        friend bool operator!=(iterator const& a, iterator const& b) noexcept {
            return a.elem_index != b.elem_index;
        }
    };
    struct const_iterator {
    private:
        const partial_vector<ElementT>* p_vector;
        ElementInfo                     elem_info;
        uint64_t                        elem_index;

        const_iterator(partial_vector<ElementT> const& p_vector, ElementInfo elem_info, uint64_t elem_index) noexcept
            : p_vector(&p_vector), elem_info(elem_info), elem_index(elem_index) {}

        friend partial_vector;

    public:
        typedef const_iterator                  self_type;
        typedef ElementT                        value_type;
        typedef ElementT&                       reference;
        typedef ElementT*                       pointer;
        typedef std::random_access_iterator_tag iterator_category;
        typedef ptrdiff_t                       difference_type;

        ElementT const& operator*() const noexcept {
            return p_vector->parts[elem_info.part_index][elem_info.element_offset];
        }

        const_iterator operator+(uint64_t n) const noexcept {
            const_iterator tmp = *this;
            tmp.elem_index += n;
            tmp.elem_info = p_vector->find_element(tmp.elem_index);
            return tmp;
        }

        const_iterator& operator++() noexcept {
            elem_info = p_vector->next_element(elem_info);
            elem_index++;
            return *this;
        }

        const_iterator operator++(int) noexcept {
            const_iterator tmp = *this;
            elem_info          = p_vector->next_element(elem_info);
            elem_index++;
            return tmp;
        }

        const_iterator operator-(uint64_t n) const noexcept {
            const_iterator tmp = *this;
            tmp.elem_index -= n;
            tmp.elem_info = p_vector->find_element(tmp.elem_index);
            return tmp;
        }

        const_iterator& operator--() noexcept {
            elem_info = p_vector->previous_element(elem_info);
            elem_index--;
            return *this;
        }

        const_iterator operator--(int) noexcept {
            const_iterator tmp = *this;
            elem_info          = p_vector->previous_element(elem_info);
            elem_index--;
            return tmp;
        }

        friend ptrdiff_t operator-(iterator const& a, iterator const& b) noexcept {
            return a.elem_index - b.elem_index;
        }

        friend bool operator==(const_iterator const& a, const_iterator const& b) noexcept {
            return a.elem_index == b.elem_index;
        }

        friend bool operator!=(const_iterator const& a, const_iterator const& b) noexcept {
            return a.elem_index != b.elem_index;
        }
    };

    explicit partial_vector(partial_vector<ElementT> const& another) noexcept
        : parts(another.parts), part_offsets(another.part_offsets), part_count(another.part_count), size(another.size) {}

    explicit partial_vector(size_t size = 0) {
        resize(size);
    }

    explicit partial_vector(std::vector<ElementT> const& vector) {
        reserve(vector.size());

        // push_back is faster than []
        for (auto const& element : vector)
            push_back(element);
    }

    partial_vector(size_t size, ElementT init_value) {
        reserve(size);

        // push_back is faster than []
        for (size_t i = 0; i < size; i++)
            push_back(init_value);
    }

    template<typename IteratorT, typename = std::_RequireInputIter<IteratorT>>
    partial_vector(IteratorT first, IteratorT last) {
        reserve(last - first);

        for (IteratorT iter = first; iter < last; iter++)
            push_back(*iter);
    }

    void reserve(size_t r_size) noexcept {
        uint32_t parts_to_reserve = std::ceil(static_cast<double>(r_size) / max_part_size);
        // parts.reserve(parts_to_reserve);
        parts.resize(parts_to_reserve);
        part_offsets.reserve(parts_to_reserve);

        for (auto& part : parts)
            part.reserve(max_part_size);
    }

    void shrink_to_fit() noexcept {
        for (auto& part : parts)
            part.shrink_to_fit();

        parts.shrink_to_fit();
        part_offsets.shrink_to_fit();
    }

    void resize(size_t new_size) {
        if (this->size == new_size) return;

        if (new_size == 0) {
            part_count = 0;
            parts.resize(part_count);
        } else if (this->size == 0) {
            uint32_t parts_to_alloc         = new_size / max_part_size;
            uint32_t n_last_part_alloc_size = new_size - parts_to_alloc * max_part_size;

            part_count = part_count + parts_to_alloc + (n_last_part_alloc_size == 0 ? 0 : 1);
            parts.resize(part_count);

            for (uint32_t i = 0; i < parts_to_alloc; i++)
                parts[i].resize(max_part_size);

            if (n_last_part_alloc_size > 0) parts[part_count - 1].resize(n_last_part_alloc_size);
        } else if (new_size > this->size) {
            size_t size_to_alloc = new_size - this->size;

            uint32_t last_part_size = parts[part_count - 1].size();
            if (last_part_size < max_part_size) {
                uint32_t alloc_size = std::min(static_cast<size_t>(max_part_size - last_part_size), size_to_alloc);
                parts[part_count - 1].resize(last_part_size + alloc_size);

                size_to_alloc -= alloc_size;
            }

            uint32_t i = part_count - 1;
            while (size_to_alloc > 0) {
                uint32_t alloc_size = std::min(size_to_alloc, static_cast<size_t>(max_part_size));

                parts.push_back(std::vector<ElementT>(alloc_size));
                part_count++;

                size_to_alloc -= alloc_size;
            }
        } else { // new_size < this->size
            size_t size_to_remove = this->size - new_size;

            for (int i = part_count - 1; i >= 0; i--) {
                uint32_t part_size = parts[i].size();

                if (part_size <= size_to_remove) {
                    size_to_remove -= part_size;
                    parts.pop_back();
                    part_count--;
                    if (part_size == size_to_remove) break;
                } else { // part_size > size_to_remove
                    parts[i].resize(part_size - size_to_remove);
                    break;
                }
            }
        }

        part_offsets.reserve(new_size / max_part_size + 1);
        part_offsets.resize(std::min(part_offsets.size(), static_cast<size_t>(part_count)));

        this->size = new_size;
    }

    void clear() {
        resize(0);
    }

    void insert(iterator const& position, ElementT element) {
        // if (Index > size) throw std::runtime_error("Index >= size + 1");

        if (position.elem_index == size) {
            parts.push_back(std::vector<ElementT>({ element }));
            part_count++;
        } else { // Index < size
            ElementInfo const& elem_info = position.elem_info;
            auto&              part      = parts[elem_info.part_index];

            if (part.size() == max_part_size) {
                uint32_t part_n_index = elem_info.part_index + 1;

                if (part_n_index < part_count && parts[part_n_index].size() < max_part_size) {
                    parts[part_n_index].insert(parts[part_n_index].begin(), part[part.size() - 1]);
                } else {
                    parts.insert(parts.begin() + elem_info.part_index + 1,
                                 std::vector<ElementT>(part.begin() + part.size() - 1, part.end()));
                    part_count++;
                }

                // Here 'part' may be undefined because of 'parts.insert' in 'else' branch

                auto& part_t = parts[elem_info.part_index];
                part_t.erase(part_t.begin() + part_t.size() - 1);
            }

            auto& part_t = parts[elem_info.part_index];
            part_t.insert(part_t.begin() + elem_info.element_offset, element);

            part_offsets.resize(elem_info.part_index + 1);
        }

        size++;
    }

    void remove(size_t index) {
        if (index >= size) throw std::runtime_error("Index >= size");

        ElementInfo elem_info = find_element(index);
        auto&       part      = parts[elem_info.part_index];

        part.erase(part.begin() + elem_info.element_offset);
        if (part.empty()) parts.erase(parts.begin() + elem_info.part_index);

        part_offsets.resize(elem_info.part_index + 1);
        size--;
    }

    // Adding element via push_back is faster than [] operator
    void push_back(ElementT element) {
        if (size == 0) {
            part_count = 1;
            parts.resize(part_count);
            parts[0].push_back(element);
        } else {
            auto& part = parts[part_count - 1];

            if (part.size() < max_part_size) {
                part.push_back(element);
            } else {
                part_count = part_count + 1;
                parts.resize(part_count);
                parts[part_count - 1].push_back(element);
            }
        }

        size++;
    }

    ElementT& operator[](size_t index) {
        if (index >= size) throw std::runtime_error("Index >= size");

        ElementInfo elem_info = find_element(index);
        return parts[elem_info.part_index][elem_info.element_offset];
    }

    ElementT const& operator[](size_t index) const {
        if (index >= size) throw std::runtime_error("Index >= size");

        ElementInfo elem_info = find_element(index);
        return parts[elem_info.part_index][elem_info.element_offset];
    }

    void get_contiguous_data(void* output, size_t start_index, size_t count) const {
        if (start_index >= size) throw std::runtime_error("StartIndex >= size");
        count = std::min(count, size - start_index);

        auto        element_output  = static_cast<ElementT*>(output);
        size_t      size_to_read    = count;
        ElementInfo start_elem_info = find_element(start_index);

        for (uint32_t i = start_elem_info.part_index; i < part_count; i++) {
            auto&    part       = parts[i];
            uint32_t elem_index = i == start_elem_info.part_index ? start_elem_info.element_offset : 0;
            uint32_t elem_count = std::min(part.size() - elem_index, size_to_read);

            std::copy(part.begin() + elem_index, part.begin() + elem_index + elem_count, element_output);
            element_output += elem_count;

            size_to_read -= elem_count;
        }
    }

    void get_contiguous_data(void* output) const {
        get_contiguous_data(output, 0, SIZE_MAX);
    }

    std::vector<ElementT> to_vector(size_t start_index, size_t count = SIZE_MAX) const {
        if (start_index >= size) throw std::runtime_error("StartIndex >= size");
        count = std::min(count, size - start_index);

        std::vector<ElementT> data;
        data.resize(count);

        get_contiguous_data(data.data(), start_index, count);

        return data;
    }

    std::vector<ElementT> to_vector() const {
        return to_vector(0, SIZE_MAX);
    }

    size_t get_size() const noexcept {
        return size;
    }

    uint32_t get_part_count() const noexcept {
        return part_count;
    }

    iterator begin() noexcept {
        return iterator(*this, ElementInfo { 0, 0 }, 0);
    }

    const_iterator begin() const noexcept {
        return const_iterator(*this, ElementInfo { 0, 0 }, 0);
    }

    iterator end() noexcept {
        return iterator(*this,
                        ElementInfo {
                            .part_index     = part_count - 1,
                            .element_offset = static_cast<uint32_t>(parts[part_count - 1].size()),
                        },
                        size);
    }

    const_iterator end() const noexcept {
        return const_iterator(*this,
                              ElementInfo {
                                  .part_index     = part_count - 1,
                                  .element_offset = static_cast<uint32_t>(parts[part_count - 1].size()),
                              },
                              size);
    }
};

#endif // PARTIAL_VECTOR__PARTIAL_VECTOR_H