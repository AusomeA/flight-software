#include <QQmlApplicationEngine>
#include <QGuiApplication>
#include <QQmlContext>
#include <iostream>


#include "SpacecraftSimulator.h"

using namespace std;

int main(int argc, char *argv[]){
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    SpacecraftSimulator simulator;
    simulator.Start();

    engine.rootContext()->setContextProperty("readouts", simulator.Readouts());
    /*
    QString log;

    log += "Initial State:\n" + simulator.StateText() + "\n\n\n";
    
    log += "Updating simulation by 5 seconds...\n\n";

    simulator.Update(5.0);

    log += "State after update:\n" + simulator.StateText()+ "\n\n\n";
    
    engine.rootContext()->setContextProperty("logText", log);
    */
    engine.loadFromModule("SpacecraftSimulator", "Main");

    return app.exec();
}