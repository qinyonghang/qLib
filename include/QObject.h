#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <numeric>
#include <sstream>
#include <vector>

#ifdef WITH_NEON
#include <arm_neon.h>
#else
using float32_t = float;
using float64_t = double;
#endif

template <typename Type>
bool exists(Type&& _path) {
    std::filesystem::path path{_path};
    return std::filesystem::exists(path);
}

template <typename Type>
inline Type align(Type value, size_t alignment) {
    return (((value) + ((alignment)-1)) & ~((alignment)-1));
}

template <typename Type>
inline Type align_32(Type value) {
    return align(value, 32);
}

template <typename Type>
std::string serialize(Type const* value, size_t size) {
    std::stringstream out{};

    out << "[";
    for (auto i = 0u; i < size; ++i) {
        out << value[i];
        if (i != size - 1) {
            out << ",";
        }
    }
    out << "]";

    return out.str();
}

template <typename QVec>
std::string serialize(QVec const& arr) {
    return serialize(arr.data(), arr.size());
}

template <typename Type = float64_t>
Type sigmoid(Type x) {
    return static_cast<Type>(1.0) / (static_cast<Type>(1.0) + exp(-x));
}

template <typename Type = float64_t>
std::tuple<std::vector<Type>, std::vector<size_t>> topk(std::vector<Type> const& vec, size_t k) {
    std::vector<int> indices(vec.size());
    std::iota(indices.begin(), indices.end(), 0);
    std::partial_sort(indices.begin(), indices.begin() + k, indices.end(),
                      [&vec](int a, int b) { return vec[a] > vec[b]; });

    std::vector<Type> topk_values(k);
    std::vector<size_t> topk_indices(k);
    for (auto i = 0u; i < k; ++i) {
        topk_values[i] = vec[indices[i]];
        topk_indices[i] = indices[i];
    }
    return {topk_values, topk_indices};
}

class QObject {
public:
    QObject(QObject* parent = nullptr);
    virtual ~QObject() = 0;

protected:
    QObject* __parent{nullptr};
};

using QObjectPtr = std::shared_ptr<QObject>;

template <typename T>
struct QTraits;

template <typename QDerived>
class QAlgorithm : public QObject {
public:
    virtual typename QTraits<QDerived>::return_type operator()() = 0;
};
