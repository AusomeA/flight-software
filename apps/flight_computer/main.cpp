#include <QQmlApplicationEngine>
#include <QGuiApplication>

#include "FlightComputer.h"

int main(int argc, char *argv[]){

    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    engine.loadFromModule("FlightComputer", "Main");

    return app.exec();
}