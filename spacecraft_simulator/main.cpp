#include <QQmlApplicationEngine>
#include <QGuiApplication>
#include <QQmlContext>
#include <iostream>


#include "SpacecraftSimulator.h"

using namespace std;

int main(int argc, char *argv[]){

    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    engine.loadFromModule("SpacecraftSimulator", "Main");

    return app.exec();
}