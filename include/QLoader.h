#pragma once

#include <sys/stat.h>

#include <memory>
#include <string>
#include <fstream>

#include "QObject.h"
#include "QException.h"

template <typename QType = uint8_t>
class QLoader final : public QObject {
public:
    using QObject::QObject;

    QLoader(std::string const& filepath, QObject* parent = nullptr);
    ~QLoader() = default;

    int init(std::string const& filepath);

    std::vector<QType>& data();
    std::vector<QType> const& data() const { return const_cast<QLoader*>(this)->data(); }

protected:
    struct QLoaderImpl : public QObject {
        std::vector<QType> data;
    };

    QObjectPtr __impl;
};

template <typename QType>
QLoader<QType>::QLoader(std::string const& filepath, QObject* parent) : QObject(parent) {
    int32_t result(init(filepath));
    if (result != 0) {
        QCMTHROW_EXCEPTION("init return {}... model_path = {}", result, filepath);
        return;
    }
}

template <typename QType>
int32_t QLoader<QType>::init(std::string const& filepath) {
    int32_t result{0};

    do {
        struct stat st;
        if (stat(filepath.c_str(), &st) != 0) {
            result = -1;
            break;
        }

        auto impl = std::make_shared<QLoaderImpl>();

        impl->data.resize(st.st_size / sizeof(QType));

        std::ifstream file{filepath};
        if (!file.is_open()) {
            result = -2;
            break;
        }

        file.read(reinterpret_cast<char*>(impl->data.data()), st.st_size);
        if (file.bad()) {
            result = -3;
            break;
        }

        __impl = impl;
    } while (0);

    return result;
}

template <typename QType>
std::vector<QType>& QLoader<QType>::data() {
    auto impl = std::dynamic_pointer_cast<QLoaderImpl>(__impl);
    if (!impl) {
        QCMTHROW_EXCEPTION("impl is nullptr");
    }

    return impl->data;
}
