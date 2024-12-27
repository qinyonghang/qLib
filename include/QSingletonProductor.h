#pragma once

#include "QObject.h"

template <typename QType>
class QSingletonProductor final : public QObject {
protected:
    QType __instance;

    QSingletonProductor() = default;
    QSingletonProductor(QSingletonProductor const&) = default;
    QSingletonProductor(QSingletonProductor&&) = default;
    QSingletonProductor& operator=(QSingletonProductor const&) = default;
    QSingletonProductor& operator=(QSingletonProductor&&) = default;

    template <typename... QArgs>
    QSingletonProductor(QArgs&&... args) : __instance{std::forward<QArgs>(args)...} {}

public:
    static QType& get_instance() {
        static QSingletonProductor<QType> productor;
        return productor.__instance;
    }

    template <typename... QArgs>
    static QType& get_instance(QArgs&&... args) {
        static QSingletonProductor<QType> productor{std::forward<QArgs>(args)...};
        return productor.__instance;
    }

    ~QSingletonProductor() = default;
};
