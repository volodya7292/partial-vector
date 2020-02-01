#include <cassert>
#include <iostream>
#include <vector>

#include "partial_vector.h"

static void pv_unit_0(uint32_t size) {
    partial_vector<size_t> v(size, 7654321);
    partial_vector<size_t> v2(v);

    for (uint32_t i = 0; i < v.get_size(); i++)
        assert(v[i] == v2[i]);
};

static void pv_unit_1(uint32_t size) {
    partial_vector<size_t> v(size);
    std::vector<size_t>    v_stl(size);

    assert(v.get_size() == size);
    for (uint32_t i = 0; i < v.get_size(); i++)
        v_stl[i] = v[i];
}

static void pv_unit_2(uint32_t size) {
    std::vector<size_t> v_stl(size);
    for (uint32_t i = 0; i < v_stl.size(); i++)
        v_stl[i] = i;

    partial_vector<size_t> v(v_stl);
    assert(v.get_size() == v_stl.size());

    for (uint32_t i = 0; i < v.get_size(); i++)
        assert(v[i] == v_stl[i]);
}

static void pv_unit_3(uint32_t size) {
    partial_vector<size_t> v(size, 7654321);
    assert(v.get_size() == size);

    for (uint32_t i = 0; i < v.get_size(); i++)
        assert(v[i] == 7654321);
}

static void pv_unit_4(uint32_t size) {
    std::vector<size_t> v_stl(size);
    for (uint32_t i = 0; i < v_stl.size(); i++)
        v_stl[i] = i;

    partial_vector<size_t> v(v_stl.begin(), v_stl.end());
    assert(v.get_size() == v_stl.size());

    for (uint32_t i = 0; i < v.get_size(); i++)
        assert(v[i] == v_stl[i]);
}

static void pv_unit_5(uint32_t size) {
    partial_vector<size_t> v;
    v.reserve(size);

    for (uint32_t i = 0; i < size; i++)
        v.push_back(i);

    for (uint32_t i = 0; i < size; i++)
        assert(v[i] == i);
}

static void pv_unit_6(uint32_t size) {
    partial_vector<size_t> v;
    v.reserve(size * 2);

    for (uint32_t i = 0; i < size; i++)
        v.push_back(i);

    v.shrink_to_fit();
}

static void pv_unit_7(uint32_t size) {
    partial_vector<size_t> v;

    for (uint32_t i = 0; i < size; i++)
        v.push_back(i);

    v.resize(size / 2);
    v.resize(size * 2);

    std::vector<size_t> v_stl(4096);
    for (uint32_t i = 0; i < 4096; i++)
        v_stl[i] = 0;
}

static void pv_unit_8(uint32_t size) {
    partial_vector<size_t> v;

    for (uint32_t i = 0; i < size; i++)
        v.insert(v.begin() + i, i);

    for (uint32_t i = 0; i < size; i++)
        assert(v[i] == i);

    partial_vector<size_t> v2;

    for (uint32_t i = 0; i < size; i++)
        v2.insert(v.begin(), i);

    for (uint32_t i = 0; i < size; i++)
        assert(v2[i] == size - i - 1);
}

static void pv_unit_9(uint32_t size) {
    partial_vector<size_t> v;

    for (uint32_t i = 0; i < size; i++)
        v.push_back(i);

    for (uint32_t i = 0; i < size; i++)
        v.remove(0);

    assert(v.get_size() == 0);
}

static void pv_unit_10(uint32_t size) {
    partial_vector<size_t> v;

    for (uint32_t i = 0; i < size; i++)
        v.push_back(i);

    assert(v.get_size() == size);

    for (uint32_t i = 0; i < size; i++)
        assert(v[i] == i);
}

static void pv_unit_11(uint32_t size) {
    partial_vector<size_t> v(size);

    for (uint32_t i = 0; i < size; i++)
        v[i] = i;

    auto data0 = v.to_vector();
    for (uint32_t i = 0; i < size; i++)
        assert(data0[i] == i);

    auto data1 = v.to_vector(5);
    for (uint32_t i = 0; i < data1.size(); i++)
        assert(data1[i] == i + 5);

    auto data2 = v.to_vector(0, PARTIAL_VECTOR_PART_MAX_BYTE_SIZE / 8 + 10);
    for (uint32_t i = 0; i < data2.size(); i++)
        assert(data2[i] == i);

    auto data3 = v.to_vector(5, 10);
    for (uint32_t i = 0; i < data3.size(); i++)
        assert(data3[i] == i + 5);
}

