#ifdef QT_QML_DEBUG
#include <QtQuick>
#endif

#include <QQuickView>
#include <QScopedPointer>
#include <QGuiApplication>
#include <QtQml>
#include <QQmlEngine>
#include <QDebug>

#include <sailfishapp.h>

#include "secretshandler.h"
#include "chatgptclient.h"
#include "chatstorage.h"
#include "databasemanager.h"
#include "chatmessage.h"
#include "settings.h"
#include "logginghandler.h"
#include "logsexporter.h"

int main(int argc, char *argv[])
{
    QScopedPointer<QGuiApplication> app(SailfishApp::application(argc, argv));
    QScopedPointer<QQuickView> v(SailfishApp::createView());

    DatabaseManager::createAndMigrate();

    const auto loggingHandler = new LoggingHandler(app.data());
    loggingHandler->debug("App started");

    v->rootContext()->setContextProperty("logger", loggingHandler);
    v->rootContext()->setContextProperty("settings", new Settings(app.data()));

#ifdef QT_DEBUG
   v->rootContext()->setContextProperty("isDebug", true);
#else
   v->rootContext()->setContextProperty("isDebug", false);
#endif

    qmlRegisterType<SecretsHandler>("cz.chrastecky.chatgpt", 1, 0, "SecretsHandler");
    qmlRegisterType<ChatGptClient>("cz.chrastecky.chatgpt", 1, 0, "ChatGptClient");
    qmlRegisterType<ChatStorage>("cz.chrastecky.chatgpt", 1, 0, "ChatStorage");
    qmlRegisterType<LogsExporter>("cz.chrastecky.logs", 1, 0, "LogsExporter");

    qmlRegisterUncreatableType<ChatMessage>("cz.chrastecky.chatgpt", 1, 0, "ChatMessage", "Uncreatable");
    qmlRegisterUncreatableType<LoggingHandler>("cz.chrastecky.chatgpt", 1, 0, "LogLevel", "Uncreatable"); // only register it to get access to enum constants

    v->setSource(SailfishApp::pathToMainQml());
    v->show();

    return app->exec();
}
