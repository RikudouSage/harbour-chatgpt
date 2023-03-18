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

int main(int argc, char *argv[])
{
    QScopedPointer<QGuiApplication> app(SailfishApp::application(argc, argv));
    QScopedPointer<QQuickView> v(SailfishApp::createView());

    DatabaseManager::createAndMigrate();

    qmlRegisterType<SecretsHandler>("cz.chrastecky.chatgpt", 1, 0, "SecretsHandler");
    qmlRegisterType<ChatGptClient>("cz.chrastecky.chatgpt", 1, 0, "ChatGptClient");
    qmlRegisterType<ChatStorage>("cz.chrastecky.chatgpt", 1, 0, "ChatStorage");
    qmlRegisterUncreatableType<ChatMessage>("cz.chrastecky.chatgpt", 1, 0, "ChatMessage", "Uncreatable");

    v->setSource(SailfishApp::pathToMainQml());
    v->show();

    return app->exec();
}
