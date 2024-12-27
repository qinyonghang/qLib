#pragma once

#ifdef QNOEXCEPTION
#define QTHROW_EXCEPTION(fmt, ...)
#else

#include <cstdio>
#include <exception>
#include <string>
#include <typeinfo>

#include "spdlog/spdlog.h"

#define QMESSAGE_MAX_LENGTH (512)

class QException final : public std::exception {
protected:
    char __what[QMESSAGE_MAX_LENGTH];

public:
    QException(char const* file, int line, char const* message) {
        snprintf(__what, sizeof(__what), "[%s:%d]%s", file, line, message);
    }

    ~QException() = default;

    char const* what() const noexcept override { return __what; }
};

template <typename format_string_t, typename... Args>
void qthrow_exception(char const* file, int line, format_string_t fmt, Args&&... args) {
    throw QException(file, line, fmt::format(fmt, std::forward<Args>(args)...).c_str());
}

#define QTHROW_EXCEPTION(fmt, ...) qthrow_exception(__FILE__, __LINE__, fmt, ##__VA_ARGS__)

#endif

#define QCTHROW_EXCEPTION(fmt, ...)                                                                \
    QTHROW_EXCEPTION("{}: " fmt, typeid(*this).name(), ##__VA_ARGS__)
#define QCMTHROW_EXCEPTION(fmt, ...)                                                               \
    QTHROW_EXCEPTION("{}::{}: " fmt, typeid(*this).name(), __func__, ##__VA_ARGS__)
