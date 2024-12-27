#include "QObject.h"

QObject::QObject(QObject* parent) : __parent{parent} {}

QObject::~QObject() = default;
