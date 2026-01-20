#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "datamanager.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    engine.rootContext()->setContextProperty("dataManager", &DataManager::instance());

    const QUrl url(QStringLiteral("qrc:/ALDA_FINAL/main.qml"));
    engine.load(url);
    if (engine.rootObjects().isEmpty()) return -1;
    return app.exec();
}