static void partial_vector_unit_tests() {
    pv_unit_0(10);
    pv_unit_0(PARTIAL_VECTOR_PART_MAX_BYTE_SIZE / 8);
    pv_unit_0(PARTIAL_VECTOR_PART_MAX_BYTE_SIZE / 8 + 10);
    pv_unit_0(PARTIAL_VECTOR_PART_MAX_BYTE_SIZE / 8 * 10 + 10);

    pv_unit_1(10);
    pv_unit_1(PARTIAL_VECTOR_PART_MAX_BYTE_SIZE / 8);
    pv_unit_1(PARTIAL_VECTOR_PART_MAX_BYTE_SIZE / 8 + 10);
    pv_unit_1(PARTIAL_VECTOR_PART_MAX_BYTE_SIZE / 8 * 10 + 10);

    pv_unit_2(10);
    pv_unit_2(PARTIAL_VECTOR_PART_MAX_BYTE_SIZE / 8);
    pv_unit_2(PARTIAL_VECTOR_PART_MAX_BYTE_SIZE / 8 + 10);
    pv_unit_2(PARTIAL_VECTOR_PART_MAX_BYTE_SIZE / 8 * 10 + 10);

    pv_unit_3(10);
    pv_unit_3(PARTIAL_VECTOR_PART_MAX_BYTE_SIZE / 8);
    pv_unit_3(PARTIAL_VECTOR_PART_MAX_BYTE_SIZE / 8 + 10);
    pv_unit_3(PARTIAL_VECTOR_PART_MAX_BYTE_SIZE / 8 * 10 + 10);

    pv_unit_4(10);
    pv_unit_4(PARTIAL_VECTOR_PART_MAX_BYTE_SIZE / 8);
    pv_unit_4(PARTIAL_VECTOR_PART_MAX_BYTE_SIZE / 8 + 10);
    pv_unit_4(PARTIAL_VECTOR_PART_MAX_BYTE_SIZE / 8 * 10 + 10);

    pv_unit_5(10);
    pv_unit_5(PARTIAL_VECTOR_PART_MAX_BYTE_SIZE / 8);
    pv_unit_5(PARTIAL_VECTOR_PART_MAX_BYTE_SIZE / 8 + 10);
    pv_unit_5(PARTIAL_VECTOR_PART_MAX_BYTE_SIZE / 8 * 10 + 10);

    pv_unit_6(10);
    pv_unit_6(PARTIAL_VECTOR_PART_MAX_BYTE_SIZE / 8);
    pv_unit_6(PARTIAL_VECTOR_PART_MAX_BYTE_SIZE / 8 + 10);
    pv_unit_6(PARTIAL_VECTOR_PART_MAX_BYTE_SIZE / 8 * 10 + 10);

    pv_unit_7(10);
    pv_unit_7(PARTIAL_VECTOR_PART_MAX_BYTE_SIZE / 8);
    pv_unit_7(PARTIAL_VECTOR_PART_MAX_BYTE_SIZE / 8 + 10);
    pv_unit_7(PARTIAL_VECTOR_PART_MAX_BYTE_SIZE / 8 * 10 + 10);

    pv_unit_8(10);
    pv_unit_8(PARTIAL_VECTOR_PART_MAX_BYTE_SIZE / 8);
    pv_unit_8(PARTIAL_VECTOR_PART_MAX_BYTE_SIZE / 8 + 10);
    pv_unit_8(PARTIAL_VECTOR_PART_MAX_BYTE_SIZE / 8 * 10 + 10);

    pv_unit_9(10);
    pv_unit_9(PARTIAL_VECTOR_PART_MAX_BYTE_SIZE / 8);
    pv_unit_9(PARTIAL_VECTOR_PART_MAX_BYTE_SIZE / 8 + 10);
    pv_unit_9(PARTIAL_VECTOR_PART_MAX_BYTE_SIZE / 8 * 10 + 10);

    pv_unit_10(10);
    pv_unit_10(PARTIAL_VECTOR_PART_MAX_BYTE_SIZE / 8);
    pv_unit_10(PARTIAL_VECTOR_PART_MAX_BYTE_SIZE / 8 + 10);
    pv_unit_10(PARTIAL_VECTOR_PART_MAX_BYTE_SIZE / 8 * 10 + 10);

    pv_unit_11(10);
    pv_unit_11(PARTIAL_VECTOR_PART_MAX_BYTE_SIZE / 8);
    pv_unit_11(PARTIAL_VECTOR_PART_MAX_BYTE_SIZE / 8 + 10);
    pv_unit_11(PARTIAL_VECTOR_PART_MAX_BYTE_SIZE / 8 * 10 + 10);
}

#include <algorithm>

int main() {
    partial_vector_unit_tests();

    // std::vector<int> aa;

    partial_vector<int> aa({ 5, 8, 2, 3 });
    aa.to_vector() auto dd  = aa.begin() + 5;
    auto                dd3 = aa.end() + 8;
    auto                dd4 = dd3 - dd;

    std::sort(aa.begin(), aa.end(), std::greater<>());

    for (auto& d : aa) {
        std::cout << d << std::endl;
    }

    return 0;
}