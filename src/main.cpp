#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>

#include "appcontroller.h"
#include "bootstrap.h"
#include "brewadapter.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    app.setOrganizationName(QStringLiteral("YAS"));
    app.setApplicationName(QStringLiteral("yas-brew"));
    app.setApplicationDisplayName(QStringLiteral("Yet Another Store for Brew"));

    QQuickStyle::setStyle(QStringLiteral("Basic"));
    yas::loadBundledFonts();

    BrewAdapter adapter;
    yas::AppController controller(&adapter);

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty(QStringLiteral("App"), &controller);
    engine.loadFromModule("YasBrew", "Main");
    return engine.rootObjects().isEmpty() ? 1 : app.exec();
}
