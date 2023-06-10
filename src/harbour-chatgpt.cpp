#ifdef QT_QML_DEBUG
#include <QtQuick>
#endif

#include <QQuickView>
#include <QScopedPointer>
#include <QGuiApplication>
#include <QtQml>
#include <QQmlEngine>
#include <QDebug>
#include <QTranslator>

#include <sailfishapp.h>

#include "secretshandler.h"
#include "openaiclient.h"
#include "chatstorage.h"
#include "databasemanager.h"
#include "chatmessage.h"
#include "settings.h"
#include "logginghandler.h"
#include "logsexporter.h"
#include "localehelper.h"

const QStringList extractTranslationLanguages() {
    const auto translationFiles = QString(APP_TRANSLATIONS).split(" ");
    QStringList result {"en"};

    for (const auto &file : translationFiles) {
        result.append(file.mid(QString("translations/harbour-chatgpt-").size(), 2));
    }

    return result;
}

const QLocale getLocale(Settings *settings) {
    QLocale result;
    auto language = settings->language();
    if (language.isEmpty()) {
        result = QLocale::system();
    } else {
        result = QLocale(language);
    }

    return result;
}

int main(int argc, char *argv[])
{
    QScopedPointer<QGuiApplication> app(SailfishApp::application(argc, argv));
    QScopedPointer<QQuickView> v(SailfishApp::createView());

    DatabaseManager::createAndMigrate();

    auto settings = new Settings(app.data());
    auto locale = getLocale(settings);

    auto translator = new QTranslator(app.data());
    if (translator->load("harbour-chatgpt-" + locale.name().split("_")[0], LOCALIZATIONS_DIR)) {
        app->installTranslator(translator);
    }

    const auto loggingHandler = new LoggingHandler(app.data());
    loggingHandler->debug("App started");

    v->rootContext()->setContextProperty("logger", loggingHandler);
    v->rootContext()->setContextProperty("settings", settings);
    v->rootContext()->setContextProperty("languages", extractTranslationLanguages());

#ifdef QT_DEBUG
   v->rootContext()->setContextProperty("isDebug", true);
#else
   v->rootContext()->setContextProperty("isDebug", false);
#endif

    qmlRegisterType<SecretsHandler>("cz.chrastecky.chatgpt", 1, 0, "SecretsHandler");
    qmlRegisterType<OpenAiClient>("cz.chrastecky.chatgpt", 1, 0, "ChatGptClient");
    qmlRegisterType<ChatStorage>("cz.chrastecky.chatgpt", 1, 0, "ChatStorage");
    qmlRegisterType<LogsExporter>("cz.chrastecky.logs", 1, 0, "LogsExporter");
    qmlRegisterType<LocaleHelper>("cz.chrastecky.chatgpt", 1, 0, "LocaleHelper");

    qmlRegisterUncreatableType<ChatMessage>("cz.chrastecky.chatgpt", 1, 0, "ChatMessage", "Uncreatable");
    qmlRegisterUncreatableType<LoggingHandler>("cz.chrastecky.chatgpt", 1, 0, "LogLevel", "Uncreatable"); // only register it to get access to enum constants

    v->setSource(SailfishApp::pathToMainQml());
    v->show();

    return app->exec();
}
