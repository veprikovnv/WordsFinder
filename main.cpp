#include "manager.h"
#include "infostruct.h"
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

int main(int argc, char *argv[])
{
    // регистрируем тип данных для передачи между
    // тредом чтения и менеджером
    qRegisterMetaType<QVector<Info>>("QVector<Info>");

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    // установка имени компании (для настроек)
    QCoreApplication::setOrganizationName("VeprikovNV");
    // установка имени приложения (для настроек)
    QCoreApplication::setApplicationName("Words Finder");

    QGuiApplication app(argc, argv);

    Manager manager;

    QQmlApplicationEngine engine;

    // загружаем указатель на менеджер в контекст для установки соединения
    engine.rootContext()->setContextProperty("manager", &manager);

    // передаём в менеджер указатель на контекст
    manager.setContext(engine.rootContext());

    engine.load(QUrl("qrc:/main.qml"));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
