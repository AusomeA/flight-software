#pragma once
#include <QObject>
#include <QQmlEngine>

namespace SharedTypes{
    Q_NAMESPACE
    QML_ELEMENT

    enum class Mode{
        nominal,
        lowPower,
        safe
    };
    Q_ENUM_NS(Mode)
}