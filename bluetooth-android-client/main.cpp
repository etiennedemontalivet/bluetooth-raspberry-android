#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "bluetoothclient.h"
#include <QLoggingCategory>

int main(int argc, char *argv[])
{
    QLoggingCategory::setFilterRules("default.debug=true");

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    qmlRegisterType<BluetoothClient>("Home", 1, 0, "Bluetooth");

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
